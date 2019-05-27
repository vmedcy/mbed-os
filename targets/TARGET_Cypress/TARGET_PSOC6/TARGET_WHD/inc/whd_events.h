/*
 * Copyright 2019 Cypress Semiconductor Corporation
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

/** @file
 *
 */

#ifndef INCLUDED_WHD_EVENTS_API_H
#define INCLUDED_WHD_EVENTS_API_H

#include "whd.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* event_type */
#define WLC_E_SET_SSID                  =   0, /* indicates status of set SSID */
#define WLC_E_DEAUTH                    =   5, /* 802.11 DEAUTH request */
#define WLC_E_DEAUTH_IND                =   6, /* 802.11 DEAUTH indication */
#define WLC_E_ASSOC_IND                 =   8, /* 802.11 ASSOC indication */
#define WLC_E_REASSOC_IND               =  10, /* 802.11 REASSOC indication */
#define WLC_E_DISASSOC                  =  11, /* 802.11 DISASSOC request */
#define WLC_E_DISASSOC_IND              =  12, /* 802.11 DISASSOC indication */
#define WLC_E_LINK                      =  16, /* generic link indication */
#define WLC_E_PROBREQ_MSG               =  44, /* probe request received */
#define WLC_E_PSK_SUP                   =  46, /* WPA Handshake fail */
#define WLC_E_P2P_DISC_LISTEN_COMPLETE  =  55, /* listen state expires */
#define WLC_E_ACTION_FRAME              =  59, /* Action frame Rx */
#define WLC_E_ACTION_FRAME_COMPLETE     =  60, /* Action frame Tx complete */
#define WLC_E_ESCAN_RESULT              =  69, /* escan result event */
#define WLC_E_CSA_COMPLETE_IND          =  80,  /* 802.11 CHANNEL SWITCH ACTION completed */

/* Status codes - Applicable for any event type */
#define WLC_E_STATUS_SUCCESS        0   /* operation was successful */
#define WLC_E_STATUS_FAIL           1   /* operation failed */
#define WLC_E_STATUS_TIMEOUT        2   /* operation timed out */
#define WLC_E_STATUS_NO_NETWORKS    3   /* failed due to no matching network found */
#define WLC_E_STATUS_ABORT          4   /* operation was aborted */
#define WLC_E_STATUS_NO_ACK         5   /* protocol failure: packet not ack'd */
#define WLC_E_STATUS_UNSOLICITED    6   /* AUTH or ASSOC packet was unsolicited */
#define WLC_E_STATUS_ATTEMPT        7   /* attempt to assoc to an auto auth configuration */
#define WLC_E_STATUS_PARTIAL        8   /* scan results are incomplete */
#define WLC_E_STATUS_NEWSCAN        9   /* scan aborted by another scan */
#define WLC_E_STATUS_NEWASSOC       10  /* scan aborted due to assoc in progress */
#define WLC_E_STATUS_11HQUIET       11  /* 802.11h quiet period started */
#define WLC_E_STATUS_SUPPRESS       12  /* user disabled scanning (WLC_SET_SCANSUPPRESS) */
#define WLC_E_STATUS_NOCHANS        13  /* no allowable channels to scan */
#define WLC_E_STATUS_CCXFASTRM      14  /* scan aborted due to CCX fast roam */
#define WLC_E_STATUS_CS_ABORT       15  /* abort channel select */
#define WLC_E_STATUS_ERROR          16  /* request failed due to error */
#define WLC_E_STATUS_INVALID        0xff  /* Invalid status code to init variables. */

/* Status code for event WLC_E_PSK_SUP */
typedef enum sup_auth_status
{
    /* Basic supplicant authentication states */
    WLC_SUP_DISCONNECTED = 0,
    WLC_SUP_CONNECTING,
    WLC_SUP_IDREQUIRED,
    WLC_SUP_AUTHENTICATING,
    WLC_SUP_AUTHENTICATED,
    WLC_SUP_KEYXCHANGE,
    WLC_SUP_KEYED,
    WLC_SUP_TIMEOUT,
    WLC_SUP_LAST_BASIC_STATE,

    /* Extended supplicant authentication states */
    /* Waiting to receive handshake msg M1 */
    WLC_SUP_KEYXCHANGE_WAIT_M1 = WLC_SUP_AUTHENTICATED,
    /* Preparing to send handshake msg M2 */
    WLC_SUP_KEYXCHANGE_PREP_M2 = WLC_SUP_KEYXCHANGE,
    /* Waiting to receive handshake msg M3 */
    WLC_SUP_KEYXCHANGE_WAIT_M3 = WLC_SUP_LAST_BASIC_STATE,
    WLC_SUP_KEYXCHANGE_PREP_M4, /* Preparing to send handshake msg M4 */
    WLC_SUP_KEYXCHANGE_WAIT_G1, /* Waiting to receive handshake msg G1 */
    WLC_SUP_KEYXCHANGE_PREP_G2  /* Preparing to send handshake msg G2 */
} sup_auth_status_t;

#define WHD_MSG_IFNAME_MAX 16

#pragma pack(1)
typedef struct whd_event_eth_hdr
{
    uint16_t subtype;      /** Vendor specific..32769 */
    uint16_t length;
    uint8_t version;       /** Version is 0 */
    uint8_t oui[3];        /**  OUI */
    uint16_t usr_subtype;  /** user specific Data */
} whd_event_eth_hdr_t;

typedef struct whd_event_ether_header
{
    whd_mac_t destination_address;
    whd_mac_t source_address;
    uint16_t ethertype;
} whd_event_ether_header_t;

struct whd_event_msg
{
    uint16_t version;
    uint16_t flags;                 /* see flags below */
    uint32_t event_type;            /* Message (see below) */
    uint32_t status;                /* Status code (see below) */
    uint32_t reason;                /* Reason code (if applicable) */
    uint32_t auth_type;             /* WLC_E_AUTH */
    uint32_t datalen;               /* data buf */
    whd_mac_t addr;                 /* Station address (if applicable) */
    char ifname[WHD_MSG_IFNAME_MAX];               /* name of the packet incoming interface */
    uint8_t ifidx;                                 /* destination OS i/f index */
    uint8_t bsscfgidx;                             /* source bsscfg index */
};

/* used by driver msgs */
typedef struct whd_event
{
    whd_event_ether_header_t eth;
    whd_event_eth_hdr_t eth_evt_hdr;
    whd_event_header_t whd_event;
    /* data portion follows */
} whd_event_t;

#pragma pack()

/**
 * Event handler prototype definition
 *
 * @param[out] whd_event_header_t : whd event header
 * @param[out] uint8_t*           : event data
 * @param[out] handler_user_data  : semaphore data
 */
typedef void *(*whd_event_handler_t)(whd_interface_t ifp, const whd_event_header_t *event_header,
                                     const uint8_t *event_data, /*@null@*/ void *handler_user_data);

/** Registers a handler to receive event callbacks.
 *
 *  This function registers a callback handler to be notified when
 *  a particular event is received.
 *
 *  Alternately the function clears callbacks for given event type.
 *
 *  @note : Currently each event may only be registered to one handler
 *          and there is a limit to the number of simultaneously registered
 *          events
 *
 *  @param  ifp               : Pointer to handle instance of whd interface
 *  @param  event_nums        : The event types that are to trigger the handler
 *                              See @ref whd_event_num_t for available events.
 *                              Must be defined in a global constant.
 *  @param  handler_func      : A function pointer to the new handler callback,
 *                              or NULL if callbacks are to be disabled for the given event type
 *  @param  handler_user_data : A pointer value which will be passed to the event handler function
 *                              at the time an event is triggered (NULL is allowed)
 *
 *  @return WHD_SUCCESS or Error code
 */
uint32_t whd_wifi_set_event_handler(whd_interface_t ifp, const uint32_t *event_type, whd_event_handler_t handler_func,
                                    void *handler_user_data);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* ifndef  */
