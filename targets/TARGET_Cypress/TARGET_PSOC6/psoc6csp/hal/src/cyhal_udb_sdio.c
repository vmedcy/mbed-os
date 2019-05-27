/*******************************************************************************
* File Name: cyhal_udb_sdio.c
*
* Description:
* Provides a high level interface for interacting with the Cypress UDB SDIO.
* This is a wrapper around the lower level UDB SDIO API.
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

#include "cyhal_hwmgr.h"

#if defined(CY8C6247BZI_D54) /* Cypress ticket: BSP-525 */

#include <stdlib.h>
#include "cyhal_sdio.h"
#include "cyhal_gpio.h"
#include "cyhal_interconnect.h"
#include "SDIO_HOST_cfg.h"

/* Not connected pin define */
#define SDIO_PINS_NC    ((cyhal_gpio_t) CYHAL_NC_PIN_VALUE)

#define CY_HAL_SDIO_CLK_DIV_VALUE   ((uint8_t) 0xFF)

/* Not configured clock divider define*/
#define CY_HAL_SDIO_CLK_DIV_NC      ((cy_en_divider_types_t) CY_HAL_SDIO_CLK_DIV_VALUE)

/* Define for default SDIO frequency */
#define  CY_HAL_SDIO_DEF_FREQ       (400000U)

/* The 64b block transition mode define */
#define CY_HAL_SDIO_64B       (64u)

/* The 1 byte transition mode define */
#define CY_HAL_SDIO_1B        (1u)

/*******************************************************************************
*       (Internal) Configuration structures for SDIO pins
*******************************************************************************/
const cy_stc_gpio_pin_config_t pin_cmd_config = 
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = P2_4_DSI_DSI, /* DSI controls 'out' and 'output enable' */
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
};

const cy_stc_gpio_pin_config_t pin_data_config = 
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = P2_0_DSI_DSI, /* DSI controls 'out' and 'output enable' */
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
};

const cy_stc_gpio_pin_config_t pin_clk_config = 
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = P2_5_DSI_GPIO, /* DSI controls 'out', GPIO controls 'output enable' */
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
};

/* Callback pointers */
static cyhal_sdio_irq_handler cyhal_sdio_callback = NULL;
static cyhal_sdio_t *cyhal_sdio_config_struct = NULL;
static void *cyhal_sdio_callback_args = NULL;


/*******************************************************************************
*       Dispatcher Interrupt Callbacks Service Routine
*******************************************************************************/
static void cyhal_sdio_interrupts_dispatcher_IRQHandler(void)
{
    if (cyhal_sdio_callback != NULL)
    {
        (void)(cyhal_sdio_callback)(cyhal_sdio_callback_args, (cyhal_sdio_irq_event_t) cyhal_sdio_config_struct->irq_cause);
    }
}


/*******************************************************************************
*       Internal functions
*******************************************************************************/
static cy_rslt_t cyhal_free_pins(cyhal_sdio_t *obj);

static cy_rslt_t cyhal_free_pins(cyhal_sdio_t *obj)
{
    cy_rslt_t retVal = CY_RSLT_SUCCESS;

    if (obj->pin_clk != SDIO_PINS_NC)
    {
        retVal = cyhal_disconnect_pin(obj->pin_clk);
        if (retVal == CY_RSLT_SUCCESS)
        {
            cyhal_resource_inst_t rsc = cyhal_utils_get_gpio_resource(obj->pin_clk);
            retVal = cyhal_hwmgr_free(&rsc);
        }
    }
    
    if ((retVal == CY_RSLT_SUCCESS) && (obj->pin_cmd != SDIO_PINS_NC))
    {
        retVal = cyhal_disconnect_pin(obj->pin_cmd);
        if (retVal == CY_RSLT_SUCCESS)
        {
            cyhal_resource_inst_t rsc = cyhal_utils_get_gpio_resource(obj->pin_cmd);
            retVal = cyhal_hwmgr_free(&rsc);
        }
    }

    if ((retVal == CY_RSLT_SUCCESS) && (obj->pin_data0 != SDIO_PINS_NC))
    {
        retVal = cyhal_disconnect_pin(obj->pin_data0);
        if (retVal == CY_RSLT_SUCCESS)
        {
            cyhal_resource_inst_t rsc = cyhal_utils_get_gpio_resource(obj->pin_data0);
            retVal = cyhal_hwmgr_free(&rsc);
        }
    }
    
    if ((retVal == CY_RSLT_SUCCESS) && (obj->pin_data1 != SDIO_PINS_NC))
    {
        retVal = cyhal_disconnect_pin(obj->pin_data1);
        if (retVal == CY_RSLT_SUCCESS)
        {
            cyhal_resource_inst_t rsc = cyhal_utils_get_gpio_resource(obj->pin_data1);
            retVal = cyhal_hwmgr_free(&rsc);
        }
    }
    
    if ((retVal == CY_RSLT_SUCCESS) && (obj->pin_data2 != SDIO_PINS_NC))
    {
        retVal = cyhal_disconnect_pin(obj->pin_data2);
        if (retVal == CY_RSLT_SUCCESS)
        {
            cyhal_resource_inst_t rsc = cyhal_utils_get_gpio_resource(obj->pin_data2);
            retVal = cyhal_hwmgr_free(&rsc);
        }
    }
    
    if ((retVal == CY_RSLT_SUCCESS) && (obj->pin_data3 != SDIO_PINS_NC))
    {
        retVal = cyhal_disconnect_pin(obj->pin_data3);
        if (retVal == CY_RSLT_SUCCESS)
        {
            cyhal_resource_inst_t rsc = cyhal_utils_get_gpio_resource(obj->pin_data3);
            retVal = cyhal_hwmgr_free(&rsc);
        }
    }

    return retVal;
}

static cy_rslt_t cyhal_free_clocks(cyhal_sdio_t *obj)
{
    cy_rslt_t retVal = CY_RSLT_SUCCESS;

    if (obj->clock.div_num != CY_HAL_SDIO_CLK_DIV_NC)
    {
        /* Free clock divider resource*/
        cyhal_resource_inst_t udbClkRsc;
        
        udbClkRsc.type = CYHAL_RSC_CLOCK;
        udbClkRsc.block_num = obj->clock.div_num;
        udbClkRsc.channel_num = 0;
        
        retVal = cyhal_hwmgr_set_unconfigured(udbClkRsc.type, udbClkRsc.block_num, udbClkRsc.channel_num);
        
        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_free(&udbClkRsc);
        }
    }

    return retVal;
}


static cy_rslt_t cyhal_free_dmas(cyhal_sdio_t *obj)
{
    cy_rslt_t retVal = CY_RSLT_SUCCESS;

    /* Free clock divider resource*/
    cyhal_resource_inst_t dmaRsc;

    /* Release DMA0 CH0 */
    dmaRsc.type = CYHAL_RSC_DMA;
    dmaRsc.block_num = 0;
    dmaRsc.channel_num = 0;

    if (obj->dma0Ch0.resource.type != CYHAL_RSC_INVALID)
    {
        retVal = cyhal_hwmgr_set_unconfigured(dmaRsc.type, dmaRsc.block_num, dmaRsc.channel_num);
    }

    if ((retVal == CY_RSLT_SUCCESS) && (obj->dma0Ch0.resource.type != CYHAL_RSC_INVALID))
    {
        retVal = cyhal_hwmgr_free(&dmaRsc);
    }

    /* Release DMA0 CH1 */
    dmaRsc.block_num = 0;
    dmaRsc.channel_num = 1;

    if ((retVal == CY_RSLT_SUCCESS) && (obj->dma0Ch1.resource.type != CYHAL_RSC_INVALID))
    {
        retVal = cyhal_hwmgr_set_unconfigured(dmaRsc.type, dmaRsc.block_num, dmaRsc.channel_num);
    
        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_free(&dmaRsc);
        }
    }
    
    /* Release DMA1 CH1 */
    dmaRsc.block_num = 1;
    dmaRsc.channel_num = 1;

    if ((retVal == CY_RSLT_SUCCESS) && (obj->dma1Ch1.resource.type != CYHAL_RSC_INVALID))
    {
        retVal = cyhal_hwmgr_set_unconfigured(dmaRsc.type, dmaRsc.block_num, dmaRsc.channel_num);
    
        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_free(&dmaRsc);
        }
    }
    
    /* Release DMA1 CH1 */
    dmaRsc.block_num = 1;
    dmaRsc.channel_num = 3;

    if ((retVal == CY_RSLT_SUCCESS) && (obj->dma1Ch3.resource.type != CYHAL_RSC_INVALID))
    {
        retVal = cyhal_hwmgr_set_unconfigured(dmaRsc.type, dmaRsc.block_num, dmaRsc.channel_num);
    
        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_free(&dmaRsc);
        }
    }
    return retVal;
}


/** Initialize the SDIO peripheral
 *
 * @param[out] obj               The SDIO object
 * @param[out] clk               The pin connected to the clk signal
 * @param[in]  cmd               The pin connected to the command signal
 * @param[in]  data0             The pin connected to the data0 signal
 * @param[in]  data1             The pin connected to the data1 signal
 * @param[in]  data2             The pin connected to the data2 signal
 * @param[in]  data3             The pin connected to the data3 signal
 * @return The status of the init request
 */
cy_rslt_t cyhal_sdio_init(cyhal_sdio_t *obj, cyhal_gpio_t cmd, cyhal_gpio_t clk, cyhal_gpio_t data0, cyhal_gpio_t data1, cyhal_gpio_t data2, cyhal_gpio_t data3)
{
    bool isConfigured = false;
    cy_rslt_t retVal = CY_RSLT_SUCCESS;

    if(NULL == obj)
    {
        return CYHAL_SDIO_RSLT_ERR_BAD_PARAM;
    }
    
    memset(obj, 0, sizeof(cyhal_sdio_t));
    
    /* If something go wrong, any resource not marked as invalid will be freed.
    *  Explicitly marked not allocated resources as invalid to prevent freeing 
    *  them.
    * 
    *  Before we reserve UDB SDIO need to reserve clock divider, then 
    *  DMAs and pins. If all these resources are reserved only then reserve UDB 
    *  SDIO.
    */
    obj->resource.type = CYHAL_RSC_INVALID;
    obj->pin_cmd = SDIO_PINS_NC;
    obj->pin_clk = SDIO_PINS_NC;
    obj->pin_data0 = SDIO_PINS_NC;
    obj->pin_data1 = SDIO_PINS_NC;
    obj->pin_data2 = SDIO_PINS_NC;
    obj->pin_data3 = SDIO_PINS_NC;
    obj->dma0Ch0.resource.type = CYHAL_RSC_INVALID;
    obj->dma0Ch1.resource.type = CYHAL_RSC_INVALID;
    obj->dma1Ch1.resource.type = CYHAL_RSC_INVALID;
    obj->dma1Ch3.resource.type = CYHAL_RSC_INVALID;
    
    obj->clock.div_type = CY_HAL_SDIO_CLK_DIV_NC;

    /* Reserve clock */
    cyhal_resource_inst_t udbClkRsc = { CYHAL_RSC_CLOCK, 0, 0 };
    retVal = cyhal_hwmgr_reserve(&udbClkRsc);
    if (retVal == CY_RSLT_SUCCESS)
    {
        /* Check is the clock divider 0 configured */
        retVal =
        cyhal_hwmgr_is_configured(udbClkRsc.type, udbClkRsc.block_num, udbClkRsc.channel_num, &isConfigured);

        /* UDB SDIO uses hard-coded clock divider 0 */
        if (retVal == CY_RSLT_SUCCESS && !isConfigured)
        {
            /* Assign clock divider */
            cy_en_sysclk_status_t clockAssignResult;

            obj->clock.div_type = CY_SYSCLK_DIV_8_BIT;
            obj->clock.div_num = 0;

            clockAssignResult = Cy_SysClk_PeriphSetDivider(obj->clock.div_type, obj->clock.div_num, 0U);

            if (CY_SYSCLK_SUCCESS == clockAssignResult)
            {
                clockAssignResult = Cy_SysClk_PeriphEnableDivider(obj->clock.div_type, obj->clock.div_num);
            }

            if (CY_SYSCLK_SUCCESS == clockAssignResult)
            {
                clockAssignResult = Cy_SysClk_PeriphAssignDivider(PCLK_UDB_CLOCKS0, obj->clock.div_type, obj->clock.div_num);
            }

            retVal = (clockAssignResult == CY_SYSCLK_SUCCESS) ? CY_RSLT_SUCCESS : CYHAL_SDIO_RSLT_ERR_CLOCK;

            /* Update object with reserved clock info */
            cyhal_hwmgr_set_configured(udbClkRsc.type, udbClkRsc.block_num, udbClkRsc.channel_num);
        }
    }

    /* The DMAs are initialized in SDIO_Init() function, so only reserve */
    if (retVal == CY_RSLT_SUCCESS)
    {
        /* Reserve DMA0 CH0 */
        cyhal_resource_inst_t dmaRsc = { CYHAL_RSC_DMA, 0, 0 };
        retVal = cyhal_hwmgr_reserve(&dmaRsc);

        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_is_configured(dmaRsc.type, dmaRsc.block_num, dmaRsc.channel_num, &isConfigured);

            if (retVal == CY_RSLT_SUCCESS && !isConfigured)
            {
                cyhal_hwmgr_set_configured(dmaRsc.type, dmaRsc.block_num, dmaRsc.channel_num);
            }
        }
    }

    if (retVal == CY_RSLT_SUCCESS)
    {
        /* Reserve DMA0 CH1 */
        cyhal_resource_inst_t dmaRsc = { CYHAL_RSC_DMA, 0, 1 };

        retVal = cyhal_hwmgr_reserve(&dmaRsc);
        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_is_configured(dmaRsc.type, dmaRsc.block_num, dmaRsc.channel_num, &isConfigured);

            if (retVal == CY_RSLT_SUCCESS && !isConfigured)
            {
                cyhal_hwmgr_set_configured(dmaRsc.type, dmaRsc.block_num, dmaRsc.channel_num);
            }
        }
    }
    
    if (retVal == CY_RSLT_SUCCESS)
    {
        /* Reserve DMA1 CH1 */
        cyhal_resource_inst_t dmaRsc = { CYHAL_RSC_DMA, 1, 1 };
        retVal = cyhal_hwmgr_reserve(&dmaRsc);

        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_is_configured(dmaRsc.type, dmaRsc.block_num, dmaRsc.channel_num, &isConfigured);

            if (retVal == CY_RSLT_SUCCESS && !isConfigured)
            {
                cyhal_hwmgr_set_configured(dmaRsc.type, dmaRsc.block_num, dmaRsc.channel_num);
            }
        }
    }

    if (retVal == CY_RSLT_SUCCESS)
    {
        /* Reserve DMA1 CH3 */
        cyhal_resource_inst_t dmaRsc = { CYHAL_RSC_DMA, 1, 3 };
        retVal = cyhal_hwmgr_reserve(&dmaRsc);

        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_is_configured(dmaRsc.type, dmaRsc.block_num, dmaRsc.channel_num, &isConfigured);
            
            if (retVal == CY_RSLT_SUCCESS && !isConfigured)
            {
                cyhal_hwmgr_set_configured(dmaRsc.type, dmaRsc.block_num, dmaRsc.channel_num);
            }
        }
    }

    /* Reserve the clk pin */
    if (retVal == CY_RSLT_SUCCESS)
    {
        cyhal_resource_inst_t pinRsc = cyhal_utils_get_gpio_resource(clk);
        retVal = cyhal_hwmgr_reserve(&pinRsc);
        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_is_configured(pinRsc.type, pinRsc.block_num, pinRsc.channel_num, &isConfigured);
            if (retVal == CY_RSLT_SUCCESS && !isConfigured)
            {
                Cy_GPIO_Pin_Init(CYHAL_GET_PORTADDR(clk), CYHAL_GET_PIN(clk), &pin_clk_config);

                obj->pin_clk = clk;
                cyhal_hwmgr_set_configured(pinRsc.type, pinRsc.block_num, pinRsc.channel_num);
            }
        }
    }

    /* Reserve the cmd pin */
    if (retVal == CY_RSLT_SUCCESS)
    {
        cyhal_resource_inst_t pinRsc = cyhal_utils_get_gpio_resource(cmd);
        retVal = cyhal_hwmgr_reserve(&pinRsc);
        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_is_configured(pinRsc.type, pinRsc.block_num, pinRsc.channel_num, &isConfigured);
            if (retVal == CY_RSLT_SUCCESS && !isConfigured)
            {
                Cy_GPIO_Pin_Init(CYHAL_GET_PORTADDR(cmd), CYHAL_GET_PIN(cmd), &pin_cmd_config);

                obj->pin_cmd = cmd;
                cyhal_hwmgr_set_configured(pinRsc.type, pinRsc.block_num, pinRsc.channel_num);
            }
        }
    }

    /* Reserve the data0 pin */
    if (retVal == CY_RSLT_SUCCESS)
    {
        cyhal_resource_inst_t pinRsc = cyhal_utils_get_gpio_resource(data0);
        retVal = cyhal_hwmgr_reserve(&pinRsc);
        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_is_configured(pinRsc.type, pinRsc.block_num, pinRsc.channel_num, &isConfigured);
            if (retVal == CY_RSLT_SUCCESS && !isConfigured)
            {
                Cy_GPIO_Pin_Init(CYHAL_GET_PORTADDR(data0), CYHAL_GET_PIN(data0), &pin_data_config);

                obj->pin_data0 = data0;
                cyhal_hwmgr_set_configured(pinRsc.type, pinRsc.block_num, pinRsc.channel_num);
            }
        }
    }
    
    /* Reserve the data1 pin */
    if (retVal == CY_RSLT_SUCCESS)
    {
        cyhal_resource_inst_t pinRsc = cyhal_utils_get_gpio_resource(data1);
        retVal = cyhal_hwmgr_reserve(&pinRsc);
        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_is_configured(pinRsc.type, pinRsc.block_num, pinRsc.channel_num, &isConfigured);
            if (retVal == CY_RSLT_SUCCESS && !isConfigured)
            {
                Cy_GPIO_Pin_Init(CYHAL_GET_PORTADDR(data1), CYHAL_GET_PIN(data1), &pin_data_config);

                obj->pin_data1 = data1;
                cyhal_hwmgr_set_configured(pinRsc.type, pinRsc.block_num, pinRsc.channel_num);
            }
        }
    }
    
    /* Reserve the data2 pin */
    if (retVal == CY_RSLT_SUCCESS)
    {
        cyhal_resource_inst_t pinRsc = cyhal_utils_get_gpio_resource(data2);
        retVal = cyhal_hwmgr_reserve(&pinRsc);
        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_is_configured(pinRsc.type, pinRsc.block_num, pinRsc.channel_num, &isConfigured);
            if (retVal == CY_RSLT_SUCCESS && !isConfigured)
            {
                Cy_GPIO_Pin_Init(CYHAL_GET_PORTADDR(data2), CYHAL_GET_PIN(data2), &pin_data_config);

                obj->pin_data2 = data2;
                cyhal_hwmgr_set_configured(pinRsc.type, pinRsc.block_num, pinRsc.channel_num);
            }
        }
    }
    
    /* Reserve the data3 pin */
    if (retVal == CY_RSLT_SUCCESS)
    {
        cyhal_resource_inst_t pinRsc = cyhal_utils_get_gpio_resource(data3);
        retVal = cyhal_hwmgr_reserve(&pinRsc);
        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_is_configured(pinRsc.type, pinRsc.block_num, pinRsc.channel_num, &isConfigured);
            if (retVal == CY_RSLT_SUCCESS && !isConfigured)
            {
                Cy_GPIO_Pin_Init(CYHAL_GET_PORTADDR(data3), CYHAL_GET_PIN(data3), &pin_data_config);

                obj->pin_data3 = data3;
                cyhal_hwmgr_set_configured(pinRsc.type, pinRsc.block_num, pinRsc.channel_num);
            }
        }
    }
    
    /* Reserve UDB SDIO */
    if (retVal == CY_RSLT_SUCCESS)
    {
        /* UDB SDIO resource */
        cyhal_resource_inst_t udbRsc = { CYHAL_RSC_UDB, 0, 0 };
    
        retVal = cyhal_hwmgr_reserve(&udbRsc);
        if (retVal == CY_RSLT_SUCCESS)
        {
            /* Check is the UDB configured */
            retVal = cyhal_hwmgr_is_configured(udbRsc.type, udbRsc.block_num, udbRsc.channel_num, &isConfigured);
            if (retVal == CY_RSLT_SUCCESS && !isConfigured)
            {
                /* Update SDIO object */
                obj->resource.type = udbRsc.type;
                obj->resource.block_num = udbRsc.block_num;
                obj->resource.channel_num = udbRsc.channel_num;
                
                /* Set default interrupt priority of 7 (lowest possible priority) */
                cy_stc_sysint_t irqCfg = {udb_interrupts_0_IRQn, 7};
                Cy_SysInt_Init(&irqCfg, &SDIO_IRQ);
                NVIC_EnableIRQ(udb_interrupts_0_IRQn);
                
                /* Setup write DMA interrupt handler */
                cy_stc_sysint_t irqDma1_1 = {cpuss_interrupts_dw1_1_IRQn, 7};
                Cy_SysInt_Init(&irqDma1_1, &SDIO_WRITE_DMA_IRQ);
                NVIC_EnableIRQ(cpuss_interrupts_dw1_1_IRQn);
                
                /* Setup read DMA interrupt handler */
                cy_stc_sysint_t irqDma1_3 = {cpuss_interrupts_dw1_3_IRQn, 7};
                Cy_SysInt_Init(&irqDma1_3, &SDIO_READ_DMA_IRQ);
                NVIC_EnableIRQ(cpuss_interrupts_dw1_3_IRQn);

                /* Connect UDB to DMA */
                Cy_TrigMux_Connect(TRIG0_IN_TR_GROUP14_OUTPUT1, TRIG0_OUT_CPUSS_DW0_TR_IN1, false, TRIGGER_TYPE_LEVEL);
                Cy_TrigMux_Connect(TRIG0_IN_TR_GROUP14_OUTPUT4, TRIG0_OUT_CPUSS_DW0_TR_IN0, false, TRIGGER_TYPE_LEVEL);
                Cy_TrigMux_Connect(TRIG1_IN_TR_GROUP14_OUTPUT0, TRIG1_OUT_CPUSS_DW1_TR_IN1, false, TRIGGER_TYPE_LEVEL);
                Cy_TrigMux_Connect(TRIG1_IN_TR_GROUP14_OUTPUT5, TRIG1_OUT_CPUSS_DW1_TR_IN3, false, TRIGGER_TYPE_LEVEL);
                
                Cy_TrigMux_Connect(TRIG14_IN_UDB_TR_UDB0, TRIG14_OUT_TR_GROUP1_INPUT43, false, TRIGGER_TYPE_LEVEL);
                Cy_TrigMux_Connect(TRIG14_IN_UDB_TR_UDB1, TRIG14_OUT_TR_GROUP0_INPUT44, false, TRIGGER_TYPE_LEVEL);
                Cy_TrigMux_Connect(TRIG14_IN_UDB_TR_UDB3, TRIG14_OUT_TR_GROUP0_INPUT47, false, TRIGGER_TYPE_LEVEL);
                Cy_TrigMux_Connect(TRIG14_IN_UDB_TR_UDB7, TRIG14_OUT_TR_GROUP1_INPUT48, false, TRIGGER_TYPE_LEVEL);
                
                stc_sdio_irq_cb_t   irq_cbs;
                irq_cbs.pfnCardIntCb = cyhal_sdio_interrupts_dispatcher_IRQHandler;

                SDIO_Init(&irq_cbs);

                /* SDIO_Init() configures the SDIO to 40 kHz */
                obj->frequencyhal_hz = CY_HAL_SDIO_DEF_FREQ;
                obj->block_size   = CY_HAL_SDIO_64B;

                retVal = cyhal_hwmgr_set_configured(udbRsc.type, udbRsc.block_num, udbRsc.channel_num);
            }
        }
    }

    if (retVal != CY_RSLT_SUCCESS)
    {
        cyhal_sdio_free(obj);
    }

    return retVal;
}


/** Release the UDB SDIO and reserved peripheral.
 *
 * @param[in,out] obj The SDIO object
 * @return The status of the free request
 */
cy_rslt_t cyhal_sdio_free(cyhal_sdio_t *obj)
{
    cy_rslt_t retVal;
    
    if (NULL == obj)
    {
        return CYHAL_SDIO_RSLT_ERR_BAD_PARAM;
    }
    
    retVal = cyhal_free_pins(obj);

    if (retVal == CY_RSLT_SUCCESS)
    {
        retVal = cyhal_free_clocks(obj);
    }
    
    if (retVal == CY_RSLT_SUCCESS)
    {
        retVal = cyhal_free_dmas(obj);
    }
    
    if ((retVal == CY_RSLT_SUCCESS) && (obj->resource.type != CYHAL_RSC_INVALID))
    {
        /* All other resources were released so need to release UDB resource */
        retVal = cyhal_hwmgr_set_unconfigured(obj->resource.type, obj->resource.block_num, obj->resource.channel_num);
        
        /* This is the only available function to disable functionality */
        SDIO_DisableSdClk();
        
        if (retVal == CY_RSLT_SUCCESS)
        {
            retVal = cyhal_hwmgr_free(&(obj->resource));
        }
    }
    return retVal;
}


/** Configure the SDIO block.
 *
 * @param[in,out] obj    The SDIO object
 * @param[in]     config The SDIO configuration to apply
 * @return The status of the configure request
 */
cy_rslt_t cyhal_sdio_configure(cyhal_sdio_t *obj, const cyhal_sdio_cfg_t *config)
{
    if (NULL == obj)
    {
        return CYHAL_SDIO_RSLT_ERR_BAD_PARAM;
    }

    if (config->frequencyhal_hz != 0)
    {
        SDIO_SetSdClkFrequency(config->frequencyhal_hz);
        obj->frequencyhal_hz = config->frequencyhal_hz;
    }
    
    if (config->block_size != 0)
    {
        SDIO_SetBlockSize(config->block_size);
        obj->block_size   = config->block_size;
    }

    return CY_RSLT_SUCCESS;
}


/** Sends a command to the SDIO block.
 *
 * @param[in,out] obj       The SDIO object
 * @param[in]     direction The direction of transfer (read/write)
 * @param[in]     command   The SDIO command to send
 * @param[in]     argument  The argument to the command
 * @param[out]    response  The response from the SDIO device
 * @return The status of the configure request
 */
cy_rslt_t cyhal_sdio_send_cmd(const cyhal_sdio_t *obj, cyhal_transfer_t direction, cyhal_sdio_command_t command, uint32_t argument, uint32_t* response)
{
    if ((NULL == obj) && (command == CYHAL_SDIO_CMD_IO_RW_EXTENDED))
    {
        return CYHAL_SDIO_RSLT_ERR_BAD_PARAM;
    }
    
    uint32_t cmdResponse;
    stc_sdio_cmd_t cmd;
    en_sdio_result_t status;
    cy_rslt_t retVal = CY_RSLT_SUCCESS;

    if (response != NULL)
    {
        *response = 0;
    }

    cmd.u32CmdIdx = (uint32_t) command;
    cmd.u32Arg = argument;
    cmd.pu32Response = &cmdResponse;
    cmd.pu8Data = NULL;              /* Not used */
    cmd.bRead = (direction != CYHAL_READ) ? false : true;
    cmd.u16BlockCnt = 0U;            /* Not used */
    cmd.u16BlockSize = 0U;           /* Not used */

    status = SDIO_SendCommandAndWait(&cmd);

    if (Ok != status)
    {
        retVal = CYHAL_SDIO_RSLT_ERR_FUNC_RET(status);
    }

    if (response != NULL)
    {
        *response = cmdResponse;
    }

    return retVal;
}


/** Performs a bulk data transfer (CMD=53) to the SDIO block.
 *
 * @param[in,out] obj       The SDIO object
 * @param[in]     direction The direction of transfer (read/write)
 * @param[in]     argument  The argument to the command
 * @param[in]     data      The data to send to the SDIO device
 * @param[in]     length    The number of bytes to send
 * @param[out]    response  The response from the SDIO device
 * @return The status of the configure request
 */
cy_rslt_t cyhal_sdio_bulk_transfer(cyhal_sdio_t *obj, cyhal_transfer_t direction, uint32_t argument, const uint32_t* data, uint16_t length, uint32_t* response)
{
    if (NULL == obj)
    {
        return CYHAL_SDIO_RSLT_ERR_BAD_PARAM;
    }

    stc_sdio_cmd_t cmd;
    en_sdio_result_t status;
    uint32_t cmdResponse;
    cy_rslt_t retVal = CY_RSLT_SUCCESS;;

    if (response != NULL)
    {
        *response = 0;
    }

    cmd.u32CmdIdx = (uint32_t) CYHAL_SDIO_CMD_IO_RW_EXTENDED;
    cmd.u32Arg = argument;
    cmd.pu32Response = &cmdResponse;

    /* Note that this implementation uses 8b address */
    cmd.pu8Data = (uint8_t *) data;
    cmd.bRead = (direction != CYHAL_READ) ? false : true;

    /* Cypress ID BSP-542 */
    uint8_t* tempBuffer = (uint8_t*)malloc(length + obj->block_size - 1);
    if (length >= obj->block_size)
    {
        cmd.u16BlockCnt = (uint16_t) ((length + obj->block_size - 1)/obj->block_size);
        cmd.u16BlockSize = obj->block_size;

        if (cmd.bRead)
        {
            cmd.pu8Data = tempBuffer;
        }
    }
    else
    {
        /* Data will be sent as one packet */
        cmd.u16BlockCnt = CY_HAL_SDIO_1B;
        cmd.u16BlockSize = length;
    }

    status = SDIO_SendCommandAndWait(&cmd);

    if (Ok != status)
    {
        retVal = CYHAL_SDIO_RSLT_ERR_FUNC_RET(status);
    }

    if(retVal == CY_RSLT_SUCCESS && length >= obj->block_size && cmd.bRead)
    {
        memcpy((uint8_t *)data, tempBuffer, (size_t)length);
    }

    if (response != NULL)
    {
        *response = cmdResponse;
    }

    free(tempBuffer);

    return retVal;
}

/** Performs a bulk asynchronus data transfer (CMD=53) to the SDIO block.
 *
 * @param[in,out] obj       The SDIO object
 * @param[in]     direction The direction of transfer (read/write)
 * @param[in]     argument  The argument to the command
 * @param[in]     data      The data to send to the SDIO device
 * @param[in]     length    The number of bytes to send
 * @return The status of the configure request
 */
cy_rslt_t cyhal_sdio_transfer_async(cyhal_sdio_t *obj, cyhal_transfer_t direction, uint32_t argument, const uint32_t* data, uint16_t length)
{
    /* UDB SDIO implementation does not support async transfers */
    
    /* Just add check to suppress warning about unused arguments */
    if ((NULL == obj) && (data == NULL) && (length == 0) && (argument == 0) && (direction == ((cyhal_transfer_t) 0x3)))
    {
        return CYHAL_SDIO_RSLT_ERR_BAD_PARAM;
    }

    return CYHAL_SDIO_FUNC_NOT_SUPPORTED;
}


/** Checks if the specified SDIO is in use
 *
 * @param[in]  obj  The SDIO peripheral to check
 * @param[out] busy Indication of whether the SDIO is still transmitting
 * @return The status of the is_busy request
 */
cy_rslt_t cyhal_sdio_is_busy(const cyhal_sdio_t *obj, bool *busy)
{
    /* UDB SDIO does not support async transfers */
    if ((obj == NULL) && (busy == NULL))
    {
        return CYHAL_SDIO_RSLT_ERR_BAD_PARAM;
    }

    return CYHAL_SDIO_FUNC_NOT_SUPPORTED;
}

/** Abort an SDIO transfer
 *
 * @param[in] obj The SDIO peripheral to stop. Resets UDB block
 * @return The status of the abort_async request
 */
cy_rslt_t cyhal_sdio_abort_async(const cyhal_sdio_t *obj)
{
    /* Reset UDB SDIO */
    SDIO_Reset();
    return CY_RSLT_SUCCESS;
}

/** The sdio interrupt handler registration
 *
 * @param[in] obj         The SDIO object
 * @param[in] handler     The callback handler which will be invoked when the interrupt fires
 * @param[in] handler_arg Generic argument that will be provided to the handler when called
 * @return The status of the register_irq request
 */
cy_rslt_t cyhal_sdio_register_irq(cyhal_sdio_t *obj, cyhal_sdio_irq_handler handler, void *handler_arg)
{
    if (obj == NULL)
    {
        return CYHAL_SDIO_RSLT_ERR_BAD_PARAM;
    }

    cyhal_sdio_config_struct = obj;
    cyhal_sdio_callback = handler;
    cyhal_sdio_callback_args = handler_arg;

    return CY_RSLT_SUCCESS;
}

/** Configure SDIO interrupt.
 *
 * @param[in] obj      The SDIO object
 * @param[in] event    The SDIO IRQ type
 * @param[in] enable   Set to non-zero to enable events, or zero to disable them
 * @return The status of the irq_enable request
 */
cy_rslt_t cyhal_sdio_irq_enable(cyhal_sdio_t *obj, cyhal_sdio_irq_event_t event, bool enable)
{
    if (enable)
    {
        obj->irq_cause |= event;
        
        if (cyhal_sdio_config_struct != NULL)
        {
            cyhal_sdio_config_struct->irq_cause |= event;
        }
        
        SDIO_EnableChipInt();
    }
    else
    {
        obj->irq_cause &= ~event;

        if (cyhal_sdio_config_struct != NULL)
        {
            cyhal_sdio_config_struct->irq_cause &= ~event;
        }
        
        SDIO_DisableChipInt();
    }
    return CY_RSLT_SUCCESS;
}


#endif /* defined(CY8C6247BZI_D54) */
