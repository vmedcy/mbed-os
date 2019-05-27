/***************************************************************************//**
* \file cyhal_system.h
*
* \brief
* Provides a high level interface for interacting with the Cypress power
* management and system clock configuration. This interface abstracts out the
* chip specific details. If any chip specific functionality is necessary, or
* performance is critical the low level functions can be used directly.
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
* \addtogroup group_hal_system SYSTEM (Power Management and System Clock)
* \ingroup group_hal
* \{
* High level interface for interacting with the Cypress power management
* and system clock configuration.
*
* \defgroup group_hal_system_macros Macros
* \defgroup group_hal_system_functions Functions
* \defgroup group_hal_system_data_structures Data Structures
* \defgroup group_hal_system_enums Enumerated Types
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "cy_result.h"
#include "cyhal_implementation.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
* \addtogroup group_hal_system_macros
* \{
*/

/** \} group_hal_system_macros */


/**
* \addtogroup group_hal_system_functions
* \{
*/

/** Enter a critical section
 *
 * Disables interrupts and returns a value indicating whether the interrupts were previously 
 * enabled.
 *
 * @return Returns the state before entering the critical section. This value must be provided
 * to \ref cyhal_system_critical_section_exit() to properly restore the state
 */
uint32_t cyhal_system_critical_section_enter(void);

/** Exit a critical section
 *
 * Re-enables the interrupts if they were enabled before
*  cyhal_system_critical_section_enter() was called. The argument should be the value
*  returned from \ref cyhal_system_critical_section_enter().
 *
 * @param[in] oldState The state of interrupts from cyhal_system_critical_section_enter()
 */
void cyhal_system_critical_section_exit(uint32_t oldState);

/** Send the device to sleep
 *
 * The processor is setup ready for sleep, and sent to sleep using __WFI(). In this mode, the
 * system clock to the core is stopped until a reset or an interrupt occurs.
 *
 * @return Returns CY_RSLT_SUCCESS if the processor successfully entered and exited sleep,
 * otherwise error
 */
cy_rslt_t cyhal_system_sleep(void);

/** Send the device to deep sleep
 *
 * This processor is setup ready for deep sleep, and sent to sleep using __WFI(). This mode
 * has the same sleep features as sleep plus it powers down peripherals and clocks. All state
 * is still maintained.
 *
 * @return Returns CY_RSLT_SUCCESS if the processor successfully entered and exited sleep,
 * otherwise error
 */
cy_rslt_t cyhal_system_deepsleep(void);

/** Register the specified handler with the power manager to be notified when the power
 *  state changes.
 *
 * @param[in] handler The handler to notify on power transitions
 * @return The status of the register_callback request
 */
cy_rslt_t cyhal_system_register_callback(cy_stc_syspm_callback_t *handler);

/** Removes the specified handler from the power manager so no future notification are made.
 *
 * @param[in] handler The handler to remove from receiving notifications
 * @return The status of the unregister_callback request
 */
cy_rslt_t cyhal_system_unregister_callback(cy_stc_syspm_callback_t const *handler);

/** Sets the specified clock's frequency and enables it.
 *  This will turn on any additional clocks needed to drive this.
 *
 * @param[in]  clock        The high frequency clock to configure
 * @param[in]  frequencyhal_hz The frequency to run the clock at
 * @return The status of the clock_frequency request
 */
cy_rslt_t cyhal_system_clock_frequency(uint8_t clock, uint32_t frequencyhal_hz);

/** Divides the clock driving the CM4 from what system clock 0 is
 *  running at
 *
 * @param[in]  divider The amount to divide the system clock by
 * @return The status of the cm4_divider request
 */
cy_rslt_t cyhal_system_cm4_divider(uint8_t divider);

/** Divides the clock driving the peripherals from what system
 *  clock 0 is running at
 *
 * @param[in]  divider The amount to divide the system clock by
 * @return The status of the peri_divider request
 */
cy_rslt_t cyhal_system_peri_divider(uint8_t divider);

/** Divides the clock driving the CM0 from what peri clock is
 *  running at
 *
 * @param[in]  divider The amount to divide the peri clock by
 * @return The status of the cm0_divider request
 */
cy_rslt_t cyhal_system_cm0_divider(uint8_t divider);

/** \} group_hal_system_functions */

#if defined(__cplusplus)
}
#endif

/** \} group_hal_system */
