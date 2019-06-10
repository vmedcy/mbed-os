/* WHD STAION implementation of NetworkInterfaceAPI
 * Copyright (c) 2017-2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstring>
#include <algorithm>
#include "WhdSTAInterface.h"
#include "nsapi.h"
#include "lwipopts.h"
#include "lwip/etharp.h"
#include "lwip/ethip6.h"
#include "rtos.h"
#include "whd_emac.h"
#include "whd_wifi_api.h"

#define SCAN_RESULT_BUFF_SIZE     (30u)
#define CMP_MAC( a, b )  (((((unsigned char*)a)[0])==(((unsigned char*)b)[0]))&& \
                          ((((unsigned char*)a)[1])==(((unsigned char*)b)[1]))&& \
                          ((((unsigned char*)a)[2])==(((unsigned char*)b)[2]))&& \
                          ((((unsigned char*)a)[3])==(((unsigned char*)b)[3]))&& \
                          ((((unsigned char*)a)[4])==(((unsigned char*)b)[4]))&& \
                          ((((unsigned char*)a)[5])==(((unsigned char*)b)[5])))

struct whd_scan_userdata {
    Semaphore *sema;
    WiFiAccessPoint *aps;
    whd_scan_result_t result_buff[SCAN_RESULT_BUFF_SIZE];
    unsigned count;
    unsigned offset;
    whd_interface_t ifp;
    bool scan_in_progress;
};

static whd_scan_userdata interal_scan_data;
static whd_scan_result_t internal_scan_result;
static whd_scan_result_t *result_ptr = &internal_scan_result;

extern "C" void whd_emac_wifi_link_state_changed(bool state_up, whd_interface_t ifp);


static int whd_toerror(whd_result_t res)
{
    switch (res) {
        case WHD_NETWORK_NOT_FOUND:
            return NSAPI_ERROR_NO_SSID;
        case WHD_NOT_AUTHENTICATED:
        case WHD_EAPOL_KEY_PACKET_M3_TIMEOUT:
            return NSAPI_ERROR_AUTH_FAILURE;
        default:
            return -res;
    }
}

static nsapi_security_t whd_tosecurity(whd_security_t sec)
{
    switch (sec & (WEP_ENABLED | WPA_SECURITY | WPA2_SECURITY)) {
        case WEP_ENABLED:
            return NSAPI_SECURITY_WEP;
        case WPA_SECURITY:
            return NSAPI_SECURITY_WPA;
        case WPA2_SECURITY:
            return NSAPI_SECURITY_WPA2;
        default:
            if (sec == WHD_SECURITY_OPEN) {
                return NSAPI_SECURITY_NONE;
            } else {
                return NSAPI_SECURITY_UNKNOWN;
            }
    }
}

static whd_security_t whd_fromsecurity(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return WHD_SECURITY_OPEN;
        case NSAPI_SECURITY_WEP:
            return WHD_SECURITY_WEP_PSK;
        case NSAPI_SECURITY_WPA:
            return WHD_SECURITY_WPA_MIXED_PSK;
        case NSAPI_SECURITY_WPA2:
            return WHD_SECURITY_WPA2_AES_PSK;
        case NSAPI_SECURITY_WPA_WPA2:
            return WHD_SECURITY_WPA2_MIXED_PSK;
        default:
            return WHD_SECURITY_UNKNOWN;
    }
}

WhdSTAInterface::WhdSTAInterface(WHD_EMAC &emac, OnboardNetworkStack &stack)
    : EMACInterface(emac, stack),
      _ssid("\0"),
      _pass("\0"),
      _security(NSAPI_SECURITY_NONE),
      _whd_emac(emac)
{
}

nsapi_error_t WhdSTAInterface::connect(
    const char *ssid, const char *pass,
    nsapi_security_t security,
    uint8_t channel)
{
    int err = set_channel(channel);
    if (err) {
        return err;
    }

    err = set_credentials(ssid, pass, security);
    if (err) {
        return err;
    }

    return connect();
}

nsapi_error_t WhdSTAInterface::set_credentials(const char *ssid, const char *pass, nsapi_security_t security)
{
    if ((ssid == NULL) ||
            (strlen(ssid) == 0) ||
            (pass == NULL && security != NSAPI_SECURITY_NONE) ||
            (strlen(pass) == 0 && security != NSAPI_SECURITY_NONE) ||
            (strlen(pass) > 63 && (security == NSAPI_SECURITY_WPA2 || security == NSAPI_SECURITY_WPA || security == NSAPI_SECURITY_WPA_WPA2))
       ) {
        return NSAPI_ERROR_PARAMETER;
    }

    memset(_ssid, 0, sizeof(_ssid));
    strncpy(_ssid, ssid, sizeof(_ssid));

    memset(_pass, 0, sizeof(_pass));
    strncpy(_pass, pass, sizeof(_pass));

    _security = security;

    return NSAPI_ERROR_OK;
}

nsapi_error_t WhdSTAInterface::connect()
{

#define MAX_RETRY_COUNT    ( 5 )
    int i;

    // initialize wiced, this is noop if already init
    if (!_whd_emac.powered_up) {
        _whd_emac.power_up();
    }

    if (!_interface) {
        nsapi_error_t err = _stack.add_ethernet_interface(_emac, true, &_interface);
        if (err != NSAPI_ERROR_OK) {
            _interface = NULL;
            return err;
        }
        _interface->attach(_connection_status_cb);
    }

    if ((_ssid == NULL) ||
            (strlen(_ssid) == 0)) {
        return NSAPI_ERROR_PARAMETER;
    }

    // setup ssid
    whd_ssid_t ssid;
    strncpy((char *)ssid.value, _ssid, SSID_NAME_SIZE);
    ssid.value[SSID_NAME_SIZE - 1] = '\0';
    ssid.length = strlen((char *)ssid.value);

    // choose network security
    whd_security_t security = whd_fromsecurity(_security);

    // join the network
    whd_result_t res;
    for (i = 0; i < MAX_RETRY_COUNT; i++) {
        res = (whd_result_t)whd_wifi_join(_whd_emac.ifp,
                                          &ssid,
                                          security,
                                          (const uint8_t *)_pass, strlen(_pass));
        if (res == WHD_SUCCESS) {
            break;
        }
    }

    if (res != WHD_SUCCESS) {
        return whd_toerror(res);
    }

    /* Use DHCP to get IP address? */
    set_dhcp((_ip_address[0] ? false : true));

    if (whd_wifi_is_ready_to_transceive(_whd_emac.ifp) == WHD_SUCCESS) {
        whd_emac_wifi_link_state_changed(true, _whd_emac.ifp);
    }

    // bring up
    return _interface->bringup(_dhcp,
                               _ip_address[0] ? _ip_address : 0,
                               _netmask[0] ? _netmask : 0,
                               _gateway[0] ? _gateway : 0,
                               DEFAULT_STACK);
}


nsapi_error_t WhdSTAInterface::disconnect()
{
    if (!_interface) {
        return NSAPI_STATUS_DISCONNECTED;
    }

    // bring down
    int err = EMACInterface::disconnect();
    if (err) {
        return err;
    }

    // leave network
    whd_result_t res = whd_wifi_leave(_whd_emac.ifp);
    if (res != WHD_SUCCESS) {
        return whd_toerror(res);
    }

    return NSAPI_ERROR_OK;
}

int8_t WhdSTAInterface::get_rssi()
{
    int32_t rssi;
    whd_result_t res;

    res = (whd_result_t)whd_wifi_get_rssi(_whd_emac.ifp, &rssi);
    if (res != 0) {
        return 0;
    }

    return (int8_t)rssi;
}

static void whd_scan_handler(whd_scan_result_t **result_ptr,
                             void *user_data, whd_scan_status_t status)
{

    whd_scan_userdata *data = (whd_scan_userdata *)user_data;

    /* Even after stopping scan, some results will still come as results are already present in the queue */
    if (data->scan_in_progress == false) {
        return;
    }

    // finished scan, either succesfully or through an abort
    if (status != WHD_SCAN_INCOMPLETE) {
        data->scan_in_progress = false;
        data->sema->release();
        return;
    }

    // can't really keep anymore scan results
    if (data->count > 0 && data->offset >= std::min(data->count, SCAN_RESULT_BUFF_SIZE)) {
        /* We can not abort the scan as this function is getting executed in WHD context,
           Note that to call any WHD API, caller function should not in WHD context */
        return;
    }

    whd_scan_result_t *record = *result_ptr;

    for (unsigned int i = 0; i < data->offset; i++) {
        if (CMP_MAC(data->result_buff[i].BSSID.octet, record->BSSID.octet)) {
            return;
        }
    }

    if (data->count > 0) {
        // get ap stats
        nsapi_wifi_ap ap;

        uint8_t length = record->SSID.length;
        if (length < sizeof(ap.ssid) - 1) {
            length = sizeof(ap.ssid) - 1;
        }
        memcpy(ap.ssid, record->SSID.value, length);
        ap.ssid[length] = '\0';

        memcpy(ap.bssid, record->BSSID.octet, sizeof(ap.bssid));

        ap.security = whd_tosecurity(record->security);
        ap.rssi = record->signal_strength;
        ap.channel = record->channel;
        data->aps[data->offset] = WiFiAccessPoint(ap);
    }

    // store to result_buff for future duplication removal
    data->result_buff[data->offset] = *record;
    data->offset += 1;

}


int WhdSTAInterface::scan(WiFiAccessPoint *aps, unsigned count)
{
    // initialize wiced, this is noop if already init
    if (!_whd_emac.powered_up) {
        _whd_emac.power_up();
    }

    interal_scan_data.sema = new Semaphore();
    interal_scan_data.aps = aps;
    interal_scan_data.count = count;
    interal_scan_data.offset = 0;
    interal_scan_data.ifp = _whd_emac.ifp;
    interal_scan_data.scan_in_progress = true;
    whd_result_t res;


    res = (whd_result_t)whd_wifi_scan(_whd_emac.ifp, WHD_SCAN_TYPE_ACTIVE, WHD_BSS_TYPE_ANY,
                                      NULL, NULL, NULL, NULL, whd_scan_handler, (whd_scan_result_t **) &result_ptr, &interal_scan_data);
    if (res != WHD_SUCCESS) {
        delete interal_scan_data.sema;
        return whd_toerror(res);
    }

    int tok = interal_scan_data.sema->wait();
    if (tok < 1) {
        delete interal_scan_data.sema;
        return NSAPI_ERROR_WOULD_BLOCK;
    }

    delete interal_scan_data.sema;
    return interal_scan_data.offset;
}

