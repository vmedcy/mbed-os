/* mbed Microcontroller Library
 * Copyright (c) 2017 ARM Limited
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
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "whd_buffer.h"
#include "whd_buffer_api.h"
#include "mbed_critical.h"
#include "pbuf.h"

whd_result_t host_buffer_check_leaked(void)
{
    /* Nothing to do */
    return WHD_SUCCESS;
}

whd_result_t internal_host_buffer_get(whd_buffer_t* buffer, whd_buffer_dir_t direction, unsigned short size, unsigned long timeout_ms)
{
    assert(core_util_are_interrupts_enabled());
    whd_buffer_impl_t* pkt = NULL;
    struct pbuf *pbuffer = NULL;
    uint16_t bufsize = sizeof(whd_buffer_impl_t) + size;

    if (direction == WHD_NETWORK_RX )
    {
      pbuffer = pbuf_alloc(PBUF_RAW, bufsize, PBUF_RAM);
      if (pbuffer == NULL) {
          return WHD_BUFFER_UNAVAILABLE_PERMANENT;;
      }
      pkt = (whd_mbed_buffer_t)pbuffer->payload;
      *buffer = (whd_mbed_buffer_t)pbuffer->payload;
      pkt->parent = pbuffer;
    }
    else
    {
    	 void *p = malloc(sizeof(whd_buffer_impl_t) + size);
    	 if (!p) {
    	        return WHD_BUFFER_UNAVAILABLE_PERMANENT;
    	 }
    	 pkt = (whd_buffer_impl_t *)p;
    	 *buffer = p;

    }
    whd_mbed_buffer_t* mpbuffer = (whd_mbed_buffer_t*)buffer;
    (*mpbuffer)->ptr = &pkt[1];

    (*mpbuffer)->size = size;
    (*mpbuffer)->offset = 0;
    return WHD_SUCCESS;
}

whd_result_t host_buffer_get(/*@out@*/ whd_buffer_t * buffer, whd_buffer_dir_t direction, unsigned short size, whd_bool_t wait)
{
    return internal_host_buffer_get(buffer, direction, size, wait);
}

void host_buffer_release(whd_buffer_t buffer, whd_buffer_dir_t direction )
{
    assert(buffer != NULL);
    assert(core_util_are_interrupts_enabled());
    if ( direction != WHD_NETWORK_RX )
    {
    	free(buffer);
    }
    else
    {
        whd_mbed_buffer_t mbuffer = (whd_mbed_buffer_t)buffer;
    	pbuf_free(mbuffer->parent);
    }
}

uint8_t* host_buffer_get_current_piece_data_pointer(whd_buffer_t buffer )
{
    whd_mbed_buffer_t mbuffer = (whd_mbed_buffer_t)buffer;
    return &mbuffer->ptr[mbuffer->offset];
}

uint16_t host_buffer_get_current_piece_size(whd_buffer_t buffer)
{
    whd_mbed_buffer_t mbuffer = (whd_mbed_buffer_t)buffer;
    return mbuffer->size;
}

whd_buffer_t host_buffer_get_next_piece(whd_buffer_t buffer)
{
    UNUSED_PARAMETER(buffer);
    return NULL;
}

whd_result_t host_buffer_add_remove_at_front(whd_buffer_t * buffer, int32_t add_remove_amount)
{
    whd_mbed_buffer_t* mpbuffer = (whd_mbed_buffer_t*)buffer;
    assert((*mpbuffer)->offset >= -add_remove_amount);
    (*mpbuffer)->offset += add_remove_amount;
    (*mpbuffer)->size -= add_remove_amount;
    return WHD_SUCCESS;
}

whd_result_t host_buffer_set_size(whd_buffer_t buffer, unsigned short size)
{
    whd_mbed_buffer_t mbuffer = (whd_mbed_buffer_t)buffer;
    mbuffer->size = size;
    return WHD_SUCCESS;
}
