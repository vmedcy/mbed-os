/*******************************************************************************
* File Name: cyhal_uart.c
*
* Description:
* Provides a high level interface for interacting with the Cypress UART. This is
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
#include "cyhal_implementation.h"

#define UART_OVERSAMPLE                 12

/* Default UART configuration */
static const cy_stc_scb_uart_config_t default_uart_config = {
    .uartMode                   = CY_SCB_UART_STANDARD,
    .enableMutliProcessorMode   = false,
    .smartCardRetryOnNack       = false,
    .irdaInvertRx               = false,
    .irdaEnableLowPowerReceiver = false,

    .oversample                 = UART_OVERSAMPLE,

    .enableMsbFirst             = false,
    .dataWidth                  = 8UL,
    .parity                     = CY_SCB_UART_PARITY_NONE,
    .stopBits                   = CY_SCB_UART_STOP_BITS_1,
    .enableInputFilter          = false,
    .breakWidth                 = 11UL,
    .dropOnFrameError           = false,
    .dropOnParityError          = false,

    .receiverAddress            = 0x0UL,
    .receiverAddressMask        = 0x0UL,
    .acceptAddrInFifo           = false,

    .enableCts                  = false,
    .ctsPolarity                = CY_SCB_UART_ACTIVE_LOW,
    .rtsRxFifoLevel             = 20UL,
    .rtsPolarity                = CY_SCB_UART_ACTIVE_LOW,

    .rxFifoTriggerLevel         = 0UL,  /* Level triggers when at least one element is in FIFO */
    .rxFifoIntEnableMask        = 0x0UL,

    .txFifoTriggerLevel         = 63UL, /* Level triggers when half-fifo is half empty */
    .txFifoIntEnableMask        = 0x0UL
};

static cyhal_uart_irq_event_t cyhal_convert_interrupt_cause(uint32_t pdl_cause);

static cyhal_uart_t *cyhal_uart_config_structs[CY_IP_MXSCB_INSTANCES];
static cyhal_uart_irq_handler_t cyhal_uart_user_callbacks[CY_IP_MXSCB_INSTANCES];
static void *cyhal_uart_callback_args[CY_IP_MXSCB_INSTANCES];

static void cyhal_uart_0_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_1_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_2_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_3_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_4_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_5_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_6_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_7_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_8_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_9_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_10_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_11_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_12_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_13_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_14_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_15_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_16_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_17_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_18_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_uart_19_cb_wrapper(uint32_t event) __attribute__((unused));

static void cyhal_uart_0_irq_handler(void) __attribute__((unused));
static void cyhal_uart_1_irq_handler(void) __attribute__((unused));
static void cyhal_uart_2_irq_handler(void) __attribute__((unused));
static void cyhal_uart_3_irq_handler(void) __attribute__((unused));
static void cyhal_uart_4_irq_handler(void) __attribute__((unused));
static void cyhal_uart_5_irq_handler(void) __attribute__((unused));
static void cyhal_uart_6_irq_handler(void) __attribute__((unused));
static void cyhal_uart_7_irq_handler(void) __attribute__((unused));
static void cyhal_uart_8_irq_handler(void) __attribute__((unused));
static void cyhal_uart_9_irq_handler(void) __attribute__((unused));
static void cyhal_uart_10_irq_handler(void) __attribute__((unused));
static void cyhal_uart_11_irq_handler(void) __attribute__((unused));
static void cyhal_uart_12_irq_handler(void) __attribute__((unused));
static void cyhal_uart_13_irq_handler(void) __attribute__((unused));
static void cyhal_uart_14_irq_handler(void) __attribute__((unused));
static void cyhal_uart_15_irq_handler(void) __attribute__((unused));
static void cyhal_uart_16_irq_handler(void) __attribute__((unused));
static void cyhal_uart_17_irq_handler(void) __attribute__((unused));
static void cyhal_uart_18_irq_handler(void) __attribute__((unused));
static void cyhal_uart_19_irq_handler(void) __attribute__((unused));

static __INLINE  void cyhal_uart_cb_wrapper_indexed(uint32_t event, uint8_t idx)
{
    if (NULL != cyhal_uart_user_callbacks[idx])
    {
        cyhal_uart_irq_event_t anded_events = (cyhal_uart_irq_event_t)(cyhal_uart_config_structs[idx]->irq_cause & (uint32_t)cyhal_convert_interrupt_cause(event));
        if (anded_events)
        {
            cyhal_uart_user_callbacks[idx](cyhal_uart_callback_args[idx], anded_events);
        }
    }
}
static void cyhal_uart_0_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 0);
}
static void cyhal_uart_1_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 1);
}
static void cyhal_uart_2_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 2);
}
static void cyhal_uart_3_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 3);
}
static void cyhal_uart_4_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 4);
}
static void cyhal_uart_5_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 5);
}
static void cyhal_uart_6_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 6);
}
static void cyhal_uart_7_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 7);
}
static void cyhal_uart_8_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 8);
}
static void cyhal_uart_9_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 9);
}
static void cyhal_uart_10_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 10);
}
static void cyhal_uart_11_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 11);
}
static void cyhal_uart_12_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 12);
}
static void cyhal_uart_13_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 13);
}
static void cyhal_uart_14_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 14);
}
static void cyhal_uart_15_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 15);
}
static void cyhal_uart_16_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 16);
}
static void cyhal_uart_17_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 17);
}
static void cyhal_uart_18_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 18);
}
static void cyhal_uart_19_cb_wrapper(uint32_t event)
{
    cyhal_uart_cb_wrapper_indexed(event, 19);
}

static void (*cyhal_uart_cb_wrapper_table[CY_IP_MXSCB_INSTANCES])(uint32_t event) =
{
#if (CY_IP_MXSCB_INSTANCES > 0)
    cyhal_uart_0_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 1)
    cyhal_uart_1_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 2)
    cyhal_uart_2_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 3)
    cyhal_uart_3_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 4)
    cyhal_uart_4_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 5)
    cyhal_uart_5_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 6)
    cyhal_uart_6_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 7)
    cyhal_uart_7_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 8)
    cyhal_uart_8_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 9)
    cyhal_uart_9_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 10)
    cyhal_uart_10_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 11)
    cyhal_uart_11_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 12)
    cyhal_uart_12_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 13)
    cyhal_uart_13_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 14)
    cyhal_uart_14_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 15)
    cyhal_uart_15_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 16)
    cyhal_uart_16_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 17)
    cyhal_uart_17_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 18)
    cyhal_uart_18_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 19)
    cyhal_uart_19_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 20)
    #error "Unhandled scb count"
#endif
};

static void cyhal_uart_interrupts_dispatcher_IRQHandler(uint32_t uart_num)
{
    Cy_SCB_UART_Interrupt(cyhal_uart_config_structs[uart_num]->base, &(cyhal_uart_config_structs[uart_num]->context));
}
static void cyhal_uart_0_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(0);
}
static void cyhal_uart_1_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(1);
}
static void cyhal_uart_2_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(2);
}
static void cyhal_uart_3_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(3);
}
static void cyhal_uart_4_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(4);
}
static void cyhal_uart_5_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(5);
}
static void cyhal_uart_6_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(6);
}
static void cyhal_uart_7_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(7);
}
static void cyhal_uart_8_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(8);
}
static void cyhal_uart_9_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(9);
}
static void cyhal_uart_10_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(10);
}
static void cyhal_uart_11_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(11);
}
static void cyhal_uart_12_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(12);
}
static void cyhal_uart_13_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(13);
}
static void cyhal_uart_14_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(14);
}
static void cyhal_uart_15_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(15);
}
static void cyhal_uart_16_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(16);
}
static void cyhal_uart_17_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(17);
}
static void cyhal_uart_18_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(18);
}
static void cyhal_uart_19_irq_handler(void)
{
    cyhal_uart_interrupts_dispatcher_IRQHandler(19);
}
static void (*cyhal_uart_interrupts_dispatcher_table[CY_IP_MXSCB_INSTANCES])(void) =
{
#if (CY_IP_MXSCB_INSTANCES > 0)
    cyhal_uart_0_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 1)
    cyhal_uart_1_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 2)
    cyhal_uart_2_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 3)
    cyhal_uart_3_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 4)
    cyhal_uart_4_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 5)
    cyhal_uart_5_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 6)
    cyhal_uart_6_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 7)
    cyhal_uart_7_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 8)
    cyhal_uart_8_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 9)
    cyhal_uart_9_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 10)
    cyhal_uart_10_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 11)
    cyhal_uart_11_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 12)
    cyhal_uart_12_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 13)
    cyhal_uart_13_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 14)
    cyhal_uart_14_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 15)
    cyhal_uart_15_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 16)
    cyhal_uart_16_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 17)
    cyhal_uart_17_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 18)
    cyhal_uart_18_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 19)
    cyhal_uart_19_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 20)
    #error "Unhandled scb count"
#endif
};

static cy_en_syspm_status_t cyhal_uart_pm_callback(cy_stc_syspm_callback_params_t *params, cy_en_syspm_callback_mode_t mode)
{
    cy_en_syspm_status_t rslt = Cy_SCB_UART_DeepSleepCallback(params, mode);
    cyhal_uart_t *obj = params->context;
    GPIO_PRT_Type *txport = obj->pin_tx != NC ? CYHAL_GET_PORTADDR(obj->pin_tx) : NULL,
                  *rtsport = obj->pin_rts != NC ? CYHAL_GET_PORTADDR(obj->pin_rts) : NULL;
    uint8_t txpin = (uint8_t)CYHAL_GET_PIN(obj->pin_tx), rtspin = (uint8_t)CYHAL_GET_PIN(obj->pin_rts);
    switch (mode)
    {
    case CY_SYSPM_CHECK_READY:
        if (rslt == CY_SYSPM_SUCCESS)
        {
            if (NULL != txport)
            {
                obj->saved_tx_hsiom = Cy_GPIO_GetHSIOM(txport, txpin);
                Cy_GPIO_Clr(txport, txpin);
                Cy_GPIO_SetHSIOM(txport, txpin, HSIOM_SEL_GPIO);
            }
            if (NULL != rtsport)
            {
                obj->saved_rts_hsiom = Cy_GPIO_GetHSIOM(rtsport, rtspin);
                Cy_GPIO_Clr(rtsport, rtspin);
                Cy_GPIO_SetHSIOM(rtsport, rtspin, HSIOM_SEL_GPIO);
            }
        }
        break;
    case CY_SYSPM_CHECK_FAIL: // fallthrough
    case CY_SYSPM_AFTER_TRANSITION:
        if (NULL != txport)
        {
            Cy_GPIO_SetHSIOM(txport, txpin, obj->saved_tx_hsiom);
            Cy_GPIO_Set(txport, txpin);
        }
        if (NULL != rtsport)
        {
            Cy_GPIO_SetHSIOM(rtsport, rtspin, obj->saved_rts_hsiom);
            Cy_GPIO_Set(rtsport, rtspin);
        }
        break;
    default: break;
    }
    return rslt;
}

static cy_en_scb_uart_parity_t convert_parity(cyhal_uart_parity_t parity)
{
    switch (parity)
    {
        case CYHAL_UART_PARITY_NONE:
            return CY_SCB_UART_PARITY_NONE;
        case CYHAL_UART_PARITY_EVEN:
            return CY_SCB_UART_PARITY_EVEN;
        case CYHAL_UART_PARITY_ODD:
            return CY_SCB_UART_PARITY_ODD;
        default:
            return CY_SCB_UART_PARITY_NONE;
    }
}

static cy_en_scb_uart_stop_bits_t convert_stopbits(uint8_t stopbits)
{
    switch (stopbits)
    {
        case 1:
            return CY_SCB_UART_STOP_BITS_1;
        case 2:
            return CY_SCB_UART_STOP_BITS_2;
        case 3:
            return CY_SCB_UART_STOP_BITS_3;
        case 4:
            return CY_SCB_UART_STOP_BITS_4;
        default:
            CY_ASSERT(false);
            return CY_SCB_UART_STOP_BITS_1;
    }
}

cy_rslt_t cyhal_uart_init(cyhal_uart_t *obj, cyhal_gpio_t tx, cyhal_gpio_t rx, const cyhal_clock_divider_t *clk, const cyhal_uart_cfg_t *cfg)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    // If something go wrong, any resource not marked as invalid will be freed.
    // Explicitly marked not allocated resources as invalid to prevent freeing them.
    memset(obj, 0, sizeof(cyhal_uart_t));
    cyhal_resource_inst_t pin_rsc;

    // Reserve the UART
    const cyhal_resource_pin_mapping_t *tx_map = CY_UTILS_GET_RESOURCE(tx, cyhal_pin_map_scb_uart_tx);
    const cyhal_resource_pin_mapping_t *rx_map = CY_UTILS_GET_RESOURCE(rx, cyhal_pin_map_scb_uart_rx);
    if (NULL == tx_map || NULL == rx_map || tx_map->inst->block_num != rx_map->inst->block_num)
    {
        return CYHAL_UART_RSLT_ERR_INVALID_PIN;
    }

    obj->resource = *rx_map->inst;
    

    if (CY_RSLT_SUCCESS != (result = cyhal_hwmgr_reserve(&obj->resource)))
        return result;

    // reserve the TX pin
    pin_rsc = cyhal_utils_get_gpio_resource(tx);
    result = cyhal_hwmgr_reserve(&pin_rsc);
    if (result == CY_RSLT_SUCCESS)
    {
        obj->pin_tx = tx;
    }

    //reseve the RX pin
    if (result == CY_RSLT_SUCCESS)
    {
        pin_rsc = cyhal_utils_get_gpio_resource(rx);
        result = cyhal_hwmgr_reserve(&pin_rsc);
        if (result == CY_RSLT_SUCCESS)
        {
            obj->pin_rx = rx;
        }
    }
    
    obj->pin_cts = NC;
    obj->pin_rts = NC;

    obj->base = CY_SCB_BASE_ADDRESSES[obj->resource.block_num];

    if (result == CY_RSLT_SUCCESS)
    {
        if (clk == NULL)
        {
            obj->is_user_clock = false;
            result = cyhal_hwmgr_allocate_clock(&(obj->clock), CY_SYSCLK_DIV_16_BIT, true);
        }
        else
        {
            obj->is_user_clock = true;
            obj->clock = *clk;
        }
    }

    if (result == CY_RSLT_SUCCESS)
    {
        result = (cy_rslt_t)Cy_SysClk_PeriphAssignDivider(
            (en_clk_dst_t)((uint8_t)PCLK_SCB0_CLOCK + obj->resource.block_num), obj->clock.div_type, obj->clock.div_num);
    }
    if (result == CY_RSLT_SUCCESS)
    {
        result = cyhal_connect_pin(rx_map);
    }
    if (result == CY_RSLT_SUCCESS)
    {
        result = cyhal_connect_pin(tx_map);
    }

    bool configured = cyhal_hwmgr_is_configured(obj->resource.type, obj->resource.block_num, obj->resource.channel_num);
    if (result == CY_RSLT_SUCCESS && !configured)
    {
        if (cfg == NULL)
        {
            Cy_SCB_UART_Init(obj->base, &default_uart_config, &(obj->context));
        }
        else
        {
            cy_stc_scb_uart_config_t config_structure = default_uart_config;
            config_structure.dataWidth = cfg->data_bits;
            config_structure.stopBits = convert_stopbits(cfg->stop_bits);
            config_structure.parity = convert_parity(cfg->parity);
            Cy_SCB_UART_Init(obj->base, &config_structure, &(obj->context));
            if (cfg->rx_buffer != NULL)
            {
                Cy_SCB_UART_StartRingBuffer(obj->base, cfg->rx_buffer, cfg->rx_buffer_size, &(obj->context));
            }

            obj->pm_params.base = obj->base;
            obj->pm_params.context = obj;
            obj->pm_callback.callback = &cyhal_uart_pm_callback;
            obj->pm_callback.type = CY_SYSPM_DEEPSLEEP;
            obj->pm_callback.skipMode = 0;
            obj->pm_callback.callbackParams = &(obj->pm_params);
            obj->pm_callback.prevItm = NULL;
            obj->pm_callback.nextItm = NULL;
            if (!Cy_SysPm_RegisterCallback(&(obj->pm_callback)))
                result = CYHAL_UART_RSLT_ERR_PM_CALLBACK;
        }        
        cyhal_hwmgr_set_configured(obj->resource.type, obj->resource.block_num, obj->resource.channel_num);
    }

    if (result == CY_RSLT_SUCCESS)
    {
        if (obj->is_user_clock)
        {
            Cy_SCB_UART_Enable(obj->base);
        }
        else
        {
            result = cyhal_uart_baud(obj, CYHAL_UART_DEFAULT_BAUD);
        }
    }

    if (result != CY_RSLT_SUCCESS)
    {
        cyhal_uart_free(obj);
    }
    return result;
}

void cyhal_uart_free(cyhal_uart_t *obj)
{
    if (obj->resource.type != CYHAL_RSC_INVALID)
    {
        cyhal_hwmgr_set_unconfigured(obj->resource.type, obj->resource.block_num, obj->resource.channel_num);
        cyhal_hwmgr_free(&(obj->resource));
    }
    if (CYHAL_NC_PIN_VALUE != obj->pin_rx)
    {
        cyhal_utils_disconnect_and_free(obj->pin_rx);
    }
    if (CYHAL_NC_PIN_VALUE != obj->pin_tx)
    {
        cyhal_utils_disconnect_and_free(obj->pin_tx);
    }
    if (CYHAL_NC_PIN_VALUE != obj->pin_rts)
    {
        cyhal_utils_disconnect_and_free(obj->pin_rts);
    }
    if (CYHAL_NC_PIN_VALUE != obj->pin_cts)
    {
        cyhal_utils_disconnect_and_free(obj->pin_cts);
    }
    if (!(obj->is_user_clock))
    {
        cyhal_hwmgr_free_clock(&(obj->clock));
    }
}

#define FRACT_DIV_INT(divider)      (((divider) >> 5U) - 1U)
#define FRACT_DIV_FARCT(divider)    ((divider) & 0x1FU)

static uint32_t cyhal_divider_value(uint32_t frequency, uint32_t frac_bits)
{
    CY_ASSERT(frequency != 0);
    /* UARTs use peripheral clock */
    return ((cy_PeriClkFreqHz * (1 << frac_bits)) + (frequency / 2)) / frequency;
}

cy_rslt_t cyhal_uart_baud(cyhal_uart_t *obj, uint32_t baudrate)
{
    Cy_SCB_UART_Disable(obj->base, NULL);
    cy_en_sysclk_status_t status;

    Cy_SysClk_PeriphDisableDivider(obj->clock.div_type, obj->clock.div_num);

    /* Set baud rate */
    if ((obj->clock.div_type == CY_SYSCLK_DIV_16_5_BIT) || (obj->clock.div_type == CY_SYSCLK_DIV_24_5_BIT)) {
        /* Get fractional divider */
        uint32_t divider = cyhal_divider_value(baudrate * UART_OVERSAMPLE, 5U);

        status = Cy_SysClk_PeriphSetFracDivider(obj->clock.div_type,
                                                obj->clock.div_num,
                                                FRACT_DIV_INT(divider),
                                                FRACT_DIV_FARCT(divider));
    } else {
        /* Get integer divider */
        status = Cy_SysClk_PeriphSetDivider(obj->clock.div_type,
                                            obj->clock.div_num,
                                            cyhal_divider_value(baudrate * UART_OVERSAMPLE, 0));
    }

    Cy_SysClk_PeriphEnableDivider(obj->clock.div_type, obj->clock.div_num);

    Cy_SCB_UART_Enable(obj->base);
    return status;
}

cy_rslt_t cyhal_uart_getc(cyhal_uart_t *obj, uint8_t *value, uint32_t timeout)
{
    uint32_t read_value = Cy_SCB_UART_Get(obj->base);
    uint32_t timeoutTicks = timeout;
    while (read_value == CY_SCB_UART_RX_NO_DATA)
    {
        if(timeout != 0UL)
        {
            if(timeoutTicks > 0UL)
            {
                Cy_SysLib_Delay(1);
                timeoutTicks--;
            }
            else
            {
                return CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT;
            }            
        }
        read_value = Cy_SCB_UART_Get(obj->base);
    }
    *value = (uint8_t)read_value;
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_uart_putc(cyhal_uart_t *obj, uint32_t value)
{
    uint32_t count = 0;
    while (count == 0)
    {
        count = Cy_SCB_UART_Put(obj->base, value);
    }
    return CY_RSLT_SUCCESS;
}

uint32_t cyhal_uart_readable(cyhal_uart_t *obj)
{
    uint32_t number_available = Cy_SCB_UART_GetNumInRxFifo(obj->base);
    if(obj->context.rxRingBuf != NULL)
    {
        number_available += Cy_SCB_UART_GetNumInRingBuffer(obj->base, &(obj->context));
    }
    return number_available;
}

uint32_t cyhal_uart_writable(cyhal_uart_t *obj)
{
    return Cy_SCB_GetFifoSize(obj->base) - Cy_SCB_GetNumInTxFifo(obj->base);
}

cy_rslt_t cyhal_uart_clear(cyhal_uart_t *obj)
{
    Cy_SCB_UART_ClearRxFifo(obj->base);
    Cy_SCB_UART_ClearTxFifo(obj->base);

    if(obj->context.rxRingBuf != NULL)
    {
        Cy_SCB_UART_ClearRingBuffer(obj->base, &(obj->context));
    }

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_uart_set_flow_control(cyhal_uart_t *obj, cyhal_gpio_t cts, cyhal_gpio_t rts)
{
    cy_rslt_t result;
    cyhal_resource_inst_t pin_rsc;

    if (cts != obj->pin_cts)
    {
        if (NC == cts)
        {
            result = cyhal_disconnect_pin(cts);

            if (CY_RSLT_SUCCESS == result)
            {
                Cy_SCB_UART_DisableCts(obj->base);

                pin_rsc = cyhal_utils_get_gpio_resource(cts);
                cyhal_hwmgr_free(&pin_rsc);
            }
        }
        else
        {
            const cyhal_resource_pin_mapping_t *cts_map = CY_UTILS_GET_RESOURCE(cts, cyhal_pin_map_scb_uart_cts);
            if (obj->resource.block_num != cts_map->inst->block_num ||
                obj->resource.channel_num != cts_map->inst->channel_num)
            {
                return CYHAL_UART_RSLT_ERR_INVALID_PIN;
            }

            pin_rsc = cyhal_utils_get_gpio_resource(cts);
            result = cyhal_hwmgr_reserve(&pin_rsc);

            if (CY_RSLT_SUCCESS == result)
            {
                Cy_SCB_UART_EnableCts(obj->base);
                result = cyhal_connect_pin(cts_map);
            }
        }

        if (result != CY_RSLT_SUCCESS)
        {
            return result;
        }
        obj->pin_cts = cts;
    }
    if (rts != obj->pin_rts)
    {
        if (NC == rts)
        {
            result = cyhal_disconnect_pin(rts);

            if (CY_RSLT_SUCCESS == result)
            {
                pin_rsc = cyhal_utils_get_gpio_resource(rts);
                cyhal_hwmgr_free(&pin_rsc);
            }
        }
        else
        {
            const cyhal_resource_pin_mapping_t *rts_map = CY_UTILS_GET_RESOURCE(rts, cyhal_pin_map_scb_uart_rts);
            if (obj->resource.block_num != rts_map->inst->block_num ||
                obj->resource.channel_num != rts_map->inst->channel_num)
            {
                return CYHAL_UART_RSLT_ERR_INVALID_PIN;
            }

            pin_rsc = cyhal_utils_get_gpio_resource(rts);
            result = cyhal_hwmgr_reserve(&pin_rsc);

            if (CY_RSLT_SUCCESS == result)
            {
                result = cyhal_connect_pin(rts_map);
            }
        }

        if (result != CY_RSLT_SUCCESS)
        {
            return result;
        }
        obj->pin_rts = rts;
    }
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_uart_tx(cyhal_uart_t *obj, void *tx, size_t *tx_length)
{
    *tx_length = Cy_SCB_UART_PutArray(obj->base, tx, *tx_length);
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_uart_rx(cyhal_uart_t *obj, void *rx, size_t *rx_length)
{
    *rx_length = Cy_SCB_UART_GetArray(obj->base, rx, *rx_length);
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_uart_tx_asynch(cyhal_uart_t *obj, void *tx, size_t length)
{
    cy_en_scb_uart_status_t uart_status = Cy_SCB_UART_Transmit(obj->base, tx, length, &(obj->context));
    return uart_status == CY_SCB_UART_SUCCESS
        ? CY_RSLT_SUCCESS
        : CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_UART, 0);
}

cy_rslt_t cyhal_uart_rx_asynch(cyhal_uart_t *obj, void *rx, size_t length)
{
    cy_en_scb_uart_status_t uart_status = Cy_SCB_UART_Receive(obj->base, rx, length, &(obj->context));
    return uart_status == CY_SCB_UART_SUCCESS
        ? CY_RSLT_SUCCESS
        : CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_UART, 0);
}

bool cyhal_uart_is_tx_active(cyhal_uart_t *obj)
{
    return (0UL != (obj->context.txStatus & CY_SCB_UART_TRANSMIT_ACTIVE));
}

bool cyhal_uart_is_rx_active(cyhal_uart_t *obj)
{
    return (0UL != (obj->context.rxStatus & CY_SCB_UART_RECEIVE_ACTIVE));
}

cy_rslt_t cyhal_uart_tx_abort(cyhal_uart_t *obj)
{
    Cy_SCB_UART_AbortTransmit(obj->base, &(obj->context));
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_uart_rx_abort(cyhal_uart_t *obj)
{
    Cy_SCB_UART_AbortReceive(obj->base, &(obj->context));
    return CY_RSLT_SUCCESS;
}

static cyhal_uart_irq_event_t cyhal_convert_interrupt_cause(uint32_t pdl_cause)
{
    cyhal_uart_irq_event_t cause;
    switch (pdl_cause)
    {
        case CY_SCB_UART_TRANSMIT_IN_FIFO_EVENT:
            cause = CYHAL_UART_IRQ_TX_TRANSMIT_IN_FIFO;
            break;
        case CY_SCB_UART_TRANSMIT_DONE_EVENT:
            cause = CYHAL_UART_IRQ_TX_DONE;
            break;
        case CY_SCB_UART_RECEIVE_DONE_EVENT:
            cause = CYHAL_UART_IRQ_RX_DONE;
            break;
        case CY_SCB_UART_RB_FULL_EVENT:
            cause = CYHAL_UART_IRQ_RX_FULL;
            break;
        case CY_SCB_UART_RECEIVE_ERR_EVENT:
            cause = CYHAL_UART_IRQ_RX_ERROR;
            break;
        case CY_SCB_UART_TRANSMIT_ERR_EVENT:
            cause = CYHAL_UART_IRQ_TX_ERROR;
            break;
        default:
            cause = CYHAL_UART_IRQ_NONE;
            break;
    }
    return cause;
}

void cyhal_uart_register_irq(cyhal_uart_t *obj, cyhal_uart_irq_handler_t handler, void *handler_arg)
{
    uint8_t idx = obj->resource.block_num;
    cyhal_uart_config_structs[idx] = obj;
    cyhal_uart_user_callbacks[idx] = handler;
    cyhal_uart_callback_args[idx] = handler_arg;
    Cy_SCB_UART_RegisterCallback(obj->base, cyhal_uart_cb_wrapper_table[idx], &(obj->context));
    if (NVIC_GetEnableIRQ(CY_SCB_IRQ_N[idx]) == 0)
    {
        // default interrupt priority of 7 (lowest possible priority).
        cy_stc_sysint_t irqCfg = {CY_SCB_IRQ_N[idx], 7u};

        Cy_SysInt_Init(&irqCfg, cyhal_uart_interrupts_dispatcher_table[idx]);
        NVIC_EnableIRQ(CY_SCB_IRQ_N[idx]);
    }
}

void cyhal_uart_irq_enable(cyhal_uart_t *obj, cyhal_uart_irq_event_t event, bool enable)
{
    if (enable)
    {
        obj->irq_cause |= event;
    }
    else
    {
        obj->irq_cause &= ~event;
    }
}
