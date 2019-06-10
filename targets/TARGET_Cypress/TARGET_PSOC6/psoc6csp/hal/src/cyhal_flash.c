/***************************************************************************//**
* \file cyhal_flash.c
*
* Description:
* Provides a high level interface for interacting with the Cypress Flash. This
* is wrapper around the lower level PDL API.
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

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "cyhal_hwmgr.h"
#include "cyhal_hw_types.h"
#include "cyhal_flash.h"
#include "cyhal_flash.h"
#include <string.h>

cy_rslt_t cyhal_flash_init(cyhal_flash_t *obj)
{
    CY_ASSERT(NULL != obj);
    obj->resource.type = CYHAL_RSC_FLASH;
    obj->resource.block_num = 0;
    obj->resource.channel_num = 0;

    return cyhal_hwmgr_reserve(&(obj->resource));
}

void cyhal_flash_free(cyhal_flash_t *obj)
{
    CY_ASSERT(NULL != obj);

    cyhal_resource_inst_t *flashRsc=&(obj->resource);
    cyhal_hwmgr_free(flashRsc);
    flashRsc->type = CYHAL_RSC_INVALID;
}

void cyhal_flash_get_info(const cyhal_flash_t *obj, cyhal_flash_info_t *info)
{
    CY_ASSERT(NULL != obj);
    info->start_address = CY_FLASH_BASE;
    info->size = CY_FLASH_SIZE;
    info->sector_size = CY_FLASH_SIZEOF_ROW;
    info->page_size = CY_FLASH_SIZEOF_ROW;
    info->erase_value = 0x00U;
}

cy_rslt_t cyhal_flash_read(cyhal_flash_t *obj, uint32_t address, uint8_t *data, size_t size)
{
    CY_ASSERT(NULL != obj);
    if (address < CY_FLASH_BASE || (address + size) > (CY_FLASH_BASE + CY_FLASH_SIZE))
    {
        return CYHAL_FLASH_RSLT_ERR_ADDRESS;
    }
    memmove(data, (void *)address, size);
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_flash_erase(cyhal_flash_t *obj, uint32_t address)
{
    CY_ASSERT(NULL != obj);
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;

    if ((address >= CY_FLASH_BASE) && (address < (CY_FLASH_BASE + CY_FLASH_SIZE)))
    {
        status = Cy_Flash_EraseRow(address);
        Cy_SysLib_ClearFlashCacheAndBuffer();
    }

    return (status);
}

cy_rslt_t cyhal_flash_write(cyhal_flash_t *obj, uint32_t address, const uint32_t* data)
{
    CY_ASSERT(NULL != obj);
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;

    if (((uint32_t)data) > CY_FLASH_BASE)
    {
        static uint8_t writeBuf[CY_FLASH_SIZEOF_ROW];
        memcpy(writeBuf, (const void *)data, CY_FLASH_SIZEOF_ROW);
        status = Cy_Flash_WriteRow(address, (const uint32_t *) writeBuf);
    }
    else
    {
        status = Cy_Flash_WriteRow(address, data);
    }

    return (status);
}

cy_rslt_t cyhal_flash_program(cyhal_flash_t *obj, uint32_t address, const uint32_t *data)
{
    CY_ASSERT(NULL != obj);
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;

    if (((uint32_t)data) > CY_FLASH_BASE)
    {
        static uint8_t writeBuf[CY_FLASH_SIZEOF_ROW];
        memcpy(writeBuf, (const void *)data, CY_FLASH_SIZEOF_ROW);
        status = Cy_Flash_ProgramRow(address, (const uint32_t *) writeBuf);
    }
    else
    {
        status = Cy_Flash_ProgramRow(address, data);
    }

    Cy_SysLib_ClearFlashCacheAndBuffer();

    return (status);
}

cy_rslt_t cyhal_flash_start_erase(cyhal_flash_t *obj, uint32_t address)
{
    CY_ASSERT(NULL != obj);
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;

    if ((address >= CY_FLASH_BASE) && (address < (CY_FLASH_BASE + CY_FLASH_SIZE)))
    {
        status = Cy_Flash_StartEraseRow(address);
    }

    return (status);
}

cy_rslt_t cyhal_flash_start_write(cyhal_flash_t *obj, uint32_t address, const uint32_t* data)
{
    CY_ASSERT(NULL != obj);
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;

    uint32_t srcAddrStart = (uint32_t)data;
    uint32_t srcAddrEnd = srcAddrStart + CY_FLASH_SIZEOF_ROW;
    if (srcAddrEnd > CY_FLASH_BASE && srcAddrStart < (CY_FLASH_BASE + CY_FLASH_SIZE))
    {
        static uint8_t writeBuf[CY_FLASH_SIZEOF_ROW];
        memcpy(writeBuf, (const void *)data, CY_FLASH_SIZEOF_ROW);
        status = Cy_Flash_StartWrite(address, (const uint32_t *) writeBuf);
    }
    else
    {
      status = Cy_Flash_StartWrite(address, data);
    }

    return (status);
}

cy_rslt_t cyhal_flash_start_program(cyhal_flash_t *obj, uint32_t address, const uint32_t* data)
{
    CY_ASSERT(NULL != obj);
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;

    uint32_t srcAddrStart = (uint32_t)data;
    uint32_t srcAddrEnd = srcAddrStart + CY_FLASH_SIZEOF_ROW;
    if (srcAddrEnd > CY_FLASH_BASE && srcAddrStart < (CY_FLASH_BASE + CY_FLASH_SIZE))
    {
        static uint8_t writeBuf[CY_FLASH_SIZEOF_ROW];
        memcpy(writeBuf, (const void *)data, CY_FLASH_SIZEOF_ROW);
        status = Cy_Flash_StartProgram(address, (const uint32_t *) writeBuf);
    }
    else
    {
      status = Cy_Flash_StartProgram(address, data);
    }

    return (status);
}

bool cyhal_flash_is_operation_complete(cyhal_flash_t *obj)
{
    CY_ASSERT(NULL != obj);
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;

    status = Cy_Flash_IsOperationComplete();

    bool complete;
    if (CY_FLASH_DRV_SUCCESS == status)
    {
        Cy_SysLib_ClearFlashCacheAndBuffer();
        complete = true;
    }
    else
    {
        complete = false;
    }

    return complete;
}

#if defined(__cplusplus)
}
#endif /* __cplusplus */
