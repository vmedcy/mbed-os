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
#include <stdlib.h>
#include "whd_bus_common.h"
#include "whd_chip_reg.h"
#include "whd_sdio.h"
#include "whd_chip_constants.h"
#include "whd_int.h"
#include "whd_chip.h"
#include "whd_bus_protocol_interface.h"
#include "whd_debug.h"
#include "whd_rtos_interface.h"
#include "whd_buffer_api.h"
#include "whd_resource_if.h"
#include "whd_resource_api.h"
#include "whd_types_int.h"


/******************************************************
*                      Macros
******************************************************/
#define WHD_SAVE_INTERRUPTS(flags) do { UNUSED_PARAMETER(flags); } while (0);
#define WHD_RESTORE_INTERRUPTS(flags) do { } while (0);

/******************************************************
*             Constants
******************************************************/
#define INDIRECT_BUFFER_SIZE                    (1024)
#define WHD_BUS_ROUND_UP_ALIGNMENT              (64)
#define WHD_BUS_MAX_TRANSFER_SIZE               (WHD_BUS_MAX_BACKPLANE_TRANSFER_SIZE)

#define WHD_BUS_WLAN_ALLOW_SLEEP_INVALID_MS  ( (uint32_t)-1 )

/******************************************************
*             Structures
******************************************************/

struct whd_bus_common_info
{
    whd_bool_t bus_is_up;

    whd_time_t delayed_bus_release_deadline;
    whd_bool_t delayed_bus_release_scheduled;
    uint32_t delayed_bus_release_timeout_ms;
    volatile uint32_t delayed_bus_release_timeout_ms_request;

    uint32_t backplane_window_current_base_address;
    whd_bool_t bus_flow_control;
    volatile whd_bool_t resource_download_abort;

};

/******************************************************
*             Variables
******************************************************/

/******************************************************
*             Function declarations
******************************************************/
static whd_result_t whd_bus_common_download_resource(whd_driver_t whd_driver, whd_resource_type_t resource,
                                                     whd_bool_t direct_resource, uint32_t address);

/******************************************************
*             Function definitions
******************************************************/

whd_bool_t whd_bus_is_up(whd_driver_t whd_driver)
{
    return whd_driver->bus_common_info->bus_is_up;
}

void whd_bus_set_state(whd_driver_t whd_driver, whd_bool_t state)
{
    whd_driver->bus_common_info->bus_is_up = state;
}

whd_result_t whd_bus_set_flow_control(whd_driver_t whd_driver, uint8_t value)
{
    if (value != 0)
    {
        whd_driver->bus_common_info->bus_flow_control = WHD_TRUE;
    }
    else
    {
        whd_driver->bus_common_info->bus_flow_control = WHD_FALSE;
    }
    return WHD_SUCCESS;
}

whd_bool_t whd_bus_is_flow_controlled(whd_driver_t whd_driver)
{
    return whd_driver->bus_common_info->bus_flow_control;
}

static whd_result_t whd_bus_common_download_resource(whd_driver_t whd_driver, whd_resource_type_t resource,
                                                     whd_bool_t direct_resource, uint32_t address)
{
    uint32_t transfer_progress;
    whd_buffer_t buffer;
    uint32_t block_size;
    whd_result_t result = WHD_SUCCESS;
    uint8_t *image;
    uint32_t image_size;
    uint8_t *packet;
    uint16_t transfer_size = 0;
    uint32_t segment_size;
    uint32_t blocks_count = 0;
    uint32_t i;
    uint32_t size_out;
    uint32_t reset_instr = 0;

    result = whd_resource_size(whd_driver, resource, &image_size);

    if (result != WHD_SUCCESS)
    {
        WPRINT_WHD_ERROR( ("Fatal error: download_resource doesn't exist, %s failed at line %d \n", __func__,
                           __LINE__) );
        goto exit;
    }

    if (image_size <= 0)
    {
        WPRINT_WHD_ERROR( ("Fatal error: download_resource cannot load with invalid size, %s failed at line %d \n",
                           __func__, __LINE__) );
        result = WHD_BADARG;
        goto exit;
    }

    /* Transfer firmware image into the RAM */
    transfer_progress = 0;
    result = whd_get_resource_block_size(whd_driver, resource, &block_size);
    if (result != WHD_SUCCESS)
    {
        WPRINT_WHD_ERROR( ("Fatal error: download_resource block size not know, %s failed at line %d \n", __func__,
                           __LINE__) );
        goto exit;
    }

    result = whd_get_resource_no_of_blocks(whd_driver, resource, &blocks_count);
    if (result != WHD_SUCCESS)
    {
        WPRINT_WHD_ERROR( ("Fatal error: download_resource blocks count not known, %s failed at line %d \n", __func__,
                           __LINE__) );
        goto exit;
    }

    do
    {
        result = whd_host_buffer_get(whd_driver, &buffer, WHD_NETWORK_TX,
                                     (unsigned short)(block_size + sizeof(whd_buffer_header_t) ), WHD_FALSE);
    } while ( (result == WHD_BUFFER_UNAVAILABLE_PERMANENT) && ( (block_size >>= 1) > 1 ) );

    /*
     * In case of direct read the no of blocks is one and we would be getting a pointer pointing to entire resource
     * In case of indirect read the no of blocks is the size of the resource/ size of blocks,
     * get resource block would return pointer to the block
     */

    for (i = 0; i < blocks_count; i++)
    {
        packet = (uint8_t *)whd_buffer_get_current_piece_data_pointer(whd_driver, buffer);

        CHECK_RETURN(whd_get_resource_block(whd_driver, resource, (const uint8_t **)&image, &size_out) );

        if (resource == WHD_RESOURCE_WLAN_FIRMWARE)
        {
            memcpy(packet + sizeof(whd_buffer_header_t), &image[0], size_out);
            transfer_size = (uint16_t)block_size;

        }
        else if (resource == WHD_RESOURCE_WLAN_NVRAM)
        {
            memcpy(packet + sizeof(whd_buffer_header_t), &image[transfer_progress], block_size);
            transfer_size = (uint16_t)MIN_OF(block_size, image_size - transfer_progress);
        }
        /* Round up the size of the chunk */
        transfer_size = (uint16_t)ROUND_UP(transfer_size, WHD_BUS_ROUND_UP_ALIGNMENT);
        segment_size = transfer_size;

        if (address != 0)
        {
            if ( (resource == WHD_RESOURCE_WLAN_FIRMWARE) && (reset_instr == 0) )
            {
                /* Copy the starting address of the firmware into a global variable */
                reset_instr = *( (uint32_t *)(packet + sizeof(whd_buffer_header_t) ) );
            }
        }

        for (; segment_size != 0;
             segment_size -= transfer_size, packet += transfer_size, transfer_progress += transfer_size,
             address += transfer_size)
        {
            if (whd_driver->bus_common_info->resource_download_abort == WHD_TRUE)
            {
                WPRINT_WHD_ERROR( ("Download_resource is aborted; terminating after %lu iterations\n",
                                   transfer_progress) );
                CHECK_RETURN(whd_buffer_release(whd_driver, buffer, WHD_NETWORK_TX) );
                result = WHD_UNFINISHED;
                goto exit;
            }
            transfer_size = (uint16_t)MIN_OF(whd_bus_get_max_transfer_size(whd_driver), segment_size);
            result = whd_bus_set_backplane_window(whd_driver, address);

            if (result != WHD_SUCCESS)
            {
                CHECK_RETURN(whd_buffer_release(whd_driver, buffer, WHD_NETWORK_TX) );
                goto exit;
            }
            result = whd_bus_transfer_bytes(whd_driver, BUS_WRITE, BACKPLANE_FUNCTION,
                                            (address & BACKPLANE_ADDRESS_MASK), transfer_size,
                                            (whd_transfer_bytes_packet_t *)(packet + sizeof(whd_buffer_queue_ptr_t) ) );
            if (result != WHD_SUCCESS)
            {
                CHECK_RETURN(whd_buffer_release(whd_driver, buffer, WHD_NETWORK_TX) );
                goto exit;
            }
        }
    }

    CHECK_RETURN(whd_buffer_release(whd_driver, buffer, WHD_NETWORK_TX) );
    /* Below part of the code is applicable to arm_CR4 type chips only
     * The CR4 chips by default firmware is not loaded at 0. So we need
     * load the first 32 bytes with the offset of the firmware load address
     * which is been copied before during the firmware download
     */
    if ( (address != 0) && (reset_instr != 0) )
    {
        /* write address 0 with reset instruction */
        result = whd_bus_write_backplane_value(whd_driver, 0, sizeof(reset_instr), reset_instr);

        if (result == WHD_SUCCESS)
        {
            uint32_t tmp;

            /* verify reset instruction value */
            result = whd_bus_read_backplane_value(whd_driver, 0, sizeof(tmp), (uint8_t *)&tmp);

            if ( (result == WHD_SUCCESS) && (tmp != reset_instr) )
            {
                WPRINT_WHD_ERROR( ("%s: Failed to write 0x%08lx to addr 0\n", __FUNCTION__, reset_instr) );
                WPRINT_WHD_ERROR( ("%s: contents of addr 0 is 0x%08lx\n", __FUNCTION__, tmp) );
                return WHD_WLAN_SDIO_ERROR;
            }
        }
    }
exit: return result;
}

void whd_bus_common_info_init(whd_driver_t whd_driver)
{
    struct whd_bus_common_info *bus_common = (struct whd_bus_common_info *)malloc(sizeof(struct whd_bus_common_info) );

    if (bus_common != NULL)
    {
        whd_driver->bus_common_info = bus_common;

        bus_common->delayed_bus_release_deadline = 0;
        bus_common->delayed_bus_release_scheduled = WHD_FALSE;
        bus_common->delayed_bus_release_timeout_ms = PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS;
        bus_common->delayed_bus_release_timeout_ms_request = WHD_BUS_WLAN_ALLOW_SLEEP_INVALID_MS;
        bus_common->backplane_window_current_base_address = 0;

        bus_common->bus_is_up = WHD_FALSE;
        bus_common->bus_flow_control = WHD_FALSE;

        bus_common->resource_download_abort = WHD_FALSE;
    }
    else
    {
        WPRINT_WHD_ERROR( ("Memory allocation failed for whd_bus_common_info in %s\n", __FUNCTION__) );
    }
}

void whd_delayed_bus_release_schedule_update(whd_driver_t whd_driver, whd_bool_t is_scheduled)
{
    whd_driver->bus_common_info->delayed_bus_release_scheduled = is_scheduled;
    whd_driver->bus_common_info->delayed_bus_release_deadline = 0;
}

uint32_t whd_bus_handle_delayed_release(whd_driver_t whd_driver)
{
    uint32_t time_until_release = 0;
    struct whd_bus_common_info *bus_common = whd_driver->bus_common_info;

    if (bus_common->delayed_bus_release_timeout_ms_request != WHD_BUS_WLAN_ALLOW_SLEEP_INVALID_MS)
    {
        whd_bool_t schedule =
            ( (bus_common->delayed_bus_release_scheduled != 0) ||
              (bus_common->delayed_bus_release_deadline != 0) ) ? WHD_TRUE : WHD_FALSE;
        uint32_t flags;

        WHD_SAVE_INTERRUPTS(flags);
        bus_common->delayed_bus_release_timeout_ms = bus_common->delayed_bus_release_timeout_ms_request;
        bus_common->delayed_bus_release_timeout_ms_request = WHD_BUS_WLAN_ALLOW_SLEEP_INVALID_MS;
        WHD_RESTORE_INTERRUPTS(flags);

        DELAYED_BUS_RELEASE_SCHEDULE(whd_driver, schedule);
    }

    if (bus_common->delayed_bus_release_scheduled == WHD_TRUE)
    {
        bus_common->delayed_bus_release_scheduled = WHD_FALSE;

        if (bus_common->delayed_bus_release_timeout_ms != 0)
        {
            bus_common->delayed_bus_release_deadline = whd_rtos_get_time( ) +
                                                       bus_common->delayed_bus_release_timeout_ms;
            time_until_release = bus_common->delayed_bus_release_timeout_ms;
        }
    }
    else if (bus_common->delayed_bus_release_deadline != 0)
    {
        whd_time_t now = whd_rtos_get_time( );

        if (bus_common->delayed_bus_release_deadline - now <= bus_common->delayed_bus_release_timeout_ms)
        {
            time_until_release = bus_common->delayed_bus_release_deadline - now;
        }

        if (time_until_release == 0)
        {
            bus_common->delayed_bus_release_deadline = 0;
        }
    }

    if (time_until_release != 0)
    {
        if (whd_bus_is_up(whd_driver) == WHD_FALSE)
        {
            time_until_release = 0;
        }
        else if (whd_bus_platform_mcu_power_save_deep_sleep_enabled(whd_driver) )
        {
            time_until_release = 0;
        }
    }

    return time_until_release;
}

whd_bool_t whd_bus_platform_mcu_power_save_deep_sleep_enabled(whd_driver_t whd_driver)
{
    return WHD_FALSE;
}

void whd_bus_init_backplane_window(whd_driver_t whd_driver)
{
    whd_driver->bus_common_info->backplane_window_current_base_address = 0;
}

whd_result_t whd_bus_write_wifi_firmware_image(whd_driver_t whd_driver)
{
    whd_result_t result = WHD_SUCCESS;
    uint32_t ram_start_address;

    /* Pass the ram_start_address to the firmware Download
     * CR4 chips have offset and CM3 starts from 0 */

    ram_start_address = GET_C_VAR(whd_driver, ATCM_RAM_BASE_ADDRESS);
    result = whd_bus_common_download_resource(whd_driver, WHD_RESOURCE_WLAN_FIRMWARE, WHD_FALSE, ram_start_address);

    if (result != WHD_SUCCESS)
        WPRINT_WHD_ERROR( ("Bus common resource download failed, %s failed at %d \n", __func__, __LINE__) );

    return result;
}

whd_result_t whd_bus_write_wifi_nvram_image(whd_driver_t whd_driver)
{
    uint32_t image_size;
    uint32_t img_base;
    uint32_t img_end;

    /* Get the size of the variable image */
    CHECK_RETURN(whd_resource_size(whd_driver, WHD_RESOURCE_WLAN_NVRAM, &image_size) );

    /* Round up the size of the image */
    image_size = ROUND_UP(image_size, WHD_BUS_ROUND_UP_ALIGNMENT);

    /* Write image */
    img_end = GET_C_VAR(whd_driver, CHIP_RAM_SIZE) - 4;
    img_base = (img_end - image_size);
    img_base += GET_C_VAR(whd_driver, ATCM_RAM_BASE_ADDRESS);

    CHECK_RETURN(whd_bus_common_download_resource(whd_driver, WHD_RESOURCE_WLAN_NVRAM, WHD_FALSE, img_base) );

    /* Write the variable image size at the end */
    image_size = (~(image_size / 4) << 16) | (image_size / 4);

    img_end += GET_C_VAR(whd_driver, ATCM_RAM_BASE_ADDRESS);

    CHECK_RETURN(whd_bus_write_backplane_value(whd_driver, (uint32_t)img_end, 4, image_size) );

    return WHD_SUCCESS;
}

void whd_bus_set_resource_download_halt(whd_driver_t whd_driver, whd_bool_t halt)
{
    whd_driver->bus_common_info->resource_download_abort = halt;
}

/*
 * Update the backplane window registers
 */
whd_result_t whd_bus_set_backplane_window(whd_driver_t whd_driver, uint32_t addr)
{
    whd_result_t result = WHD_BUS_WRITE_REGISTER_ERROR;
    uint32_t base = addr & ( (uint32_t) ~BACKPLANE_ADDRESS_MASK );
    const uint32_t upper_32bit_mask = 0xFF000000;
    const uint32_t upper_middle_32bit_mask = 0x00FF0000;
    const uint32_t lower_middle_32bit_mask = 0x0000FF00;
    struct whd_bus_common_info *bus_common_info = whd_driver->bus_common_info;

    if (base == bus_common_info->backplane_window_current_base_address)
    {
        return WHD_SUCCESS;
    }
    if ( (base & upper_32bit_mask) != (bus_common_info->backplane_window_current_base_address & upper_32bit_mask) )
    {
        if (WHD_SUCCESS !=
            (result = whd_bus_write_register_value(whd_driver, BACKPLANE_FUNCTION, SDIO_BACKPLANE_ADDRESS_HIGH,
                                                   (uint8_t)1, (base >> 24) ) ) )
        {
            WPRINT_WHD_ERROR( ("Failed to write register value to the bus, %s failed at %d \n", __func__, __LINE__) );
            return result;
        }
        /* clear old */
        bus_common_info->backplane_window_current_base_address &= ~upper_32bit_mask;
        /* set new */
        bus_common_info->backplane_window_current_base_address |= (base & upper_32bit_mask);
    }

    if ( (base & upper_middle_32bit_mask) !=
         (bus_common_info->backplane_window_current_base_address & upper_middle_32bit_mask) )
    {
        if (WHD_SUCCESS !=
            (result = whd_bus_write_register_value(whd_driver, BACKPLANE_FUNCTION, SDIO_BACKPLANE_ADDRESS_MID,
                                                   (uint8_t)1, (base >> 16) ) ) )
        {
            WPRINT_WHD_ERROR( ("Failed to write register value to the bus, %s failed at %d \n", __func__, __LINE__) );
            return result;
        }
        /* clear old */
        bus_common_info->backplane_window_current_base_address &= ~upper_middle_32bit_mask;
        /* set new */
        bus_common_info->backplane_window_current_base_address |= (base & upper_middle_32bit_mask);
    }

    if ( (base & lower_middle_32bit_mask) !=
         (bus_common_info->backplane_window_current_base_address & lower_middle_32bit_mask) )
    {
        if (WHD_SUCCESS !=
            (result = whd_bus_write_register_value(whd_driver, BACKPLANE_FUNCTION, SDIO_BACKPLANE_ADDRESS_LOW,
                                                   (uint8_t)1, (base >> 8) ) ) )
        {
            WPRINT_WHD_ERROR( ("Failed to write register value to the bus, %s failed at %d \n", __func__, __LINE__) );
            return result;
        }

        /* clear old */
        bus_common_info->backplane_window_current_base_address &= ~lower_middle_32bit_mask;
        /* set new */
        bus_common_info->backplane_window_current_base_address |= (base & lower_middle_32bit_mask);
    }

    return WHD_SUCCESS;
}

/* Default implementation of WHD bus resume function, which does nothing */
whd_result_t whd_bus_resume_after_deep_sleep(whd_driver_t whd_driver)
{
    whd_assert("In order to support deep-sleep platform need to implement this function", 0);
    return WHD_UNSUPPORTED;
}

whd_result_t whd_bus_transfer_backplane_bytes(whd_driver_t whd_driver, whd_bus_transfer_direction_t direction,
                                              uint32_t address, uint32_t size, /*@in@*//*@out@*/ uint8_t *data)
{
    whd_buffer_t pkt_buffer = NULL;
    uint8_t *packet;
    uint32_t transfer_size;
    uint32_t remaining_buf_size;
    uint32_t window_offset_address;
    whd_result_t result;

    result = whd_host_buffer_get(whd_driver, &pkt_buffer, (direction == BUS_READ) ? WHD_NETWORK_RX : WHD_NETWORK_TX,
                                 ( uint16_t )(whd_bus_get_max_transfer_size(whd_driver) +
                                              whd_bus_backplane_read_padd_size(
                                                  whd_driver) + MAX_BUS_HEADER_SIZE), WHD_TRUE);
    if (result != WHD_SUCCESS)
    {
        goto done;
    }
    packet = (uint8_t *)whd_buffer_get_current_piece_data_pointer(whd_driver, pkt_buffer);

    result = whd_ensure_wlan_bus_is_up(whd_driver);
    if (result != WHD_SUCCESS)
    {
        goto done;
    }

    for (remaining_buf_size = size; remaining_buf_size != 0;
         remaining_buf_size -= transfer_size, address += transfer_size)
    {
        transfer_size = (remaining_buf_size >
                         whd_bus_get_max_transfer_size(whd_driver) ) ? whd_bus_get_max_transfer_size(whd_driver) :
                        remaining_buf_size;

        /* Check if the transfer crosses the backplane window boundary */
        window_offset_address = address & BACKPLANE_ADDRESS_MASK;
        if ( (window_offset_address + transfer_size) > BACKPLANE_ADDRESS_MASK )
        {
            /* Adjust the transfer size to within current window */
            transfer_size = BACKPLANE_WINDOW_SIZE - window_offset_address;
        }
        result = whd_bus_set_backplane_window(whd_driver, address);
        if (result != WHD_SUCCESS)
        {
            goto done;
        }

        if (direction == BUS_WRITE)
        {
            memcpy( ( (whd_transfer_bytes_packet_t *)packet )->data, data + size - remaining_buf_size, transfer_size );
            result = whd_bus_transfer_bytes(whd_driver, direction, BACKPLANE_FUNCTION,
                                            (address & BACKPLANE_ADDRESS_MASK), (uint16_t)transfer_size,
                                            (whd_transfer_bytes_packet_t *)packet);
            if (result != WHD_SUCCESS)
            {
                goto done;
            }
        }
        else
        {
            result = whd_bus_transfer_bytes(whd_driver, direction, BACKPLANE_FUNCTION,
                                            (address & BACKPLANE_ADDRESS_MASK),
                                            ( uint16_t )(transfer_size + whd_bus_backplane_read_padd_size(whd_driver) ),
                                            (whd_transfer_bytes_packet_t *)packet);
            if (result != WHD_SUCCESS)
            {
                WPRINT_WHD_ERROR( ("whd_bus_transfer_bytes failed\n") );
                goto done;
            }
            memcpy(data + size - remaining_buf_size, (uint8_t *)( (whd_transfer_bytes_packet_t *)packet )->data +
                   whd_bus_backplane_read_padd_size(whd_driver), transfer_size);
        }
    }

done: CHECK_RETURN(whd_bus_set_backplane_window(whd_driver, CHIPCOMMON_BASE_ADDRESS) );
    if (pkt_buffer != NULL)
    {
        CHECK_RETURN(whd_buffer_release(whd_driver, pkt_buffer,
                                        (direction == BUS_READ) ? WHD_NETWORK_RX : WHD_NETWORK_TX) );
    }
    CHECK_RETURN(result);

    return WHD_SUCCESS;
}

