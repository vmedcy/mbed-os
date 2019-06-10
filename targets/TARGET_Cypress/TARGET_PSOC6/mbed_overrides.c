/*
 * mbed Microcontroller Library
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

#include "device.h"
#include "cycfg.h"
#include "cyhal_hwmgr.h"
#include "mbed_power_mgmt.h"
#include "rtos_idle.h"

#if defined(COMPONENT_SPM_MAILBOX)
void mailbox_init(void);
#endif


#if (defined(CY_CFG_PWR_SYS_IDLE_MODE) && (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_ACTIVE))
/*******************************************************************************
* Function Name: active_idle_hook
****************************************************************************//**
*
* Empty idle hook function to prevent the system entering sleep mode
* automatically any time the system is idle.
*
*******************************************************************************/
static void active_idle_hook(void)
{
    /* Do nothing, so the rtos_idle_loop() performs while(1) */
}
#endif

/*******************************************************************************
* Function Name: mbed_sdk_init
****************************************************************************//**
*
* Mbed's post-memory-initialization function.
* Used here to initialize common parts of the Cypress libraries.
*
*******************************************************************************/
void mbed_sdk_init(void)
{
    /* Initialize system and clocks. */
    /* Placed here as it must be done after proper LIBC initialization. */
    SystemInit();

    /* Initialize hardware resource manager */
    cyhal_hwmgr_init();

#if defined(COMPONENT_SPM_MAILBOX)
    mailbox_init();
#endif

#if (!CY_CPU_CORTEX_M0P)
    /* Set up the device based on configurator selections */
    init_cycfg_all();

    /* Enable global interrupts (disabled in CM4 startup assembly) */
    __enable_irq();
#endif

#if defined (CY_CFG_PWR_SYS_IDLE_MODE)
    /* Configure the lowest power state the system is allowed to enter
    * based on the System Idle Power Mode parameter value in the Device
    * Configurator. The default value is system deep sleep.
    */
#if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_ACTIVE)
    rtos_attach_idle_hook(&active_idle_hook);
#elif (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_SLEEP)
    sleep_manager_lock_deep_sleep();
#else
    /* Deep sleep is default state */
#endif
#endif
}

