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

/** @file
 *  Abstract type of whd_driver structure
 */

#ifndef INCLUDED_WHD_H
#define INCLUDED_WHD_H

#include "whd_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct whd_driver *whd_driver_t;
typedef struct whd_interface *whd_interface_t;
typedef struct whd_buffer_funcs whd_buffer_funcs_t;
typedef struct whd_netif_funcs whd_netif_funcs_t;
typedef struct whd_resource_source whd_resource_source_t;

typedef struct whd_spi_config
{
    /* Bus config */
    whd_bool_t is_spi_normal_mode; /* Default is false */
    whd_bool_t is_spi_irq_falling_edge; /* Default is false */
} whd_spi_config_t;

typedef struct whd_init_config
{
    void *thread_stack_start; /* Pointer to the WHD thread stack  */
    uint32_t thread_stack_size; /* size of the    WHD thread stack  */
    uint32_t thread_priority; /* Priority to be set to WHD Thread */
    whd_country_code_t country;
    uint8_t oob_gpio_pin;
} whd_init_config_t;

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* INCLUDED_WHD_H */
