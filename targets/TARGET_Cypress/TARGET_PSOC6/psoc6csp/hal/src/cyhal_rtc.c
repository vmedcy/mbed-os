/***************************************************************************//**
* \file cyhal_rtc.c
*
* \brief
* Provides a high level interface for interacting with the Cypress Real-Time Clock.
* This interface abstracts out the chip specific details. If any chip specific
* functionality is necessary, or performance is critical the low level functions
* can be used directly.
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

#include "cyhal_rtc.h"
#include "cyhal_hwmgr.h"
#include "cy_rtc.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define CY_RTC_STATE_UNINITIALIZED 0
#define CY_RTC_STATE_ENABLED 1
#define CY_RTC_STATE_TIME_SET 2
#define CY_RTC_DEFAULT_PRIORITY 5
#define CY_TM_YEAR_BASE 1900

// Cy_RTC_DeepSleepCallback does not have a compatible signature
static cy_en_syspm_status_t cyhal_rtc_syspm_callback(cy_stc_syspm_callback_params_t *params, cy_en_syspm_callback_mode_t mode)
{
    return Cy_RTC_DeepSleepCallback(params, mode);
}

static cy_stc_syspm_callback_params_t cyhal_rtc_pm_cb_params = {NULL, NULL};
static cy_stc_syspm_callback_t cyhal_rtc_pm_cb = {
    .callback = &cyhal_rtc_syspm_callback,
    .type = CY_SYSPM_DEEPSLEEP,
    .callbackParams = &cyhal_rtc_pm_cb_params,
};
static cyhal_rtc_irq_handler cyhal_rtc_user_handler;
static void *cyhal_rtc_handler_arg;
#define CYHAL_RTC_INITIAL_CENTURY 1900
static uint16_t cyhal_rtc_century = CYHAL_RTC_INITIAL_CENTURY;
static uint8_t cyhal_rtc_initialized = CY_RTC_STATE_UNINITIALIZED;

static void cyhal_rtc_internal_handler(void)
{
    Cy_RTC_Interrupt(NULL, false);
}

void Cy_RTC_Alarm1Interrupt(void)
{
    if (NULL != cyhal_rtc_user_handler)
    {
        (*cyhal_rtc_user_handler)(cyhal_rtc_handler_arg, CYHAL_RTC_ALARM);
    }
}

void Cy_RTC_CenturyInterrupt(void)
{
    cyhal_rtc_century += 100;
}

cy_rslt_t cyhal_rtc_init(cyhal_rtc_t *obj)
{
    cy_rslt_t rslt = CY_RSLT_SUCCESS;
    if (cyhal_rtc_initialized == CY_RTC_STATE_UNINITIALIZED)
    {
        cy_stc_rtc_config_t dateTime;
        Cy_RTC_GetDateAndTime(&dateTime);
        if (0 == dateTime.dayOfWeek || 0 == dateTime.date || 0 == dateTime.month)
        {
            // Date is not valid; reset to default time
            dateTime.sec = 0;
            dateTime.min = 0;
            dateTime.hour = 0;
            dateTime.amPm = CY_RTC_AM;
            dateTime.hrFormat = CY_RTC_24_HOURS;
            dateTime.dayOfWeek = CY_RTC_THURSDAY;
            dateTime.date = 1;
            dateTime.month = 1;
            dateTime.year = 70;
            Cy_RTC_SetDateAndTime(&dateTime);
        }
        else
        {
            // Time is already set (possibly after sw reset). Assume century.
            cyhal_rtc_century = 2000;
        }
        Cy_RTC_ClearInterrupt(CY_RTC_INTR_CENTURY);
        Cy_RTC_SetInterruptMask(CY_RTC_INTR_CENTURY);
        static const cy_stc_sysint_t irqCfg = {srss_interrupt_backup_IRQn, CY_RTC_DEFAULT_PRIORITY};
        Cy_SysInt_Init(&irqCfg, &cyhal_rtc_internal_handler);
        NVIC_EnableIRQ(srss_interrupt_backup_IRQn);
        Cy_SysPm_RegisterCallback(&cyhal_rtc_pm_cb);
        cyhal_rtc_initialized = CY_RTC_STATE_ENABLED;
    }
    return rslt;
}

cy_rslt_t cyhal_rtc_free(cyhal_rtc_t *obj)
{
    Cy_RTC_SetInterruptMask(CY_RTC_INTR_CENTURY);
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_rtc_is_enabled(cyhal_rtc_t *obj)
{
    return cyhal_rtc_initialized == CY_RTC_STATE_TIME_SET ? CY_RSLT_SUCCESS : CY_RSLT_RTC_NOT_INITIALIZED;
}

cy_rslt_t cyhal_rtc_read(cyhal_rtc_t *obj, struct tm *time)
{
    // The number of days that precede each month of the year, not including Fdb 29
    static const uint16_t CUMULATIVE_DAYS[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    cy_stc_rtc_config_t dateTime;
    Cy_RTC_GetDateAndTime(&dateTime);
    int year = dateTime.year + cyhal_rtc_century;
    time->tm_sec = dateTime.sec;
    time->tm_min = dateTime.min;
    time->tm_hour = dateTime.hour;
    time->tm_mday = dateTime.date;
    time->tm_mon = dateTime.month - 1u;
    time->tm_year = year - CY_TM_YEAR_BASE;
    time->tm_wday = dateTime.dayOfWeek - 1u;
    time->tm_yday = CUMULATIVE_DAYS[time->tm_mon] + dateTime.date - 1u +
        ((dateTime.month >= 3 && Cy_RTC_IsLeapYear(year)) ? 1u : 0u);
    time->tm_isdst = -1;
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_rtc_write(cyhal_rtc_t *obj, const struct tm *time)
{
    uint32_t year2digit = time->tm_year % 100;
    cy_stc_rtc_config_t newtime = {
    .sec = time->tm_sec,
    .min = time->tm_min,
    .hour = time->tm_hour,
    .hrFormat = CY_RTC_24_HOURS,
    .dayOfWeek = time->tm_wday + 1,
    .date = time->tm_mday,
    .month = time->tm_mon + 1,
    .year = year2digit
    };
    NVIC_DisableIRQ(srss_interrupt_backup_IRQn);
    cy_rslt_t rslt = (cy_rslt_t)Cy_RTC_SetDateAndTime(&newtime);
    cyhal_rtc_century = time->tm_year - year2digit + CY_TM_YEAR_BASE;
    NVIC_EnableIRQ(srss_interrupt_backup_IRQn);
    while (CY_RTC_BUSY == Cy_RTC_GetSyncStatus()) { }
    if (rslt == CY_RSLT_SUCCESS)
        cyhal_rtc_initialized = CY_RTC_STATE_TIME_SET;
    return rslt;
}

cy_rslt_t cyhal_rtc_alarm(cyhal_rtc_t *obj, const struct tm *time)
{
    // Note: the hardware does not support year matching
    cy_stc_rtc_alarm_t alarm = {
    .sec = time->tm_sec,
    .secEn = CY_RTC_ALARM_ENABLE,
    .min = time->tm_min,
    .minEn = CY_RTC_ALARM_ENABLE,
    .hour = time->tm_hour,
    .hourEn = CY_RTC_ALARM_ENABLE,
    .dayOfWeek = 1,
    .dayOfWeekEn = CY_RTC_ALARM_DISABLE,
    .date = time->tm_mday,
    .dateEn = CY_RTC_ALARM_ENABLE,
    .month = time->tm_mon + 1,
    .monthEn = CY_RTC_ALARM_ENABLE,
    .almEn = CY_RTC_ALARM_ENABLE
    };
    return (cy_rslt_t)Cy_RTC_SetAlarmDateAndTime(&alarm, CY_RTC_ALARM_1);
}

cy_rslt_t cyhal_rtc_register_irq(cyhal_rtc_t *obj, cyhal_rtc_irq_handler handler, void *handler_arg)
{
    NVIC_DisableIRQ(srss_interrupt_backup_IRQn);
    cyhal_rtc_handler_arg = handler_arg;
    cyhal_rtc_user_handler = handler;
    NVIC_EnableIRQ(srss_interrupt_backup_IRQn);
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_rtc_irq_enable(cyhal_rtc_t *obj, cyhal_rtc_irq_event_t event, bool enable)
{
    Cy_RTC_ClearInterrupt(~Cy_RTC_GetInterruptMask());
    Cy_RTC_SetInterruptMask((enable ? CY_RTC_INTR_ALARM1 : 0) | CY_RTC_INTR_CENTURY);
    return CY_RSLT_SUCCESS;
}

#if defined(__cplusplus)
}
#endif
