/*******************************************************************************
* File Name: cy_network_buffer.h
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

/**
* \addtogroup group_abstraction_buffer Buffer management abstraction
* \ingroup group_abstraction
* \{
* TODO: Buffer abstraction high-level description
* to be set in abstraction/include/cy_network_buffer.h
*
* \defgroup group_abstraction_buffer_macros Macros
* \defgroup group_abstraction_buffer_functions Functions
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "cy_result.h"
#include "whd.h"
#include "whd_network_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
* \addtogroup group_abstraction_buffer_macros
* \{
*/

/** Error indicating that memory for the specified buffer could not be allocated. */
#define CY_RSLT_BUFFER_UNAVAILABLE (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_ABSTRACTION_BUFFER, 0))
/** Error indicating that the provided buffer is too small to accomodate the request. */
#define CY_RSLT_BUFFER_TOO_SMALL (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_ABSTRACTION_BUFFER, 1))

/** \} group_abstraction_buffer_macros */


/**
* \addtogroup group_abstraction_buffer_structures
* \{
*/

/** \} group_abstraction_buffer_structures */


/**
* \addtogroup group_abstraction_buffer_functions
* \{
*/

/** Allocates a packet buffer
 *
 *  Attempts to allocate a packet buffer of the size requested. It can do this
 *  by allocating a pre-existing packet from a pool, using a static buffer,
 *  or by dynamically allocating memory. The method of allocation does not
 *  concern WHD, however it must match the way the network stack expects packet
 *  buffers to be allocated. Usually WHD requires packet of size of WHD_LINK_MTU
 *  which includes the MTU. Refer to whd_types.h to find the size of WHD_LINK_MTU.
 *
 *  @param buffer    : A pointer which receives the allocated packet buffer handle
 *  @param direction : Indicates transmit/receive direction that the packet buffer is
 *                    used for. This may be needed if tx/rx pools are separate.
 *  @param size      : The number of bytes to allocate.
 *  @param timeout_ms: Time to wait for a packet buffer to be available
 *
 *  @return          : CY_RSLT_SUCCESS or error code
 */
cy_rslt_t cy_host_buffer_get(whd_buffer_t *buffer, whd_buffer_dir_t direction, uint16_t size, uint32_t timeout_ms);

/** Releases a packet buffer
 *
 *  This function is used by WHD to indicate that it no longer requires
 *  a packet buffer. The buffer can then be released back into a pool for
 *  reuse, or the dynamically allocated memory can be freed, according to
 *  how the packet was allocated.
 *  Returns void since WHD cannot do anything about failures
 *
 *  @param buffer    : The handle of the packet buffer to be released
 *  @param direction : Indicates transmit/receive direction that the packet buffer has
 *                     been used for. This might be needed if tx/rx pools are separate.
 */
void cy_buffer_release(whd_buffer_t buffer, whd_buffer_dir_t direction);

/** Retrieves the current pointer of a packet buffer
 *
 *  Since packet buffers usually need to be created with space at the
 *  front for additional headers, this function allows WHD to get
 *  the current 'front' location pointer.
 *
 *  @param buffer : The handle of the packet buffer whose pointer is to be retrieved
 *
 *  @return       : The packet buffer's current pointer.
 */
uint8_t *cy_buffer_get_current_piece_data_pointer(whd_buffer_t buffer);

/** Retrieves the size of a packet buffer
 *
 *  Since packet buffers usually need to be created with space at the
 *  front for additional headers, the memory block used to contain a packet buffer
 *  will often be larger than the current size of the packet buffer data.
 *  This function allows WHD to retrieve the current size of a packet buffer's data.
 *
 *  @param buffer : The handle of the packet buffer whose size is to be retrieved
 *
 *  @return       :  The size of the packet buffer.
 */
uint16_t cy_buffer_get_current_piece_size(whd_buffer_t buffer);

/** Sets the current size of a WHD packet
 *
 *  This function sets the current length of a WHD packet buffer
 *
 *  @param buffer : The packet to be modified
 *  @param size   : The new size of the packet buffer
 *
 *  @return       : CY_RSLT_SUCCESS or error code
 */
cy_rslt_t cy_buffer_set_size(whd_buffer_t buffer, uint16_t size);

/** Moves the current pointer of a packet buffer
 *
 *  Since packet buffers usually need to be created with space at the front for additional headers,
 *  this function allows WHD to move the current 'front' location pointer so that it has space to
 *  add headers to transmit packets, and so that the network stack does not see the internal WHD
 *  headers on received packets.
 *
 *  @param buffer            : A pointer to the handle of the current packet buffer for which the
 *                             current pointer will be moved. On return this may contain a pointer
 *                             to a newly allocated packet buffer which has been daisy chained to
 *                             the front of the given one. This would be the case if the given packet
 *                             buffer  didn't have enough space at the front.
 *  @param add_remove_amount : This is the number of bytes to move the current pointer of the packet
 *                             buffer - a negative value increases the space for headers at the front
 *                             of the packet, a positive value decreases the space.
 *
 *  @return                  : CY_RSLT_SUCCESS or error code
 */
cy_rslt_t cy_buffer_add_remove_at_front(whd_buffer_t *buffer, int32_t add_remove_amount);


/** Called by WHD to pass received data to the network stack
 *
 *  Packets received from the Wi-Fi network by WHD are forwarded to by calling function ptr which
 *  must be implemented in the network interface. Ethernet headers
 *  are present at the start of these packet buffers.
 *
 *  This function is called asynchronously in the context of the
 *  WHD thread whenever new data has arrived.
 *  Packet buffers are allocated within WHD, and ownership is transferred
 *  to the network stack. The network stack or application is thus
 *  responsible for releasing the packet buffers.
 *  Most packet buffering systems have a pointer to the 'current point' within
 *  the packet buffer. When this function is called, the pointer points
 *  to the start of the Ethernet header. There is other inconsequential data
 *  before the Ethernet header.
 *
 *  It is preferable that the (whd_network_process_ethernet_data)() function simply puts
 *  the received packet on a queue for processing by another thread. This avoids the
 *  WHD thread being unnecessarily tied up which would delay other packets
 *  being transmitted or received.
 *
 *  @param interface : The interface on which the packet was received.
 *  @param buffer    : Handle of the packet which has just been received. Responsibility for
 *                    releasing this buffer is transferred from WHD at this point.
 *
 */
void cy_network_process_ethernet_data(whd_interface_t interface, whd_buffer_t buffer);

/** \} group_abstraction_buffer_functions */

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** \} group_abstraction_buffer */
