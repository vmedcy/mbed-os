/***************************************************************************//**
* \file cyhal_spi.c
*
* \brief
* Provides a high level interface for interacting with the Cypress PWM. This is
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

#define SPI_DEFAULT_SPEED               100000
#define NUM_SPI_PORTS                   8
#define SPI_DEFAULT_IRQ_PRIORITY        7
#define SPI_OVERSAMPLE                  4       /* 4..16 */
/* Default timeout in milliseconds */
#define SPI_DEFAULT_TIMEOUT             1000

#define PENDING_NONE                    0
#define PENDING_RX                      1
#define PENDING_TX                      2
#define PENDING_TX_RX                   3

/* Default SPI configuration */
static
const cy_stc_scb_spi_config_t default_spi_config = {
    .spiMode                  = CY_SCB_SPI_MASTER,
    .subMode                  = CY_SCB_SPI_MOTOROLA,
    .sclkMode                 = CY_SCB_SPI_CPHA0_CPOL0,
    .oversample               = SPI_OVERSAMPLE,
    .rxDataWidth              = 8,
    .txDataWidth              = 8,
    .enableMsbFirst           = true,
    .enableFreeRunSclk        = false,
    .enableInputFilter        = false,
    .enableMisoLateSample     = false,
    .enableTransferSeperation = false,
    .enableWakeFromSleep      = false,
    .ssPolarity               = CY_SCB_SPI_ACTIVE_LOW,
    .rxFifoTriggerLevel       = 0,
    .rxFifoIntEnableMask      = 0,
    .txFifoTriggerLevel       = 0,
    .txFifoIntEnableMask      = 0,
    .masterSlaveIntEnableMask = 0
};

static cyhal_spi_irq_event_t cyhal_convert_interrupt_cause(uint32_t pdl_cause);

static cyhal_spi_t *cyhal_spi_config_structs[CY_IP_MXSCB_INSTANCES];
static cyhal_spi_irq_handler_t cyhal_spi_user_callbacks[CY_IP_MXSCB_INSTANCES];
static void *cyhal_spi_callback_args[CY_IP_MXSCB_INSTANCES];

static void cyhal_spi_0_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_1_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_2_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_3_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_4_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_5_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_6_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_7_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_8_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_9_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_10_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_11_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_12_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_13_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_14_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_15_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_16_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_17_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_18_cb_wrapper(uint32_t event) __attribute__((unused));
static void cyhal_spi_19_cb_wrapper(uint32_t event) __attribute__((unused));

static void cyhal_spi_0_irq_handler(void) __attribute__((unused));
static void cyhal_spi_1_irq_handler(void) __attribute__((unused));
static void cyhal_spi_2_irq_handler(void) __attribute__((unused));
static void cyhal_spi_3_irq_handler(void) __attribute__((unused));
static void cyhal_spi_4_irq_handler(void) __attribute__((unused));
static void cyhal_spi_5_irq_handler(void) __attribute__((unused));
static void cyhal_spi_6_irq_handler(void) __attribute__((unused));
static void cyhal_spi_7_irq_handler(void) __attribute__((unused));
static void cyhal_spi_8_irq_handler(void) __attribute__((unused));
static void cyhal_spi_9_irq_handler(void) __attribute__((unused));
static void cyhal_spi_10_irq_handler(void) __attribute__((unused));
static void cyhal_spi_11_irq_handler(void) __attribute__((unused));
static void cyhal_spi_12_irq_handler(void) __attribute__((unused));
static void cyhal_spi_13_irq_handler(void) __attribute__((unused));
static void cyhal_spi_14_irq_handler(void) __attribute__((unused));
static void cyhal_spi_15_irq_handler(void) __attribute__((unused));
static void cyhal_spi_16_irq_handler(void) __attribute__((unused));
static void cyhal_spi_17_irq_handler(void) __attribute__((unused));
static void cyhal_spi_18_irq_handler(void) __attribute__((unused));
static void cyhal_spi_19_irq_handler(void) __attribute__((unused));

static cyhal_spi_irq_event_t cyhal_convert_interrupt_cause(uint32_t pdl_cause)
{
    cyhal_spi_irq_event_t cause;
    switch (pdl_cause)
    {
        case CY_SCB_SPI_TRANSFER_IN_FIFO_EVENT:
            cause = CYHAL_SPI_IRQ_DATA_IN_FIFO;
            break;
        case CY_SCB_SPI_TRANSFER_CMPLT_EVENT:
            cause = CYHAL_SPI_IRQ_DONE;
            break;
        case CY_SCB_SPI_TRANSFER_ERR_EVENT:
            cause = CYHAL_SPI_IRQ_ERROR;
            break;
        default:
            cause = CYHAL_SPI_IRQ_ERROR;
            break;
    }
    return cause;
}
static __INLINE  void cyhal_spi_cb_wrapper_indexed(uint32_t event, uint8_t idx)
{
    cyhal_spi_irq_event_t anded_events = (cyhal_spi_irq_event_t)(cyhal_spi_config_structs[idx]->irq_cause & (uint32_t)cyhal_convert_interrupt_cause(event));
    if (anded_events)
    {
        cyhal_spi_user_callbacks[idx](cyhal_spi_callback_args[idx], anded_events);
    }
}
static void cyhal_spi_0_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 0);
}
static void cyhal_spi_1_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 1);
}
static void cyhal_spi_2_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 2);
}
static void cyhal_spi_3_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 3);
}
static void cyhal_spi_4_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 4);
}
static void cyhal_spi_5_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 5);
}
static void cyhal_spi_6_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 6);
}
static void cyhal_spi_7_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 7);
}
static void cyhal_spi_8_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 8);
}
static void cyhal_spi_9_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 9);
}
static void cyhal_spi_10_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 10);
}
static void cyhal_spi_11_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 11);
}
static void cyhal_spi_12_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 12);
}
static void cyhal_spi_13_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 13);
}
static void cyhal_spi_14_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 14);
}
static void cyhal_spi_15_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 15);
}
static void cyhal_spi_16_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 16);
}
static void cyhal_spi_17_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 17);
}
static void cyhal_spi_18_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 18);
}
static void cyhal_spi_19_cb_wrapper(uint32_t event)
{
    cyhal_spi_cb_wrapper_indexed(event, 19);
}

static void (*cyhal_spi_cb_wrapper_table[CY_IP_MXSCB_INSTANCES])(uint32_t event) =
{
#if (CY_IP_MXSCB_INSTANCES > 0)
    cyhal_spi_0_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 1)
    cyhal_spi_1_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 2)
    cyhal_spi_2_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 3)
    cyhal_spi_3_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 4)
    cyhal_spi_4_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 5)
    cyhal_spi_5_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 6)
    cyhal_spi_6_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 7)
    cyhal_spi_7_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 8)
    cyhal_spi_8_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 9)
    cyhal_spi_9_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 10)
    cyhal_spi_10_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 11)
    cyhal_spi_11_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 12)
    cyhal_spi_12_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 13)
    cyhal_spi_13_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 14)
    cyhal_spi_14_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 15)
    cyhal_spi_15_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 16)
    cyhal_spi_16_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 17)
    cyhal_spi_17_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 18)
    cyhal_spi_18_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 19)
    cyhal_spi_19_cb_wrapper,
#endif
#if (CY_IP_MXSCB_INSTANCES > 20)
    #error "Unhandled scb count"
#endif
};

static void cyhal_spi_interrupts_dispatcher_IRQHandler(uint32_t spi_num)
{
    cyhal_spi_t *obj = cyhal_spi_config_structs[spi_num];
    void *buf;

    if (NULL == obj)
    {
        return;
    }

    Cy_SCB_SPI_Interrupt(obj->base, &(obj->context));

    if (PENDING_NONE == obj->pending)
    {
        /* Possibily Synchronous API */
        return;
    }
    if (0 == (Cy_SCB_SPI_GetTransferStatus(obj->base,  &obj->context) & CY_SCB_SPI_TRANSFER_ACTIVE))
    {
       if (obj->tx_buffer)
       {
           /* Start TX Transfer */
           obj->pending = PENDING_TX;
           buf = obj->tx_buffer;
           obj->tx_buffer = NULL;

           Cy_SCB_SPI_Transfer(obj->base, buf, NULL, obj->tx_buffer_size, &obj->context);
       } else if (obj->rx_buffer)
       {
           /* Start RX Transfer */
           obj->pending = PENDING_RX;
           buf = obj->rx_buffer;
           obj->rx_buffer = NULL;

           Cy_SCB_SPI_Transfer(obj->base, NULL, buf, obj->rx_buffer_size, &obj->context);
       } else
       {
           /* Finish Async Transfer */
           obj->pending = PENDING_NONE;
       }
    }
}
static void cyhal_spi_0_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(0);
}
static void cyhal_spi_1_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(1);
}
static void cyhal_spi_2_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(2);
}
static void cyhal_spi_3_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(3);
}
static void cyhal_spi_4_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(4);
}
static void cyhal_spi_5_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(5);
}
static void cyhal_spi_6_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(6);
}
static void cyhal_spi_7_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(7);
}
static void cyhal_spi_8_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(8);
}
static void cyhal_spi_9_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(9);
}
static void cyhal_spi_10_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(10);
}
static void cyhal_spi_11_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(11);
}
static void cyhal_spi_12_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(12);
}
static void cyhal_spi_13_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(13);
}
static void cyhal_spi_14_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(14);
}
static void cyhal_spi_15_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(15);
}
static void cyhal_spi_16_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(16);
}
static void cyhal_spi_17_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(17);
}
static void cyhal_spi_18_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(18);
}
static void cyhal_spi_19_irq_handler(void)
{
    cyhal_spi_interrupts_dispatcher_IRQHandler(19);
}
static void (*cyhal_spi_interrupts_dispatcher_table[CY_IP_MXSCB_INSTANCES])(void) =
{
#if (CY_IP_MXSCB_INSTANCES > 0)
    cyhal_spi_0_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 1)
    cyhal_spi_1_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 2)
    cyhal_spi_2_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 3)
    cyhal_spi_3_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 4)
    cyhal_spi_4_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 5)
    cyhal_spi_5_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 6)
    cyhal_spi_6_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 7)
    cyhal_spi_7_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 8)
    cyhal_spi_8_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 9)
    cyhal_spi_9_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 10)
    cyhal_spi_10_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 11)
    cyhal_spi_11_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 12)
    cyhal_spi_12_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 13)
    cyhal_spi_13_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 14)
    cyhal_spi_14_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 15)
    cyhal_spi_15_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 16)
    cyhal_spi_16_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 17)
    cyhal_spi_17_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 18)
    cyhal_spi_18_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 19)
    cyhal_spi_19_irq_handler,
#endif
#if (CY_IP_MXSCB_INSTANCES > 20)
    #error "Unhandled scb count"
#endif
};

static void cyhal_spi_install_irq(cyhal_spi_t *obj)
{
    uint8_t idx = obj->resource.block_num;
    // default interrupt priority of SPI_DEFAULT_IRQ_PRIORITY (lowest possible priority).
    cy_stc_sysint_t irqCfg = {CY_SCB_IRQ_N[idx], SPI_DEFAULT_IRQ_PRIORITY};

    Cy_SysInt_Init(&irqCfg, cyhal_spi_interrupts_dispatcher_table[idx]);
    NVIC_EnableIRQ(CY_SCB_IRQ_N[idx]);
}

static cy_en_scb_spi_sclk_mode_t cyhal_convert_mode_sclk(cyhal_spi_mode_t mode)
{
    uint8_t sclk_mode = (mode & 0x6) >> 1;

    switch (sclk_mode)
    {
        case 0:
            return (CY_SCB_SPI_CPHA0_CPOL0);
        case 1:
            return (CY_SCB_SPI_CPHA0_CPOL1);
        case 2:
            return (CY_SCB_SPI_CPHA1_CPOL0);
        case 3:
            return (CY_SCB_SPI_CPHA1_CPOL1);
        default:
            return (CY_SCB_SPI_CPHA0_CPOL0);
    }
}

static bool is_cyhal_mode_msb(cyhal_spi_mode_t mode)
{
    switch(mode)
    {
        case CYHAL_SPI_MODE_00_MSB:
        case CYHAL_SPI_MODE_01_MSB:
        case CYHAL_SPI_MODE_10_MSB:
        case CYHAL_SPI_MODE_11_MSB:
            return true;
        case CYHAL_SPI_MODE_00_LSB:
        case CYHAL_SPI_MODE_01_LSB:
        case CYHAL_SPI_MODE_10_LSB:
        case CYHAL_SPI_MODE_11_LSB:
            return false;
        default:
            CY_ASSERT(false);
            return true;
    }
}

cy_rslt_t cyhal_spi_init(cyhal_spi_t *obj, cyhal_gpio_t mosi, cyhal_gpio_t miso, cyhal_gpio_t sclk, cyhal_gpio_t ssel, const cyhal_clock_divider_t *clk,
                        uint8_t bits, cyhal_spi_mode_t mode, bool is_slave)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    cyhal_resource_inst_t pin_rsc;

    if (NULL == obj)
        return CYHAL_SPI_RSLT_BAD_ARGUMENT;

    // If something go wrong, any resource not marked as invalid will be freed.
    // Explicitly marked not allocated resources as invalid to prevent freeing them.
    memset(obj, 0, sizeof(cyhal_spi_t));

    // Reserve the SPI
    const cyhal_resource_pin_mapping_t *mosi_map = CY_UTILS_GET_RESOURCE(mosi, cyhal_pin_map_scb_spi_mosi);
    const cyhal_resource_pin_mapping_t *miso_map = CY_UTILS_GET_RESOURCE(miso, cyhal_pin_map_scb_spi_miso);
    const cyhal_resource_pin_mapping_t *sclk_map = CY_UTILS_GET_RESOURCE(sclk, cyhal_pin_map_scb_spi_clk);
    const cyhal_resource_pin_mapping_t *ssel_map = CY_UTILS_GET_RESOURCE(ssel, cyhal_pin_map_scb_spi_select0);
    if (NULL == mosi_map || NULL == miso_map || NULL == sclk_map || NULL == ssel_map ||
        mosi_map->inst->block_num != miso_map->inst->block_num)
    {
        return CYHAL_SPI_RSLT_ERR_INVALID_PIN;
    }

    // reserve the MOSI pin
    if (result == CY_RSLT_SUCCESS)
    {
        pin_rsc = cyhal_utils_get_gpio_resource(mosi);
        result = cyhal_hwmgr_reserve(&pin_rsc);
    }
    if (result == CY_RSLT_SUCCESS)
    {
        obj->pin_mosi = mosi;
    }

    //reseve the MISO pin
    if (result == CY_RSLT_SUCCESS)
    {
        pin_rsc = cyhal_utils_get_gpio_resource(miso);
        result = cyhal_hwmgr_reserve(&pin_rsc);
    }
    if (result == CY_RSLT_SUCCESS)
    {
        obj->pin_miso = miso;
    }

    //reseve the SCLK pin
    if (result == CY_RSLT_SUCCESS)
    {
        pin_rsc = cyhal_utils_get_gpio_resource(sclk);
        result = cyhal_hwmgr_reserve(&pin_rsc);
    }
    if (result == CY_RSLT_SUCCESS)
    {
        obj->pin_sclk = sclk;
    }

    //reseve the SSEL pin
    if (result == CY_RSLT_SUCCESS)
    {
        pin_rsc = cyhal_utils_get_gpio_resource(ssel);
        result = cyhal_hwmgr_reserve(&pin_rsc);
    }
    if (result == CY_RSLT_SUCCESS)
    {
        obj->pin_ssel = ssel;
    }

    obj->base = CY_SCB_BASE_ADDRESSES[obj->resource.block_num];

    if (result == CY_RSLT_SUCCESS)
    {
        if (clk == NULL)
        {
            result = cyhal_hwmgr_allocate_clock(&(obj->clock), CY_SYSCLK_DIV_8_BIT, true);
        }
    }
    if (result == CY_RSLT_SUCCESS)
    {
        cy_en_sysclk_status_t clock_assign_result = Cy_SysClk_PeriphAssignDivider(
                (en_clk_dst_t)((uint8_t)PCLK_SCB0_CLOCK + obj->resource.block_num), obj->clock.div_type, obj->clock.div_num);
        result = clock_assign_result == CY_SYSCLK_SUCCESS
            ? CY_RSLT_SUCCESS
            : CYHAL_SPI_RSLT_CLOCK_ERROR;
    }
    if (result == CY_RSLT_SUCCESS)
    {
        result = cyhal_connect_pin(mosi_map);
    }
    if (result == CY_RSLT_SUCCESS)
    {
        result = cyhal_connect_pin(miso_map);
    }

    bool configured = cyhal_hwmgr_is_configured(obj->resource.type, obj->resource.block_num, obj->resource.channel_num);
    if (result == CY_RSLT_SUCCESS && !configured)
    {
        cy_stc_scb_spi_config_t config_structure = default_spi_config;
        config_structure.spiMode = is_slave == 0
            ? CY_SCB_SPI_MASTER
            : CY_SCB_SPI_SLAVE;
        config_structure.enableMsbFirst = is_cyhal_mode_msb(mode);
        config_structure.sclkMode = cyhal_convert_mode_sclk(mode);
        config_structure.rxDataWidth = bits;
        config_structure.txDataWidth = bits;
        obj->data_bits = bits;
        Cy_SCB_SPI_Init(obj->base, &config_structure, &(obj->context));
        result = cyhal_hwmgr_set_configured(obj->resource.type, obj->resource.block_num, obj->resource.channel_num);
        cyhal_spi_install_irq(obj);
    }
    if (result != CY_RSLT_SUCCESS)
    {
        cyhal_spi_free(obj);
    }
    return result;
}

void cyhal_spi_free(cyhal_spi_t *obj)
{
    if (NULL != obj->base)
    {
        Cy_SCB_SPI_Disable(obj->base, NULL);
        Cy_SCB_SPI_DeInit(obj->base);
        obj->base = NULL;
    }

    if (obj->resource.type != CYHAL_RSC_INVALID)
    {
        cyhal_hwmgr_set_unconfigured(obj->resource.type, obj->resource.block_num, obj->resource.channel_num);
        cyhal_hwmgr_free(&(obj->resource));
        obj->resource.type = CYHAL_RSC_INVALID;
    }
    if (CYHAL_NC_PIN_VALUE != obj->pin_miso)
    {
        cyhal_utils_disconnect_and_free(obj->pin_miso);
        obj->pin_miso = CYHAL_NC_PIN_VALUE;
    }
    if (CYHAL_NC_PIN_VALUE != obj->pin_mosi)
    {
        cyhal_utils_disconnect_and_free(obj->pin_mosi);
        obj->pin_mosi = CYHAL_NC_PIN_VALUE;
    }
    if (CYHAL_NC_PIN_VALUE != obj->pin_sclk)
    {
        cyhal_utils_disconnect_and_free(obj->pin_sclk);
        obj->pin_sclk = CYHAL_NC_PIN_VALUE;
    }
    if (CYHAL_NC_PIN_VALUE != obj->pin_ssel)
    {
        cyhal_utils_disconnect_and_free(obj->pin_ssel);
        obj->pin_ssel = CYHAL_NC_PIN_VALUE;
    }
}

#define FRACT_DIV_INT(divider)      (((divider) >> 5U) - 1U)
#define FRACT_DIV_FARCT(divider)    ((divider) & 0x1FU)

static uint32_t cyhal_divider_value(uint32_t frequency, uint32_t frac_bits)
{
    /* SPI use peripheral clock */
    return ((cy_PeriClkFreqHz * (1 << frac_bits)) + (frequency / 2)) / frequency;
}

cy_rslt_t cyhal_spi_frequency(cyhal_spi_t *obj, uint32_t hz)
{
    cy_en_sysclk_status_t status = CY_SYSCLK_SUCCESS;

    if (NULL == obj)
        return CYHAL_SPI_RSLT_BAD_ARGUMENT;

    Cy_SCB_SPI_Disable(obj->base, NULL);

    if (status == CY_SYSCLK_SUCCESS) {
        Cy_SysClk_PeriphDisableDivider(obj->clock.div_type, obj->clock.div_num);

        /* Set frequency */
        if ((obj->clock.div_type == CY_SYSCLK_DIV_16_5_BIT) || (obj->clock.div_type == CY_SYSCLK_DIV_24_5_BIT)) {
            /* Get fractional divider */
            uint32_t divider = cyhal_divider_value(hz * SPI_OVERSAMPLE, 5U);

            status = Cy_SysClk_PeriphSetFracDivider(obj->clock.div_type,
                                                    obj->clock.div_num,
                                                    FRACT_DIV_INT(divider),
                                                    FRACT_DIV_FARCT(divider));
        } else {
            /* Get integer divider */
            status = Cy_SysClk_PeriphSetDivider(obj->clock.div_type,
                                                obj->clock.div_num,
                                                cyhal_divider_value(hz * SPI_OVERSAMPLE, 0));
        }

        Cy_SysClk_PeriphEnableDivider(obj->clock.div_type, obj->clock.div_num);
    }

    Cy_SCB_SPI_Enable(obj->base);
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_spi_read(cyhal_spi_t *obj, uint8_t *value)
{
    uint32_t read_value = CY_SCB_SPI_RX_NO_DATA;

    if (NULL == obj)
        return CYHAL_SPI_RSLT_BAD_ARGUMENT;

    while (read_value == CY_SCB_SPI_RX_NO_DATA)
    {
        read_value = Cy_SCB_SPI_Read(obj->base);
    }
    *value = read_value;
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_spi_write(cyhal_spi_t *obj, uint32_t value)
{
    uint32_t count = 0;

    if (NULL == obj)
        return CYHAL_SPI_RSLT_BAD_ARGUMENT;

    while (count == 0)
    {
        count = Cy_SCB_SPI_Write(obj->base, value);
    }
    return CY_RSLT_SUCCESS;
}

#define MAX_DUMMY_SIZE    ( 128 )

cy_rslt_t cyhal_spi_transfer(cyhal_spi_t *obj, const uint8_t *tx, size_t tx_length, uint8_t *rx, size_t rx_length, uint8_t write_fill)
{
    uint32_t spi_status;
    uint32_t remaining_rx, remaining_tx, xfr_length;
    uint8_t  dummy[MAX_DUMMY_SIZE];
    uint8_t *bufptr;

    if (NULL == obj)
        return CYHAL_SPI_RSLT_BAD_ARGUMENT;

    if (tx_length > rx_length)
    {
        xfr_length = (uint32_t) rx_length;
        remaining_rx = 0;
        remaining_tx = (uint32_t) (tx_length - rx_length);
        spi_status = Cy_SCB_SPI_Transfer(obj->base, (uint8_t *) tx, rx, xfr_length, &(obj->context));

        while ( spi_status == CY_SCB_SPI_SUCCESS )
        {
           spi_status = Cy_SCB_SPI_GetTransferStatus(obj->base, &(obj->context));

           if ((spi_status & CY_SCB_SPI_TRANSFER_CMPLT_EVENT)== CY_SCB_SPI_TRANSFER_CMPLT_EVENT )
               break;
        }
        bufptr = obj->data_bits == 8 ?
                 (uint8_t *)(((uint8_t *)tx) + xfr_length) :
                 (uint8_t *)(((uint16_t *)tx) + xfr_length);

        spi_status = Cy_SCB_SPI_Transfer(obj->base, bufptr, NULL, remaining_tx, &(obj->context));

        while ( spi_status == CY_SCB_SPI_SUCCESS )
        {
          spi_status = Cy_SCB_SPI_GetTransferStatus(obj->base, &(obj->context));

          if ((spi_status & CY_SCB_SPI_TRANSFER_CMPLT_EVENT)== CY_SCB_SPI_TRANSFER_CMPLT_EVENT )
              break;
        }
    } else if (tx_length < rx_length)
    {
        xfr_length = (uint32_t) tx_length;
        remaining_rx = (uint32_t) (rx_length - tx_length);
        spi_status = Cy_SCB_SPI_Transfer(obj->base, (uint8_t *) tx, rx, xfr_length, &(obj->context));

        while ( spi_status == CY_SCB_SPI_SUCCESS )
        {
           spi_status = Cy_SCB_SPI_GetTransferStatus(obj->base, &(obj->context));

           if ((spi_status & CY_SCB_SPI_TRANSFER_CMPLT_EVENT)== CY_SCB_SPI_TRANSFER_CMPLT_EVENT )
               break;
        }
        memset(dummy, write_fill, sizeof(dummy));
        while (remaining_rx)
        {
           if (remaining_rx <= MAX_DUMMY_SIZE)
           {
               bufptr = obj->data_bits == 8 ?
                       (uint8_t *)(((uint8_t *)rx) + xfr_length) :
                       (uint8_t *)(((uint16_t *)rx) + xfr_length);

               spi_status = Cy_SCB_SPI_Transfer(obj->base, dummy, bufptr, remaining_rx, &(obj->context));
               remaining_rx = 0;
           } else {
               bufptr = obj->data_bits == 8 ?
                       (uint8_t *)(((uint8_t *)rx) + xfr_length) :
                       (uint8_t *)(((uint16_t *)rx) + xfr_length);

               spi_status = Cy_SCB_SPI_Transfer(obj->base, dummy, bufptr, MAX_DUMMY_SIZE, &(obj->context));
               remaining_rx -= MAX_DUMMY_SIZE;
               xfr_length += MAX_DUMMY_SIZE;
           }
           while ( spi_status == CY_SCB_SPI_SUCCESS )
           {
              spi_status = Cy_SCB_SPI_GetTransferStatus(obj->base, &(obj->context));

              if ((spi_status & CY_SCB_SPI_TRANSFER_CMPLT_EVENT)== CY_SCB_SPI_TRANSFER_CMPLT_EVENT )
                  break;
           }
       }
    } else
    {
        xfr_length = (uint32_t) tx_length;
        spi_status = Cy_SCB_SPI_Transfer(obj->base, (uint8_t *) tx, rx, xfr_length, &(obj->context));

        while ( spi_status == CY_SCB_SPI_SUCCESS )
        {
           spi_status = Cy_SCB_SPI_GetTransferStatus(obj->base, &(obj->context));

           if ((spi_status & CY_SCB_SPI_TRANSFER_CMPLT_EVENT)== CY_SCB_SPI_TRANSFER_CMPLT_EVENT )
               break;
        }
    }
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_spi_transfer_async(cyhal_spi_t *obj, const uint8_t *tx, size_t tx_length, uint8_t *rx, size_t rx_length)
{
    cy_en_scb_spi_status_t spi_status;
    uint8_t *bufptr;

    if (NULL == obj)
        return CYHAL_SPI_RSLT_BAD_ARGUMENT;

    /* Setup transfer */
    if (tx_length > rx_length) {
        if (rx_length > 0) {
            /* I) write + read, II) write only */
            obj->pending = PENDING_TX_RX;
            obj->rx_buffer = NULL;
            bufptr = obj->data_bits == 8 ?
                     (uint8_t *)(((uint8_t *)tx) + rx_length) :
                     (uint8_t *)(((uint16_t *)tx) + rx_length);

            obj->tx_buffer = bufptr;
            obj->tx_buffer_size = tx_length - rx_length;
            spi_status = Cy_SCB_SPI_Transfer(obj->base, (void *)tx, rx, rx_length, &obj->context);
        } else {
            /*  I) write only */
            obj->pending = PENDING_TX;
            obj->rx_buffer = NULL;
            obj->tx_buffer = NULL;

            spi_status = Cy_SCB_SPI_Transfer(obj->base, (void *)tx, NULL, tx_length, &obj->context);
        }
    } else if (rx_length > tx_length) {
        if (tx_length > 0) {
            /*  I) write + read, II) read only */
            obj->pending = PENDING_TX_RX;
            bufptr = obj->data_bits == 8 ?
                     (uint8_t *)(((uint8_t *)rx) + tx_length) :
                     (uint8_t *)(((uint16_t *)rx) + tx_length);

            obj->rx_buffer = bufptr;
            obj->rx_buffer_size = rx_length - tx_length;
            obj->tx_buffer = NULL;

            spi_status = Cy_SCB_SPI_Transfer(obj->base, (void *)tx, rx, tx_length, &obj->context);
        } else {
            /*  I) read only. */
            obj->pending = PENDING_RX;
            obj->rx_buffer = NULL;
            obj->tx_buffer = NULL;

            spi_status = Cy_SCB_SPI_Transfer(obj->base, NULL, rx, rx_length, &obj->context);
        }
    } else {
        /* RX and TX of the same size: I) write + read. */
        obj->pending = PENDING_TX_RX;
        obj->rx_buffer = NULL;
        obj->tx_buffer = NULL;

        spi_status = Cy_SCB_SPI_Transfer(obj->base, (void *)tx, rx, tx_length, &obj->context);
    }
    return spi_status == CY_SCB_SPI_SUCCESS
        ? CY_RSLT_SUCCESS
        : CYHAL_SPI_RSLT_TRANSFER_ERROR;
}

bool cyhal_spi_is_busy(cyhal_spi_t *obj)
{
    return Cy_SCB_SPI_IsBusBusy(obj->base);
}

cy_rslt_t cyhal_spi_abort_async(cyhal_spi_t *obj)
{
    if (NULL == obj)
        return CYHAL_SPI_RSLT_BAD_ARGUMENT;

    Cy_SCB_SPI_AbortTransfer(obj->base, &(obj->context));
    obj->pending = PENDING_NONE;
    return CY_RSLT_SUCCESS;
}

void cyhal_spi_register_irq(cyhal_spi_t *obj, cyhal_spi_irq_handler_t handler, void *handler_arg)
{
    uint8_t idx = obj->resource.block_num;
    cyhal_spi_config_structs[idx] = obj;
    cyhal_spi_user_callbacks[idx] = handler;
    cyhal_spi_callback_args[idx] = handler_arg;
    Cy_SCB_SPI_RegisterCallback(obj->base, cyhal_spi_cb_wrapper_table[idx], &(obj->context));
    if (NVIC_GetEnableIRQ(CY_SCB_IRQ_N[idx]) == 0)
    {
        // default interrupt priority of SPI_DEFAULT_IRQ_PRIORITY (lowest possible priority).
        cy_stc_sysint_t irqCfg = {CY_SCB_IRQ_N[idx], SPI_DEFAULT_IRQ_PRIORITY};

        Cy_SysInt_Init(&irqCfg, cyhal_spi_interrupts_dispatcher_table[idx]);
        NVIC_EnableIRQ(CY_SCB_IRQ_N[idx]);
    }
}

void cyhal_spi_irq_enable(cyhal_spi_t *obj, cyhal_spi_irq_event_t event, bool enable)
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
