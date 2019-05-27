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
    obj->resource.type = CYHAL_RSC_FLASH;
    obj->resource.block_num = 0;
    obj->resource.channel_num = 0;

    obj->flashBaseAddr = CY_FLASH_BASE;
    obj->flashSize = CY_FLASH_SIZE;
    obj->sectorSize = CY_FLASH_SIZEOF_ROW;
    obj->pageSize = CY_FLASH_SIZEOF_ROW;
    obj->eraseVal = 0x0U;
    
    return cyhal_hwmgr_reserve(&(obj->resource));
}

cy_rslt_t cyhal_flash_free(cyhal_flash_t *obj)
{
    cyhal_resource_inst_t *flashRsc=&(obj->resource);
    
    obj->flashBaseAddr = 0UL;
    obj->flashSize = 0UL;
    obj->sectorSize = 0UL;
    obj->pageSize = 0UL;
    obj->eraseVal = 0U;    

    cy_rslt_t status = cyhal_hwmgr_free(flashRsc);
    flashRsc->type = CYHAL_RSC_INVALID;

    return (status);
}

cy_rslt_t cyhal_flash_get_info(const cyhal_flash_t *obj, cyhal_flash_info_t *info)
{
    info->start_address = obj->flashBaseAddr;
    info->size = obj->flashSize;
    info->sector_size = obj->sectorSize;
    info->page_size = obj->pageSize;
    info->erase_value = obj->eraseVal;
    
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_flash_read(cyhal_flash_t *obj, uint32_t address, uint8_t *data, size_t size)
{
    if (address < obj->flashBaseAddr || (address + size) > (obj->flashBaseAddr + obj->flashSize))
    {
        return CYHAL_FLASH_RSLT_ERR_ADDRESS;
    }
    memmove(data, (void *)address, size);
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_flash_erase(cyhal_flash_t *obj, uint32_t address)
{
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;
    
    if ((address >= obj->flashBaseAddr) && (address <= (obj->flashBaseAddr + obj->flashSize)))
    {
        status = Cy_Flash_EraseRow(address);
        Cy_SysLib_ClearFlashCacheAndBuffer();
    }
    
    return (status);
}

cy_rslt_t cyhal_flash_write(cyhal_flash_t *obj, uint32_t address, const uint32_t* data)
{
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;
    
    if (((uint32_t)data) > obj->flashBaseAddr)
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
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;
    
    if (((uint32_t)data) > obj->flashBaseAddr)
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
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;
    
    if ((address >= obj->flashBaseAddr) && (address <= (obj->flashBaseAddr + obj->flashSize)))
    {
        status = Cy_Flash_StartEraseRow(address);
    }
    
    return (status);
}


cy_rslt_t cyhal_flash_start_write(cyhal_flash_t *obj, uint32_t address, const uint32_t* data)
{
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;

    uint32_t srcAddrStart = (uint32_t)data;
    uint32_t srcAddrEnd = srcAddrStart + CY_FLASH_SIZEOF_ROW;
    if ((obj->flashBaseAddr <= srcAddrStart && srcAddrStart <= (obj->flashBaseAddr + obj->flashSize)) ||
        (obj->flashBaseAddr <= srcAddrEnd && srcAddrEnd <= (obj->flashBaseAddr + obj->flashSize)))
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
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;
    
    uint32_t srcAddrStart = (uint32_t)data;
    uint32_t srcAddrEnd = srcAddrStart + CY_FLASH_SIZEOF_ROW;
    if ((obj->flashBaseAddr <= srcAddrStart && srcAddrStart <= (obj->flashBaseAddr + obj->flashSize)) ||
        (obj->flashBaseAddr <= srcAddrEnd && srcAddrEnd <= (obj->flashBaseAddr + obj->flashSize)))
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

cy_rslt_t cyhal_flash_is_operation_complete(cyhal_flash_t *obj, bool *complete)
{
    cy_rslt_t status = CYHAL_FLASH_RSLT_ERR_ADDRESS;
    
    status = Cy_Flash_IsOperationComplete();
    
    if (CY_FLASH_DRV_SUCCESS == status)
    {
        Cy_SysLib_ClearFlashCacheAndBuffer();
        *complete = true;
    }
    else
    {
        *complete = false;
    }

    return (status);
}

#if defined(__cplusplus)
}
#endif /* __cplusplus */
