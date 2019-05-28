/*
 * mbed Microcontroller Library
 * Copyright (c) 2017-2018 Future Electronics
 * Copyright (c) 2018-2019 Cypress Semiconductor Corporation
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

#include "lp_ticker_api.h"
#include "mbed_error.h"
#include "cyhal_wdt.h"
#include "cycfg.h"

#if DEVICE_LPTICKER

#ifdef __cplusplus
extern "C" {
#endif

static cyhal_wdt_t cy_wdt0;
static bool cy_wdt_initialized = false;

static void cy_lp_ticker_handler(MBED_UNUSED void *unused1, MBED_UNUSED cyhal_wdt_irq_event_t unused2)
{
    lp_ticker_irq_handler();
}

void lp_ticker_init(void)
{
    // It should be safe to call this function more than once
    if (!cy_wdt_initialized)
    {
        if (CY_RSLT_SUCCESS != cyhal_wdt_init(&cy_wdt0))
            MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_wdt_init");
        cy_wdt_initialized = true;
    }
    lp_ticker_disable_interrupt();
    if (CY_RSLT_SUCCESS != cyhal_wdt_register_irq(&cy_wdt0, &cy_lp_ticker_handler, NULL))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_wdt_register_irq");
}

void lp_ticker_free(void)
{
    if (CY_RSLT_SUCCESS != cyhal_wdt_free(&cy_wdt0))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_wdt_free");
    cy_wdt_initialized = false;
}

uint32_t lp_ticker_read(void)
{
    uint32_t time;
    if (CY_RSLT_SUCCESS != cyhal_wdt_read(&cy_wdt0, &time))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_wdt_read");
    return time;
}

void lp_ticker_set_interrupt(timestamp_t timestamp)
{
    if (CY_RSLT_SUCCESS != cyhal_wdt_set_match(&cy_wdt0, timestamp))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_wdt_set_time");
    if (CY_RSLT_SUCCESS != cyhal_wdt_irq_enable(&cy_wdt0, CYHAL_WDT_COMPARE_MATCH, true))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_wdt_irq_enable");
}

void lp_ticker_disable_interrupt(void)
{
    if (CY_RSLT_SUCCESS != cyhal_wdt_irq_enable(&cy_wdt0, CYHAL_WDT_COMPARE_MATCH, false))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_wdt_irq_enable");
}

void lp_ticker_clear_interrupt(void)
{
    // WDT driver does this automatically; nothing to do
}

void lp_ticker_fire_interrupt(void)
{
    if (CY_RSLT_SUCCESS != cyhal_wdt_irq_trigger(&cy_wdt0))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_wdt_irq_trigger");
}

const ticker_info_t *lp_ticker_get_info(void)
{
    static const ticker_info_t info = {
    /* .frequency = */ CY_CFG_SYSCLK_CLKLF_FREQ_HZ,
    /* .bits =      */ 16
    };
    return &info;
}

#ifdef __cplusplus
}
#endif

#endif
