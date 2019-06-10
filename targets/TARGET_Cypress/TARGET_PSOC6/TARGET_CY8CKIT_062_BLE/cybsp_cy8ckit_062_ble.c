/***************************************************************************//**
* \file cybsp_cy8ckit_062_ble.c
*
* Description:
* Provides APIs for interacting with the hardware contained on the Cypress
* CY8CKIT-062-BLE pioneer kit.
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
#include "cybsp_cy8ckit_062_ble.h"
#include "cycfg.h"
#include "cyhal_implementation.h"

#if defined(__cplusplus)
extern "C" {
#endif

cyhal_qspi_t cybsp_qspi;
cyhal_uart_t cybsp_uart;
cyhal_i2c_t cybsp_i2c;
cyhal_rtc_t cybsp_rtc;

static cy_rslt_t init_peripherals(void)
{
    cy_rslt_t result = cyhal_qspi_init(&cybsp_qspi, CYBSP_QSPI_D0, CYBSP_QSPI_D1, CYBSP_QSPI_D2, CYBSP_QSPI_D3,
            NC, NC, NC, NC, CYBSP_QSPI_SCK, CYBSP_QSPI_SS, 50000000U, 0);

    result = cyhal_uart_init(&cybsp_uart, CYBSP_UART_TX, CYBSP_UART_RX, NULL, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    result = cyhal_i2c_init(&cybsp_i2c, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    return cyhal_rtc_init(&cybsp_rtc);
}

static cy_rslt_t reserve_pin(cyhal_gpio_t pin)
{
    cyhal_resource_inst_t pin_inst = cyhal_utils_get_gpio_resource(pin);
    return cyhal_hwmgr_reserve(&pin_inst);
}

static cy_rslt_t reserve_pins(void)
{
    // Pins not connected to peripherals must be reserve manually.
    cy_rslt_t result = reserve_pin(CYBSP_WCO_IN);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    result = reserve_pin(CYBSP_WCO_OUT);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    result = reserve_pin((cyhal_gpio_t)CYBSP_USER_BTN0);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    result = reserve_pin(CYBSP_SWDIO);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    result = reserve_pin((cyhal_gpio_t)CYBSP_LED_RGB_RED);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    result = reserve_pin((cyhal_gpio_t)CYBSP_LED_RGB_BLUE);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    result = reserve_pin((cyhal_gpio_t)CYBSP_LED_RGB_GREEN);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    result = reserve_pin((cyhal_gpio_t)CYBSP_USER_LED0);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    return reserve_pin((cyhal_gpio_t)CYBSP_USER_LED1);
}

static cy_rslt_t reserve_peri_diviers(void)
{
    // Clock dividers that are preconfigured must be manually reserved.
    // Divider 0 is used for This kit need 4 dividers allocated.
    cyhal_resource_inst_t divider;
    divider.type = CYHAL_RSC_CLOCK;
    // USB
    divider.block_num = CY_SYSCLK_DIV_16_BIT; // 16 bit diviers
    divider.channel_num = peri_0_div_16_0_NUM;
    cy_rslt_t result = cyhal_hwmgr_reserve(&divider);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    // I2C
    divider.block_num = CY_SYSCLK_DIV_8_BIT; // 8 bit diviers
    divider.channel_num = peri_0_div_8_1_NUM;
    result = cyhal_hwmgr_reserve(&divider);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    // UART, BT_UART
    divider.channel_num = peri_0_div_8_2_NUM;
    result = cyhal_hwmgr_reserve(&divider);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    // CSD
    divider.channel_num = peri_0_div_8_3_NUM;
    return cyhal_hwmgr_reserve(&divider);
}

cy_rslt_t cybsp_init(void)
{
    init_cycfg_all();
    cy_rslt_t result = reserve_pins();
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    result = reserve_peri_diviers();
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    result = init_peripherals();
    return result;
}

cy_rslt_t cybsp_led_init(cybsp_led_t which)
{
    return cyhal_gpio_init((cyhal_gpio_t)which, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
}

void cybsp_led_set_state(cybsp_led_t which, bool on)
{
    cyhal_gpio_write((cyhal_gpio_t)which, on);
}

void cybsp_led_toggle(cybsp_led_t which)
{
    cyhal_gpio_toggle((cyhal_gpio_t)which);
}

cy_rslt_t cybsp_btn_init(cybsp_btn_t which)
{
    return cyhal_gpio_init((cyhal_gpio_t)which, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);
}

bool cybsp_btn_get_state(cybsp_btn_t which)
{
    return cyhal_gpio_read((cyhal_gpio_t)which);
}

static void (*btn_interrupt_call_back) (void);
static void gpio_call_back_wrapper(void *handler_arg, cyhal_gpio_irq_event_t event)
{
    if (btn_interrupt_call_back != NULL)
    {
        btn_interrupt_call_back();
    }
}

void cybsp_btn_set_interrupt(cybsp_btn_t which, cyhal_gpio_irq_event_t type, void (*callback)(void))
{
    btn_interrupt_call_back = callback;
    cyhal_gpio_register_irq((cyhal_gpio_t)which, 7, &gpio_call_back_wrapper, NULL);
    cyhal_gpio_irq_enable((cyhal_gpio_t)which, type, 1);
}

#if defined(__cplusplus)
}
#endif
