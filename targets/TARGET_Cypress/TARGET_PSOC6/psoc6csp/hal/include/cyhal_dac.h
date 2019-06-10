/***************************************************************************//**
* \file cyhal_dac.h
*
* \brief
* Provides a high level interface for interacting with the Cypress Digital to
* Analog Converter. This interface abstracts out the chip specific details. If
* any chip specific functionality is necessary, or performance is critical the
* low level functions can be used directly.
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
* \addtogroup group_hal_dac DAC (Digital to Analog Converter)
* \ingroup group_hal
* \{
* High level interface for interacting with the Cypress DAC.
*
* \defgroup group_hal_dac_macros Macros
* \defgroup group_hal_dac_functions Functions
* \defgroup group_hal_dac_data_structures Data Structures
* \defgroup group_hal_dac_enums Enumerated Types
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "cy_result.h"
#include "cyhal_hw_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
* \addtogroup group_hal_dac_functions
* \{
*/

/** Initialize the dac peripheral
 *
 * Configures the pin used by dac.
 * @param[in] obj The dac object to initialize
 * @param[in] pin The dac pin name
 * @return The status of the init request
 */
cy_rslt_t cyhal_dac_init(cyhal_dac_t *obj, cyhal_gpio_t pin);

/** Release the dac object
 *
 * @param[in,out] obj The dac object
 */
void cyhal_dac_free(cyhal_dac_t *obj);

/** Read the current voltage value on the pin, as a normalized unsigned 16bit value
 *
 * @param[in]  obj        The analogin object
 * @return The maximum output voltage in terms of the number of milivolts
 */
uint16_t cyhal_dac_get_max(cyhal_dac_t *obj);

/** Set the output voltage, specified as unsigned 16-bit millivolts
 *
 * @param[in] obj        The analogin object
 * @param[in] millivolts The unsigned 16-bit output voltage to be set
 */
void cyhal_dac_write(const cyhal_dac_t *obj, uint16_t millivolts);

/** Read the current voltage value on the pin, as a normalized unsigned 16bit value
 *
 * @param[in]  obj        The analogin object
 * @return The output voltage in terms of the number of milivolts
 */
uint16_t cyhal_dac_read(cyhal_dac_t *obj);

/** \} group_hal_dac_functions */

#if defined(__cplusplus)
}
#endif

/** \} group_hal_dac */
