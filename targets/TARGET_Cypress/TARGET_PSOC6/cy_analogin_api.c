/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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

#include "analogin_api.h"
#include "mbed_error.h"
#include "cyhal_adc.h"

#if DEVICE_ANALOGIN

#ifdef __cplusplus
extern "C" {
#endif

void analogin_init(analogin_t *obj, PinName pin)
{
    if (CY_RSLT_SUCCESS != cyhal_adc_init(&(obj->hal_adc), pin))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_ANALOG, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_adc_init");
}

float analogin_read(analogin_t *obj)
{
    return analogin_read_u16(obj) * (1.0f / (1.0f + CY_ADC_MAX_VALUE));
}

uint16_t analogin_read_u16(analogin_t *obj)
{
    uint16_t value;
    if (CY_RSLT_SUCCESS != cyhal_adc_read_u16(&(obj->hal_adc), &value))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_ANALOG, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_adc_read");
    return value;
}

const PinMap *analogin_pinmap(void)
{
    return PinMap_ADC;
}

#ifdef __cplusplus
}
#endif

#endif
