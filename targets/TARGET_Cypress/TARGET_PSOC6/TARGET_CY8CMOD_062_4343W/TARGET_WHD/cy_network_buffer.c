/*******************************************************************************
* File Name: cy_network_buffer.c
*
* Description:
* Basic set of APIs for dealing with network packet buffers. This is used by WHD
* for relaying data between the network stack and the connectivity chip.
* 
********************************************************************************
* Copyright (c) 2018-2019 Cypress Semiconductor.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include <stdlib.h>
#include "cy_network_buffer.h"
#include "cy_utils.h"

typedef struct
{
    uint16_t full_size;
    uint16_t size;
    uint16_t offset;
    uint8_t *ptr;
} whd_buffer_impl_t;

cy_rslt_t cy_host_buffer_get(whd_buffer_t *buffer, whd_buffer_dir_t direction, uint16_t size, uint32_t timeout_ms)
{
    void *p = malloc(sizeof(whd_buffer_impl_t) + size);
    if (!p)
    {
        return CY_RSLT_BUFFER_UNAVAILABLE;
    }
    whd_buffer_impl_t *myBuffer = (whd_buffer_impl_t*)p;
    *buffer = p;

    myBuffer->full_size = size;
    myBuffer->size = size;
    myBuffer->offset = 0;
    myBuffer->ptr = p + sizeof(whd_buffer_impl_t);

    return CY_RSLT_SUCCESS;
}

void cy_buffer_release(whd_buffer_t buffer, whd_buffer_dir_t direction)
{
    CY_ASSERT(buffer != NULL);
    free(buffer);
}

uint8_t *cy_buffer_get_current_piece_data_pointer(whd_buffer_t buffer)
{
    CY_ASSERT(buffer != NULL);
    whd_buffer_impl_t *myBuffer = (whd_buffer_impl_t*)buffer;
    return &(myBuffer->ptr[myBuffer->offset]);
}

uint16_t cy_buffer_get_current_piece_size(whd_buffer_t buffer)
{
    CY_ASSERT(buffer != NULL);
    whd_buffer_impl_t *myBuffer = (whd_buffer_impl_t*)buffer;
    return myBuffer->size;
}

cy_rslt_t cy_buffer_set_size(whd_buffer_t buffer, uint16_t size)
{
    CY_ASSERT(buffer != NULL);
    whd_buffer_impl_t *myBuffer = (whd_buffer_impl_t*)buffer;
    if ((myBuffer->offset + size) <= myBuffer->full_size)
    {
        myBuffer->size = size;
        return CY_RSLT_SUCCESS;
    }
    return CY_RSLT_BUFFER_TOO_SMALL;
}

cy_rslt_t cy_buffer_add_remove_at_front(whd_buffer_t *buffer, int32_t add_remove_amount)
{
    CY_ASSERT(buffer != NULL);
    whd_buffer_impl_t *myBuffer = (whd_buffer_impl_t*)*buffer;
    if ((myBuffer->offset + add_remove_amount) >= 0 && (myBuffer->offset + add_remove_amount) <= myBuffer->full_size)
    {
        myBuffer->offset += add_remove_amount;
        myBuffer->size -= add_remove_amount;
        return CY_RSLT_SUCCESS;
    }
    return CY_RSLT_BUFFER_TOO_SMALL;
}

