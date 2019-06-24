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
#include "cyhal_implementation.h"
#include "cycfg.h"

#if defined(__cplusplus)
extern "C" {
#endif

cy_rslt_t cybsp_init(void)
{
	init_cycfg_system();

    cy_rslt_t result = CY_RSLT_SUCCESS;
    /* Initialize User LEDs */
    result |= cybsp_led_init(CYBSP_LED_RGB_RED);
    result |= cybsp_led_init(CYBSP_LED_RGB_BLUE);
    result |= cybsp_led_init(CYBSP_LED_RGB_GREEN);
    result |= cybsp_led_init(CYBSP_USER_LED0);
    result |= cybsp_led_init(CYBSP_USER_LED1);
    /* Initialize User Buttons */
    result |= cybsp_btn_init(CYBSP_USER_BTN0);

    CY_ASSERT(CY_RSLT_SUCCESS == result);

#if defined(CYBSP_RETARGET_ENABLED)
    /* Initialize retargetting stdio to 'DEBUG_UART' peripheral */
    if (CY_RSLT_SUCCESS == result)
    {
        result = cybsp_retarget_init();
    }
#endif

    return result;
}

#if defined(__cplusplus)
}
#endif
