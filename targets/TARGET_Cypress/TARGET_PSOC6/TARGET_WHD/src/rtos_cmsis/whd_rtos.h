/*
 * Copyright 2019 Cypress Semiconductor Corporation
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

#include "whd_types.h"
#include "whd_debug.h"
#include "whd_rtos_interface.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
*                      Macros
******************************************************/


/******************************************************
*                    Constants
******************************************************/
#ifndef SYSTICK_FREQUENCY
#define SYSTICK_FREQUENCY  (1000)
#endif

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/


typedef osSemaphoreId_t whd_semaphore_type_t;                 /** CMSIS definition of a semaphore */
typedef osMutexId_t whd_mutex_type_t;                         /** CMSIS definition of a mutex */
typedef osThreadId_t whd_thread_type_t;                       /** CMSIS definition of a thread handle */
typedef osMessageQueueId_t whd_queue_type_t;                  /** CMSIS definition of a message queue */


/******************************************************
*                    Structures
******************************************************/


/******************************************************
*                 Global Variables
******************************************************/

/******************************************************
*               Function Declarations
******************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif

