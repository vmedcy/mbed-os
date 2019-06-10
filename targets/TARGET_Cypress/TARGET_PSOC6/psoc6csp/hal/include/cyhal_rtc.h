/***************************************************************************//**
* \file cyhal_rtc.h
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

/**
* \addtogroup group_hal_rtc RTC (Real-Time Clock)
* \ingroup group_hal
* \{
* High level interface for interacting with the Cypress RTC.
*
* \defgroup group_hal_rtc_macros Macros
* \defgroup group_hal_rtc_functions Functions
* \defgroup group_hal_rtc_data_structures Data Structures
* \defgroup group_hal_rtc_enums Enumerated Types
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "cy_result.h"
#include "cyhal_hw_types.h"

/** RTC not initialized */
#define CY_RSLT_RTC_NOT_INITIALIZED CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_RTC, 0)

#if defined(__cplusplus)
extern "C" {
#endif

/**
* \addtogroup group_hal_rtc_enums
* \{
*/

/** RTC interrupt triggers */
typedef enum {
    CYHAL_RTC_ALARM,
} cyhal_rtc_irq_event_t;

/** \} group_hal_rtc_enums */


/**
* \addtogroup group_hal_rtc_data_structures
* \{
*/

/** Handler for RTC interrupts */
typedef void (*cyhal_rtc_irq_handler)(void *handler_arg, cyhal_rtc_irq_event_t event);

/** \} group_hal_rtc_data_structures */


/**
* \addtogroup group_hal_rtc_functions
* \{
*/

/** Initialize the RTC peripheral
 *
 * Powerup the RTC in perpetration for access. This function must be called
 * before any other RTC functions ares called. This does not change the state
 * of the RTC. It just enables access to it.
 *
 * @param[out] obj RTC object
 * @return The status of the init request
 */
cy_rslt_t cyhal_rtc_init(cyhal_rtc_t *obj);

/** Deinitialize RTC
 *
 * Powerdown the RTC in preparation for sleep, powerdown or reset. That should only
 * affect the CPU domain and not the time keeping logic.
 * After this function is called no other RTC functions should be called
 * except for rtc_init.
 *
 * @param[in,out] obj RTC object
 */
void cyhal_rtc_free(cyhal_rtc_t *obj);

/** Check if the RTC has the time set and is counting
 *
 * @param[in] obj RTC object
 * @return Whether the RTC is enabled or not
 */
bool cyhal_rtc_is_enabled(cyhal_rtc_t *obj);

/** Get the current time from the RTC peripheral
 *
 * @param[in]  obj RTC object
 * @param[out] time The current time
 * @return The status of the read request
 */
cy_rslt_t cyhal_rtc_read(cyhal_rtc_t *obj, struct tm *time);

/** Write the current time in seconds to the RTC peripheral
 *
 * @param[in] obj  RTC object
 * @param[in] time The time to be set
 * @return The status of the write request
 */
cy_rslt_t cyhal_rtc_write(cyhal_rtc_t *obj, const struct tm *time);

/** Set an alarm for the specified time in seconds to the RTC peripheral
 *
 * @param[in] obj  RTC object
 * @param[in] time The alarm time to be set
 * @return The status of the alarm request
 */
cy_rslt_t cyhal_rtc_alarm(cyhal_rtc_t *obj, const struct tm *time);

/** The RTC alarm interrupt handler registration
 *
 * @param[in] obj         The RTC object
 * @param[in] handler     The callback handler which will be invoked when the alarm fires
 * @param[in] handler_arg Generic argument that will be provided to the handler when called
 */
void cyhal_rtc_register_irq(cyhal_rtc_t *obj, cyhal_rtc_irq_handler handler, void *handler_arg);

/** Configure RTC alarm interrupt enablement.
 *
 * @param[in] obj      The RTC object
 * @param[in] event    The RTC IRQ type
 * @param[in] enable   True to turn on interrupts, False to turn off
 */
void cyhal_rtc_irq_enable(cyhal_rtc_t *obj, cyhal_rtc_irq_event_t event, bool enable);

/** \} group_hal_rtc_functions */

#if defined(__cplusplus)
}
#endif

/** \} group_hal_rtc */
