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

#if defined(TARGET_WHD)

#include <stdlib.h>
#include "cy_network_buffer.h"
#include "cy_utils.h"
#include "memp.h"

cy_rslt_t cy_host_buffer_get(whd_buffer_t *buffer, whd_buffer_dir_t direction, uint16_t size, uint32_t timeout_ms)
{
    UNUSED_PARAMETER( direction );
    struct pbuf *p = NULL;
    if ( direction == WHD_NETWORK_TX)
    {
    	p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
    }
    else
    {
    	p = pbuf_alloc(PBUF_RAW, size, PBUF_RAM);
    }
    if (p != NULL )
    {
        *buffer = p;
    	return CY_RSLT_SUCCESS;
    }
    else
    {
        return WHD_BUFFER_ALLOC_FAIL;
    }
}

void cy_buffer_release(whd_buffer_t buffer, whd_buffer_dir_t direction)
{
    UNUSED_PARAMETER( direction );
    (void) pbuf_free( (struct pbuf *)buffer );
}

uint8_t *cy_buffer_get_current_piece_data_pointer(whd_buffer_t buffer)
{
    CY_ASSERT(buffer != NULL);
    struct pbuf *pbuffer= (struct pbuf*) buffer;
    return (uint8_t*) pbuffer->payload;
}

uint16_t cy_buffer_get_current_piece_size(whd_buffer_t buffer)
{
    CY_ASSERT(buffer != NULL);
    struct pbuf *pbuffer = (struct pbuf*) buffer;
    return (uint16_t) pbuffer->len;
}

cy_rslt_t cy_buffer_set_size(whd_buffer_t buffer, uint16_t size)
{
    CY_ASSERT(buffer != NULL);
    struct pbuf * pbuffer = (struct pbuf *) buffer;

    if ( size > (unsigned short) WHD_LINK_MTU  + LWIP_MEM_ALIGN_SIZE(LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf))) + LWIP_MEM_ALIGN_SIZE(size) )
    {
        return WHD_PMK_WRONG_LENGTH;
    }

    pbuffer->tot_len = size;
    pbuffer->len = size;

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_buffer_add_remove_at_front(whd_buffer_t *buffer, int32_t add_remove_amount)
{
    CY_ASSERT(buffer != NULL);
    struct pbuf **pbuffer = (struct pbuf**) buffer;

    if ( (u8_t) 0 != pbuf_header( *pbuffer, ( s16_t )( -add_remove_amount ) ) )
    {
        return WHD_PMK_WRONG_LENGTH;
    }

    return CY_RSLT_SUCCESS;
}

#endif /* defined(TARGET_WHD) */
