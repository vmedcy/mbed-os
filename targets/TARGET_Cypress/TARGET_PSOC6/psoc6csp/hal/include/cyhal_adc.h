/***************************************************************************//**
* \file cyhal_adc.h
*
* \brief
* Provides a high level interface for interacting with the Cypress Analog to
* Digital Converter. This interface abstracts out the chip specific details.
* If any chip specific functionality is necessary, or performance is critical
* the low level functions can be used directly.
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
* \addtogroup group_hal_adc ADC (Analog to Digital Converter)
* \ingroup group_hal
* \{
* High level interface for interacting with the Cypress ADC.
*
* \defgroup group_hal_adc_macros Macros
* \defgroup group_hal_adc_functions Functions
* \defgroup group_hal_adc_data_structures Data Structures
* \defgroup group_hal_adc_enums Enumerated Types
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
* \addtogroup group_hal_adc_macros
* \{
*/

/** Maximum value that the ADC can return */
#define CY_ADC_MAX_VALUE 0x0FFF

/** \} group_hal_adc_macros */


/**
* \addtogroup group_hal_adc_functions
* \{
*/

/** Initialize the adc peripheral
 *
 * Configures the pin used by adc.
 * @param[out] obj The adc object to initialize
 * @param[in]  pin The adc pin name
 * @return The status of the init request
 */
cy_rslt_t cyhal_adc_init(cyhal_adc_t *obj, cyhal_gpio_t pin);

/** Uninitialize the adc peripheral and cyhal_adc_t object
 *
 * @param[in,out] obj The adc object
 * @return The status of the free request
 */
cy_rslt_t cyhal_adc_free(cyhal_adc_t *obj);

/** Read the value from adc pin, represented as an unsigned 16bit value
 *
 * @param[in] obj The adc object
 * @param[out] value An unsigned 16bit value representing the current input voltage
 * @return The status of the read request
 */
cy_rslt_t cyhal_adc_read_u16(const cyhal_adc_t *obj, uint16_t *value);

/** \} group_hal_adc_functions */

#if defined(__cplusplus)
}
#endif

/** \} group_hal_adc */
