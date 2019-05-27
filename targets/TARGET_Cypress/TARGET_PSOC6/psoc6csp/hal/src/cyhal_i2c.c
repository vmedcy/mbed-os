/*******************************************************************************
* File Name: cyhal_i2c.c
*
* Description:
* Provides a high level interface for interacting with the Cypress I2C. This is
* a wrapper around the lower level PDL API.
* 
********************************************************************************
* \copyright
* Copyright 2018-2019 Cypress Semiconductor Corporation
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
*******************************************************************************/

#include <stdlib.h>
#include "cyhal_i2c.h"
#include "cyhal_hwmgr.h"
#include "cyhal_gpio.h"
#include "cyhal_interconnect.h"
#include "cy_scb_i2c.h"
#include "cy_scb_common.h"

#define PENDING_NONE                    0
#define PENDING_RX                      1
#define PENDING_TX                      2
#define PENDING_TX_RX                   3

/* Default I2C configuration */
static const cy_stc_scb_i2c_config_t default_i2c_config = {
        .i2cMode                   = CY_SCB_I2C_MASTER,
        .useRxFifo                 = true,
        .useTxFifo                 = true,
        .slaveAddress              = 0UL,
        .slaveAddressMask          = 0UL,
        .acceptAddrInFifo          = false,
        .ackGeneralAddr            = false,
        .enableWakeFromSleep       = false
}; 

static cyhal_i2c_irq_event_t cyhal_convert_interrupt_cause(uint32_t pdl_cause);

static cyhal_i2c_t *cyhal_i2c_config_structs[CY_IP_MXSCB_INSTANCES];
static cyhal_i2c_irq_handler_t cyhal_i2c_user_callbacks[CY_IP_MXSCB_INSTANCES];
static void *cyhal_i2c_callback_args[CY_IP_MXSCB_INSTANCES];

static void cyhal_i2c_0_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_1_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_2_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_3_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_4_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_5_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_6_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_7_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_8_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_9_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_10_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_11_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_12_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_13_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_14_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_15_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_16_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_17_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_18_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_i2c_19_cb_wrapper(uint32_t event) __attribute__((unused));

static void cyhal_i2c_0_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_1_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_2_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_3_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_4_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_5_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_6_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_7_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_8_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_9_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_10_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_11_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_12_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_13_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_14_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_15_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_16_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_17_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_18_irq_handler(void) __attribute__((unused));
static void cyhal_i2c_19_irq_handler(void) __attribute__((unused));

static __INLINE  void cyhal_i2c_cb_wrapper_indexed(uint32_t event, uint8_t idx)
{
    cyhal_i2c_irq_event_t anded_events = (cyhal_i2c_irq_event_t)(cyhal_i2c_config_structs[idx]->irq_cause & (uint32_t)cyhal_convert_interrupt_cause(event));
    if (anded_events)
    {
        cyhal_i2c_user_callbacks[idx](cyhal_i2c_callback_args[idx], anded_events);
    }
}
static void cyhal_i2c_0_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 0);
}
static void cyhal_i2c_1_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 1);
}
static void cyhal_i2c_2_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 2);
}
static void cyhal_i2c_3_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 3);
}
static void cyhal_i2c_4_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 4);
}
static void cyhal_i2c_5_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 5);
}
static void cyhal_i2c_6_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 6);
}
static void cyhal_i2c_7_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 7);
}
static void cyhal_i2c_8_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 8);
}
static void cyhal_i2c_9_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 9);
}
static void cyhal_i2c_10_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 10);
}
static void cyhal_i2c_11_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 11);
}
static void cyhal_i2c_12_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 12);
}
static void cyhal_i2c_13_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 13);
}
static void cyhal_i2c_14_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 14);
}
static void cyhal_i2c_15_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 15);
}
static void cyhal_i2c_16_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 16);
}
static void cyhal_i2c_17_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 17);
}
static void cyhal_i2c_18_cb_wrapper(uint32_t event)
{
    cyhal_i2c_cb_wrapper_indexed(event, 18);
}
static void cyhal_i2c_19_cb_wrapper(uint32_t event) 
{
    cyhal_i2c_cb_wrapper_indexed(event, 19);
}

static void (*cyhal_i2c_cb_wrapper_table[CY_IP_MXSCB_INSTANCES])(uint32_t event) =
{
#if (CY_IP_MXSCB_INSTANCES > 0)
    cyhal_i2c_0_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 1)
    cyhal_i2c_1_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 2)
    cyhal_i2c_2_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 3)
    cyhal_i2c_3_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 4)
    cyhal_i2c_4_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 5)
    cyhal_i2c_5_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 6)
    cyhal_i2c_6_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 7)
    cyhal_i2c_7_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 8)
    cyhal_i2c_8_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 9)
    cyhal_i2c_9_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 10)
    cyhal_i2c_10_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 11)
    cyhal_i2c_11_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 12)
    cyhal_i2c_12_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 13)
    cyhal_i2c_13_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 14)
    cyhal_i2c_14_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 15)
    cyhal_i2c_15_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 16)
    cyhal_i2c_16_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 17)
    cyhal_i2c_17_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 18)
    cyhal_i2c_18_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 19)
    cyhal_i2c_19_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 20)
    #error "Unhandled scb count"
#endif
};

static void cyhal_i2c_interrupts_dispatcher_IRQHandler(uint32_t i2c_num)
{
    Cy_SCB_I2C_Interrupt(cyhal_i2c_config_structs[i2c_num]->base, &(cyhal_i2c_config_structs[i2c_num]->context));
}
static void cyhal_i2c_0_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(0);
}
static void cyhal_i2c_1_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(1);
}
static void cyhal_i2c_2_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(2);
}
static void cyhal_i2c_3_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(3);
}
static void cyhal_i2c_4_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(4);
}
static void cyhal_i2c_5_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(5);
}
static void cyhal_i2c_6_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(6);
}
static void cyhal_i2c_7_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(7);
}
static void cyhal_i2c_8_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(8);
}
static void cyhal_i2c_9_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(9);
}
static void cyhal_i2c_10_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(10);
}
static void cyhal_i2c_11_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(11);
}
static void cyhal_i2c_12_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(12);
}
static void cyhal_i2c_13_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(13);
}
static void cyhal_i2c_14_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(14);
}
static void cyhal_i2c_15_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(15);
}
static void cyhal_i2c_16_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(16);
}
static void cyhal_i2c_17_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(17);
}
static void cyhal_i2c_18_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(18);
}
static void cyhal_i2c_19_irq_handler(void)
{
    cyhal_i2c_interrupts_dispatcher_IRQHandler(19);
}
static void (*cyhal_i2c_interrupts_dispatcher_table[CY_IP_MXSCB_INSTANCES])(void) = 
{
#if (CY_IP_MXSCB_INSTANCES > 0)
    cyhal_i2c_0_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 1)
    cyhal_i2c_1_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 2)
    cyhal_i2c_2_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 3)
    cyhal_i2c_3_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 4)
    cyhal_i2c_4_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 5)
    cyhal_i2c_5_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 6)
    cyhal_i2c_6_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 7)
    cyhal_i2c_7_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 8)
    cyhal_i2c_8_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 9)
    cyhal_i2c_9_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 10)
    cyhal_i2c_10_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 11)
    cyhal_i2c_11_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 12)
    cyhal_i2c_12_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 13)
    cyhal_i2c_13_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 14)
    cyhal_i2c_14_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 15)
    cyhal_i2c_15_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 16)
    cyhal_i2c_16_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 17)
    cyhal_i2c_17_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 18)
    cyhal_i2c_18_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 19)
    cyhal_i2c_19_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 20)
    #error "Unhandled scb count"
#endif
};

static cy_rslt_t cyhal_free_resources(cyhal_i2c_t *obj)
{
    cy_rslt_t error_accum = CY_RSLT_SUCCESS;
    cy_rslt_t error;
    if (obj->resource.type != CYHAL_RSC_INVALID)
    {
        error_accum = cyhal_hwmgr_free(&(obj->resource));
        obj->resource.type = CYHAL_RSC_INVALID;
    }
    if (obj->pin_sda != NC)
    {
        error = cyhal_disconnect_pin(obj->pin_sda);
        if (error == CY_RSLT_SUCCESS)
        {
            cyhal_resource_inst_t rsc = cyhal_utils_get_gpio_resource(obj->pin_sda);
            error = cyhal_hwmgr_free(&rsc);
        }
        if (error != CY_RSLT_SUCCESS && error_accum == CY_RSLT_SUCCESS)
        {
            error_accum = error;
        }
    }
    if (obj->pin_scl != NC)
    {
        error = cyhal_disconnect_pin(obj->pin_scl);
        if (error == CY_RSLT_SUCCESS)
        {
            cyhal_resource_inst_t rsc = cyhal_utils_get_gpio_resource(obj->pin_sda);
            error = cyhal_hwmgr_free(&rsc);
        }
        if (error != CY_RSLT_SUCCESS && error_accum == CY_RSLT_SUCCESS)
        {
            error_accum = error;
        }
    }    
    return error_accum;
}

/* Start API implementing */

cy_rslt_t cyhal_i2c_init(cyhal_i2c_t *obj, cyhal_gpio_t sda, cyhal_gpio_t scl, const cyhal_clock_divider_t *clk)
{
	/* If something go wrong, any resource not marked as invalid will be freed. */
    /* Explicitly marked not allocated resources as invalid to prevent freeing them. */
    memset(obj, 0, sizeof(cyhal_i2c_t));
	cyhal_resource_inst_t pin_rsc;

    
    /* Reserve the I2C */
    const cyhal_resource_pin_mapping_t *sda_map = CY_UTILS_GET_RESOURCE(sda, cyhal_pin_map_scb_i2c_sda);
    const cyhal_resource_pin_mapping_t *scl_map = CY_UTILS_GET_RESOURCE(scl, cyhal_pin_map_scb_i2c_scl);
    if ((NULL == sda_map) || (NULL == scl_map) || (sda_map->inst->block_num != scl_map->inst->block_num))
    {
        return CYHAL_I2C_RSLT_ERR_INVALID_PIN;
    }
    obj->resource = *scl_map->inst;
    
    /* Reserve the SDA pin */
    pin_rsc = cyhal_utils_get_gpio_resource(sda);
	cy_rslt_t result = cyhal_hwmgr_reserve(&pin_rsc);
    if (result == CY_RSLT_SUCCESS)
    {
        obj->pin_sda = sda;
    }
    
    /* Reseve the SCL pin */
    if (result == CY_RSLT_SUCCESS)
    {
		pin_rsc = cyhal_utils_get_gpio_resource(scl);
		cy_rslt_t result = cyhal_hwmgr_reserve(&pin_rsc);
		if (result == CY_RSLT_SUCCESS)
		{
			obj->pin_scl = scl;
		}
	}
    
    obj->base = CY_SCB_BASE_ADDRESSES[obj->resource.block_num];
    
    if (result == CY_RSLT_SUCCESS)
    {
        if (clk == NULL)
        {
            result = cyhal_hwmgr_allocate_clock(&(obj->clock), CY_SYSCLK_DIV_8_BIT, true);
        }
        else
        {
            obj->clock = *clk;
        }
         
    }
    if (result == CY_RSLT_SUCCESS)
    {
        result = Cy_SysClk_PeriphAssignDivider(
            (en_clk_dst_t)((uint8_t)PCLK_SCB0_CLOCK + obj->resource.block_num), obj->clock.div_type, obj->clock.div_num);
    }    
    if (result == CY_RSLT_SUCCESS)
    {
        result = cyhal_connect_pin(sda_map);
    }
    if (result == CY_RSLT_SUCCESS)
    {
        result = cyhal_connect_pin(scl_map);
    }

    bool configured = false;
    if (result == CY_RSLT_SUCCESS)
    {
        result = cyhal_hwmgr_is_configured(obj->resource.type, obj->resource.block_num, obj->resource.channel_num, &configured);
    }
    
    if (result == CY_RSLT_SUCCESS && !configured)
    {
        Cy_SCB_I2C_Init(obj->base, &default_i2c_config, &(obj->context));                 
        cyhal_hwmgr_set_configured(obj->resource.type, obj->resource.block_num, obj->resource.channel_num);
    }
    if (result != CY_RSLT_SUCCESS)
    {
        cyhal_free_resources(obj);
    }
    return result;	
}

cy_rslt_t cyhal_i2c_free(cyhal_i2c_t *obj)
{
	cy_rslt_t error_accum = CY_RSLT_SUCCESS;
    cy_rslt_t error = cyhal_free_resources(obj);
    if (error != CY_RSLT_SUCCESS && error_accum == CY_RSLT_SUCCESS)
    {
        error_accum = error;
    }
    error = cyhal_hwmgr_set_unconfigured(obj->resource.type, obj->resource.block_num, obj->resource.channel_num);
    if (error != CY_RSLT_SUCCESS && error_accum == CY_RSLT_SUCCESS)
    {
        error_accum = error;
    }
    error = cyhal_hwmgr_free_clock(&(obj->clock));
    if (error != CY_RSLT_SUCCESS && error_accum == CY_RSLT_SUCCESS)
    {
        error_accum = error;
    }
    return error_accum;
}

cy_rslt_t cyhal_i2c_set_config(cyhal_i2c_t *obj, const cyhal_i2c_cfg_t *cfg) 
{
	cy_stc_scb_i2c_config_t config_structure = default_i2c_config;    
    if (cfg->is_slave) {
        config_structure.i2cMode = CY_SCB_I2C_SLAVE;
    }
    else {
        config_structure.i2cMode = CY_SCB_I2C_MASTER;
    }
    config_structure.slaveAddress  = cfg->address;
	Cy_SCB_I2C_SetDataRate(obj->base, cfg->frequencyhal_hz, Cy_SysClk_PeriphGetFrequency(CY_SYSCLK_DIV_8_BIT, 0UL));

    Cy_SCB_I2C_Init(obj->base, &config_structure, &(obj->context));
    cy_rslt_t result = cyhal_hwmgr_set_configured(obj->resource.type, obj->resource.block_num, obj->resource.channel_num);    
    return result;
}

cy_rslt_t cyhal_i2c_master_send(cyhal_i2c_t *obj, uint16_t dev_addr, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	cy_en_scb_i2c_status_t status = CY_SCB_I2C_SUCCESS;

    /* Start transaction, send dev_addr. */
    if (obj->context.state == CY_SCB_I2C_IDLE) {
        status = Cy_SCB_I2C_MasterSendStart(obj->base, dev_addr, CY_SCB_I2C_WRITE_XFER, timeout, &obj->context);
    }
    if (status == CY_SCB_I2C_SUCCESS) {
        while (size > 0) {
            status = Cy_SCB_I2C_MasterWriteByte(obj->base, *data, timeout, &obj->context);
            if (status != CY_SCB_I2C_SUCCESS) {
                break;
            }
            --size;
            ++data;
        }
        /* SCB in I2C mode is very time sensitive. In practice we have to request STOP after */
        /* each block, otherwise it may break the transmission */
        Cy_SCB_I2C_MasterSendStop(obj->base, timeout, &obj->context);
    }

    if (status != CY_SCB_I2C_SUCCESS) {
        Cy_SCB_I2C_MasterSendStop(obj->base, timeout, &obj->context);
    }

    return status;
}

cy_rslt_t cyhal_i2c_master_recv(cyhal_i2c_t *obj, uint16_t dev_addr, uint8_t *data, uint16_t size, uint32_t timeout)
{
	cy_en_scb_i2c_status_t status = CY_SCB_I2C_SUCCESS;
    cy_en_scb_i2c_command_t ack = CY_SCB_I2C_ACK;

    /* Start transaction, send dev_addr */
    if (obj->context.state == CY_SCB_I2C_IDLE) {
        status = Cy_SCB_I2C_MasterSendStart(obj->base, dev_addr, CY_SCB_I2C_READ_XFER, timeout, &obj->context);
    }
    if (status == CY_SCB_I2C_SUCCESS) {
        while (size > 0) {
            if (size == 1) {
                ack = CY_SCB_I2C_NAK;
            }
            status = Cy_SCB_I2C_MasterReadByte(obj->base, ack, (uint8_t *)data, timeout, &obj->context);
            if (status != CY_SCB_I2C_SUCCESS) {
                break;
            }
            --size;
            ++data;
        }
        /* SCB in I2C mode is very time sensitive. In practice we have to request STOP after */
        /* each block, otherwise it may break the transmission */
        Cy_SCB_I2C_MasterSendStop(obj->base, timeout, &obj->context);
    }

    if (status != CY_SCB_I2C_SUCCESS) {
        Cy_SCB_I2C_MasterSendStop(obj->base, timeout, &obj->context);
    }

    return status;
}

cy_rslt_t cyhal_i2c_slave_send(cyhal_i2c_t *obj, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	if (obj->context.state == CY_SCB_I2C_IDLE) {
        Cy_SCB_I2C_SlaveConfigReadBuf(obj->base, (uint8_t *)data, size, &obj->context);
    }
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_i2c_slave_recv(cyhal_i2c_t *obj, uint8_t *data, uint16_t size, uint32_t timeout)
{
    if (obj->context.state == CY_SCB_I2C_IDLE) {	
        Cy_SCB_I2C_SlaveConfigWriteBuf(obj->base, (uint8_t *)data, size, &obj->context);
    }
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_i2c_mem_write(cyhal_i2c_t *obj, uint16_t address, uint16_t mem_addr, uint16_t mem_addr_size, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	cy_en_scb_i2c_status_t status = CY_SCB_I2C_SUCCESS;

    /* Start transaction, send address. */
    if (obj->context.state == CY_SCB_I2C_IDLE) {
        status = Cy_SCB_I2C_MasterSendStart(obj->base, address, CY_SCB_I2C_WRITE_XFER, timeout, &obj->context);
    }
    /* Send mem_addr - start address in slave memory */
    if (status == CY_SCB_I2C_SUCCESS) {
        status = Cy_SCB_I2C_MasterWriteByte(obj->base, (uint8_t)((mem_addr & 0xFF00) >> 8), timeout, &obj->context);
    }
    if (status == CY_SCB_I2C_SUCCESS) {
        status = Cy_SCB_I2C_MasterWriteByte(obj->base, (uint8_t)(mem_addr & 0x00FF), timeout, &obj->context);
    }
    if (status == CY_SCB_I2C_SUCCESS) {
        while (size > 0) {
            status = Cy_SCB_I2C_MasterWriteByte(obj->base, *data, timeout, &obj->context);
            if (status != CY_SCB_I2C_SUCCESS) {
                break;
            }
            --size;
            ++data;
        }
        /* SCB in I2C mode is very time sensitive. In practice we have to request STOP after */
        /* each block, otherwise it may break the transmission */
        Cy_SCB_I2C_MasterSendStop(obj->base, timeout, &obj->context);
    }
    if (status != CY_SCB_I2C_SUCCESS) {
        Cy_SCB_I2C_MasterSendStop(obj->base, timeout, &obj->context);
    }
    return status;    
}

cy_rslt_t cyhal_i2c_mem_read(cyhal_i2c_t *obj, uint16_t address, uint16_t mem_addr, uint16_t mem_addr_size, uint8_t *data, uint16_t size, uint32_t timeout)
{
	cy_en_scb_i2c_status_t status = CY_SCB_I2C_SUCCESS;
    cy_en_scb_i2c_command_t ack = CY_SCB_I2C_ACK;

    /* Start transaction, send address */
    if (obj->context.state == CY_SCB_I2C_IDLE) {
        status = Cy_SCB_I2C_MasterSendStart(obj->base, address, CY_SCB_I2C_READ_XFER, timeout, &obj->context);
    }
     /* Send mem_addr - start address in slave memory */
    if (status == CY_SCB_I2C_SUCCESS) {
        status = Cy_SCB_I2C_MasterWriteByte(obj->base, (uint8_t)((mem_addr & 0xFF00) >> 8), timeout, &obj->context);
    }
    if (status == CY_SCB_I2C_SUCCESS) {
        status = Cy_SCB_I2C_MasterWriteByte(obj->base, (uint8_t)(mem_addr & 0x00FF), timeout, &obj->context);
    }
    if (status == CY_SCB_I2C_SUCCESS) {
        while (size > 0) {
            if (size == 1) {
                ack = CY_SCB_I2C_NAK;
            }
            status = Cy_SCB_I2C_MasterReadByte(obj->base, ack, (uint8_t *)data, timeout, &obj->context);
            if (status != CY_SCB_I2C_SUCCESS) {
                break;
            }
            --size;
            ++data;
        }
        /* SCB in I2C mode is very time sensitive. In practice we have to request STOP after */
        /* each block, otherwise it may break the transmission */
        Cy_SCB_I2C_MasterSendStop(obj->base, timeout, &obj->context);
    }

    if (status != CY_SCB_I2C_SUCCESS) {
        Cy_SCB_I2C_MasterSendStop(obj->base, timeout, &obj->context);
    }

    return status;
}

cy_rslt_t cyhal_i2c_transfer_async(cyhal_i2c_t *obj, const void *tx, size_t tx_size, void *rx, size_t rx_size, uint16_t address)
{
	if (obj->pending != PENDING_NONE) {
        return 0; //TODO: return reason
    }

    obj->rx_config.slaveAddress = address >> 1;
    obj->tx_config.slaveAddress = address >> 1;

    obj->rx_config.buffer = rx;
    obj->rx_config.bufferSize = rx_size;

    obj->tx_config.buffer = (void *)tx;
    obj->tx_config.bufferSize = tx_size;

    if (tx_size) {
        /* Write first, then read, or write only. */
        if (rx_size > 0) {
            obj->pending = PENDING_TX_RX;
        } else {
            obj->pending = PENDING_TX;
        }
        Cy_SCB_I2C_MasterWrite(obj->base, &obj->tx_config, &obj->context);
    } else if (rx_size) {
        /* Read transaction */
        obj->pending = PENDING_RX;
        Cy_SCB_I2C_MasterRead(obj->base, &obj->rx_config, &obj->context);
    }
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_i2c_abort_async(cyhal_i2c_t *obj)
{
	if (obj->pending != PENDING_NONE) {
        if (obj->pending == PENDING_RX) {
            Cy_SCB_I2C_MasterAbortRead(obj->base, &obj->context);
        } else {
            Cy_SCB_I2C_MasterAbortWrite(obj->base, &obj->context);
        }
    }
    return CY_RSLT_SUCCESS;
}

static cyhal_i2c_irq_event_t cyhal_convert_interrupt_cause(uint32_t pdl_cause)
{    
    cyhal_i2c_irq_event_t cause = CYHAL_I2C_IRQ_NONE;
    switch(pdl_cause)
    {        
        case CY_SCB_I2C_SLAVE_READ_EVENT:
        	cause = CYHAL_I2C_SLAVE_READ_EVENT;
        	break;
        case CY_SCB_I2C_SLAVE_WRITE_EVENT:
        	cause = CYHAL_I2C_SLAVE_WRITE_EVENT;
        	break;
        case CY_SCB_I2C_SLAVE_RD_IN_FIFO_EVENT:
        	cause = CYHAL_I2C_SLAVE_RD_IN_FIFO_EVENT;
        	break;
        case CY_SCB_I2C_SLAVE_RD_BUF_EMPTY_EVENT:
        	cause = CYHAL_I2C_SLAVE_RD_BUF_EMPTY_EVENT;
        	break;
        case CY_SCB_I2C_SLAVE_RD_CMPLT_EVENT:
        	cause = CYHAL_I2C_SLAVE_RD_CMPLT_EVENT;
        	break;
        case CY_SCB_I2C_SLAVE_WR_CMPLT_EVENT:
        	cause = CYHAL_I2C_SLAVE_WR_CMPLT_EVENT;
        	break;
        case CY_SCB_I2C_SLAVE_ERR_EVENT:
        	cause = CYHAL_I2C_SLAVE_ERR_EVENT;
        	break;
        case CY_SCB_I2C_MASTER_WR_IN_FIFO_EVENT:
        	cause = CYHAL_I2C_MASTER_WR_IN_FIFO_EVENT;
        	break;
        case CY_SCB_I2C_MASTER_WR_CMPLT_EVENT:
        	cause = CYHAL_I2C_MASTER_WR_CMPLT_EVENT;
        	break;
        case CY_SCB_I2C_MASTER_RD_CMPLT_EVENT:
        	cause = CYHAL_I2C_MASTER_RD_CMPLT_EVENT;
        	break;
        case CY_SCB_I2C_MASTER_ERR_EVENT:
        	cause = CYHAL_I2C_MASTER_ERR_EVENT;
        	break;		
        default:
            cause = CYHAL_I2C_IRQ_NONE;
        	break;
    }
    return cause;
}

cy_rslt_t cyhal_i2c_register_irq(cyhal_i2c_t *obj, cyhal_i2c_irq_handler handler, void *handler_arg)
{
	uint8_t idx = obj->resource.block_num;
    cyhal_i2c_config_structs[idx] = obj;
    cyhal_i2c_user_callbacks[idx] = handler;
    cyhal_i2c_callback_args[idx] = handler_arg;
    Cy_SCB_I2C_RegisterEventCallback(obj->base, cyhal_i2c_cb_wrapper_table[idx], &(obj->context));
    if (NVIC_GetEnableIRQ(CY_SCB_IRQ_N[idx]) == 0UL)
    {
        /* Default interrupt priority of 7 (lowest possible priority). */
        cy_stc_sysint_t irqCfg = {CY_SCB_IRQ_N[idx], 7};

        Cy_SysInt_Init(&irqCfg, cyhal_i2c_interrupts_dispatcher_table[idx]);
        NVIC_EnableIRQ(CY_SCB_IRQ_N[idx]);
    }
    
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_i2c_irq_enable(cyhal_i2c_t *obj, cyhal_i2c_irq_event_t event, bool enable)
{
	if (enable)
    {
        obj->irq_cause |= event;
    }
    else
    {
        obj->irq_cause &= ~event;
    }
    return CY_RSLT_SUCCESS;
}

