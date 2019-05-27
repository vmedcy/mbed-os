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
#ifndef INCLUDED_WHD_RESOURCE_API_H_
#define INCLUDED_WHD_RESOURCE_API_H_

#include "whd.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        WHD_RESOURCE_WLAN_FIRMWARE, WHD_RESOURCE_WLAN_NVRAM, WHD_RESOURCE_WLAN_CLM,
    } whd_resource_type_t;

    /******************************************************
     *                 Global Variables
     ******************************************************/

    /** @endcond */

    /** @addtogroup platif Platform Interface
     *  @{
     */

    /** @name Resource Functions
     *  Functions that enable WHD to download
     *  firmware, nvram on a particular hardware platform.
     */
    /**@{*/

    /**
     * Interface to a data source that provides external resources to the readio driver
     */
    /**
     * \brief This data structure defines a source for data generally intended to be downloaded to the radio device
     *
     * The data is assumed to be available as a set of blocks that are all the same size with the exception
     * of the last block. The get_block_size function returns this block size. The get_total_blocks function
     * can be used to get the total block count. The data download can loop from block 0 to block N - 1 where
     * N is the value returned from the get_total_blocks function. The get_block call returns a pointer to a block
     * of data. The actual storage for the data block is owned by the data source, so only a pointer
     * to the block is returned. There are two predominate use cases. If the data is stored in the internal
     * flash memory, then get_total_blocks will return 1 and a call to get_block will return a pointer to the data
     * image with the size being the size of the data image.  If the data is stored in an external flash of some
     * type, each block of data can be read from the external flash one at a time.  get_total_blocks will return
     * the physical number of blocks in the data and each call to get_block will read data from the external memory
     * and make it available via an internal buffer.
     */
    struct whd_resource_source
    {
        uint32_t (*whd_resource_size)( whd_driver_t whd_drv, whd_resource_type_t resource, uint32_t* size_out );
        uint32_t (*whd_get_resource_block)( whd_driver_t whd_drv, whd_resource_type_t type, const uint8_t **data, uint32_t *size_out );
        uint32_t (*whd_get_resource_no_of_blocks)( whd_driver_t whd_drv, whd_resource_type_t type, uint32_t* block_count );
        uint32_t (*whd_get_resource_block_size)( whd_driver_t whd_drv, whd_resource_type_t type, uint32_t* size_out );
    };

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* ifndef INCLUDED_WHD_RESOURCE_API_H_ */
