/***************************************************************************//**
* \file cyhal_uart.h
*
* \brief
* Provides a high level interface for interacting with the Cypress UART.
* This interface abstracts out the chip specific details. If any chip specific
* functionality is necessary, or performance is critical the low level functions
* can be used directly.
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

/**
* \addtogroup group_hal_uart UART (Universal Asynchronous Receiver-Transmitter)
* \ingroup group_hal
* \{
* High level interface for interacting with the Cypress UART.
*
* \defgroup group_hal_uart_macros Macros
* \defgroup group_hal_uart_functions Functions
* \defgroup group_hal_uart_data_structures Data Structures
* \defgroup group_hal_uart_enums Enumerated Types
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "cy_result.h"
#include "cyhal_hw_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
* \addtogroup group_hal_uart_macros
* \{
*/

/** The requested resource type is invalid */
#define CYHAL_UART_RSLT_ERR_INVALID_PIN (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_UART, 0))
/** Failed to confiugre power management callback */
#define CYHAL_UART_RSLT_ERR_PM_CALLBACK (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_UART, 1))
/** The getc call timed out with no received data */
#define CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_UART, 2))
/** The baud rate to set to if no clock is specfied in the init function */
#define CYHAL_UART_DEFAULT_BAUD 115200

/** \} group_hal_uart_macros */


/**
* \addtogroup group_hal_uart_enums
* \{
*/

/** UART Parity */
typedef enum
{
    CYHAL_UART_PARITY_NONE,   /**< UART has no parity check   */
    CYHAL_UART_PARITY_EVEN,   /**< UART has even parity check */
    CYHAL_UART_PARITY_ODD,    /**< UART has odd parity check  */
} cyhal_uart_parity_t;

/** Enum to enable/disable/report interrupt cause flags. */
typedef enum
{
    CYHAL_UART_IRQ_NONE                = 0,      //!< Disable all interrupt call backs
    CYHAL_UART_IRQ_TX_TRANSMIT_IN_FIFO = 1 << 1, //!< All tx data from transmit has been moved to uart FIFO
    CYHAL_UART_IRQ_TX_DONE             = 1 << 2, //!< All tx data has been transmitted
    CYHAL_UART_IRQ_TX_ERROR            = 1 << 3, //!< An error occurred in tx
    CYHAL_UART_IRQ_RX_FULL             = 1 << 4, //!< The rx software buffer is full, additional data are store into fifo buffer.
    CYHAL_UART_IRQ_RX_DONE             = 1 << 5, //!< All rx data has been received
    CYHAL_UART_IRQ_RX_ERROR            = 1 << 6, //!< An error occurred in rx
} cyhal_uart_irq_event_t;

/** \} group_hal_uart_enums */


/**
* \addtogroup group_hal_uart_data_structures
* \{
*/

/** Initial UART configuration */
typedef struct
{
    uint32_t data_bits; //!< The number of start bits
    uint32_t stop_bits; //!< The number of stop bits
    cyhal_uart_parity_t parity; //!< The parity
    uint8_t *rx_buffer; //!< The rx software buffer pointer, if NULL, no rx software buffer will be used
    uint32_t rx_buffer_size; //!< The number of bytes in the rx software buffer
} cyhal_uart_cfg_t;

/** UART callback function type */
typedef void (*cyhal_uart_irq_handler_t)(void *handler_arg, cyhal_uart_irq_event_t event);

/** \} group_hal_uart_data_structures */


/**
* \addtogroup group_hal_uart_functions
* \{
*/

/** Initialize the uart peripheral. It sets the default parameters for uart
 *  peripheral, and configures its specifieds pins.
 *
 * @param[out] obj The uart object
 * @param[in]  tx  The TX pin name
 * @param[in]  rx  The RX pin name
 * @param[in]  clk The clock to use can be shared, if not provided a new clock will be,
 *                  allocated and the default baud rate set
 * @param[in]  cfg The uart configuration data for data bits, stop bits and parity,
 *                  if not provided, default values of (8, 1, none) will be used
 * @return The status of the init request
 */
cy_rslt_t cyhal_uart_init(cyhal_uart_t *obj, cyhal_gpio_t tx, cyhal_gpio_t rx, const cyhal_clock_divider_t *clk, const cyhal_uart_cfg_t *cfg);

/** Release the uart peripheral, not currently invoked. It requires further
 *  resource management.
 *
 * @param[in,out] obj The uart object
 */
void cyhal_uart_free(cyhal_uart_t *obj);

/** Configure the baud rate
 *
 * @param[in,out] obj      The uart object
 * @param[in]     baudrate The baud rate to be configured
 * @return The status of the baud request
 */
cy_rslt_t cyhal_uart_baud(cyhal_uart_t *obj, uint32_t baudrate);

/** Get character. This is a blocking call, waiting for a character
 *
 * @param[in] obj    The uart object
 * @param[out] value The value read from the serial port
 * @param[in] timeout The time in ms to spend attempting to receive from serial port
 *                    timeout = zero is wait forever
 * @return The status of the getc request
 */
cy_rslt_t cyhal_uart_getc(cyhal_uart_t *obj, uint8_t *value, uint32_t timeout);

/** Send a character. This is a blocking call, waiting for a peripheral to be available
 *  for writing
 *
 * @param[in] obj The uart object
 * @param[in] value The character to be sent
 * @return The status of the putc request
 */
cy_rslt_t cyhal_uart_putc(cyhal_uart_t *obj, uint32_t value);

/** Check the number of bytes avaialable to read from the receive buffers
 *
 * @param[in]  obj      The uart object
 * @return The number of readable bytes
 */
uint32_t cyhal_uart_readable(cyhal_uart_t *obj);

/** Check the number of bytes than can be written to the transmit buffer
 *
 * @param[in]  obj      The uart object
 * @return The number of bytes that can be written 
 */
uint32_t cyhal_uart_writable(cyhal_uart_t *obj);

/** Clear the uart peripheral buffers
 *
 * @param[in] obj The uart object
 * @return The status of the clear request
 */
cy_rslt_t cyhal_uart_clear(cyhal_uart_t *obj);

/** Configure the uart for the flow control. It sets flow control in the hardware
 *  if a uart peripheral supports it, otherwise software emulation is used.
 *
 * @param[in,out] obj    The uart object
 * @param[in]     cts    The TX pin name
 * @param[in]     rts    The RX pin name
 * @return The status of the init_flow_control request
 */
cy_rslt_t cyhal_uart_set_flow_control(cyhal_uart_t *obj, cyhal_gpio_t cts, cyhal_gpio_t rts);

/** Begin synchronous TX transfer. The used buffer is specified in the uart object,
 *  tx_buff
 *
 * @param[in]     obj        The uart object
 * @param[in]     tx         The transmit buffer
 * @param[in,out] tx_length  [in] The number of bytes to transmit, [out] number actually transmitted
 * @return The status of the tx request
 */
cy_rslt_t cyhal_uart_tx(cyhal_uart_t *obj, void *tx, size_t *tx_length);

/** Begin synchronous RX transfer (enable interrupt for data collecting)
 *  The used buffer is specified in the uart object - rx_buff
 *
 * @param[in]     obj       The uart object
 * @param[in]     rx        The receive buffer
 * @param[in,out] rx_length [in] The number of bytes to receive, [out] number actually received
 * @return The status of the rx request
 */
cy_rslt_t cyhal_uart_rx(cyhal_uart_t *obj, void *rx, size_t *rx_length);

/** Begin asynchronous TX transfer. The used buffer is specified in the uart object,
 *  tx_buff
 *
 * @param[in] obj     The uart object
 * @param[in] tx      The transmit buffer
 * @param[in] length  The number of bytes to transmit
 * @return The status of the tx_asynch request
 */
cy_rslt_t cyhal_uart_tx_asynch(cyhal_uart_t *obj, void *tx, size_t length);

/** Begin asynchronous RX transfer (enable interrupt for data collecting)
 *  The used buffer is specified in the uart object - rx_buff
 *
 * @param[in]  obj     The uart object
 * @param[out] rx      The receive buffer
 * @param[in]  length  The number of bytes to receive
 * @return The status of the rx_asynch request
 */
cy_rslt_t cyhal_uart_rx_asynch(cyhal_uart_t *obj, void *rx, size_t length);

/** Attempts to determine if the uart peripheral is already in use for TX
 *
 * @param[in]  obj    The uart object
 * @return Is the TX channel active
 */
bool cyhal_uart_is_tx_active(cyhal_uart_t *obj);

/** Attempts to determine if the uart peripheral is already in use for RX
 *
 * @param[in]  obj    The uart object
 * @return Is the RX channel active
 */
bool cyhal_uart_is_rx_active(cyhal_uart_t *obj);

/** Abort the ongoing TX transaction. It disables the enabled interupt for TX and
 *  flushes the TX hardware buffer if TX FIFO is used
 *
 * @param[in] obj The uart object
 * @return The status of the tx_abort request
 */
cy_rslt_t cyhal_uart_tx_abort(cyhal_uart_t *obj);

/** Abort the ongoing RX transaction. It disables the enabled interrupt for RX and
 *  flushes the RX hardware buffer if RX FIFO is used
 *
 * @param[in] obj The uart object
 * @return The status of the rx_abort request
 */
cy_rslt_t cyhal_uart_rx_abort(cyhal_uart_t *obj);

/** The uart interrupt handler registration
 *
 * @param[in] obj         The uart object
 * @param[in] handler     The callback handler which will be invoked when the interrupt fires
 * @param[in] handler_arg Generic argument that will be provided to the handler when called
 */
void cyhal_uart_register_irq(cyhal_uart_t *obj, cyhal_uart_irq_handler_t handler, void *handler_arg);

/** Configure uart interrupt. This function is used for word-approach
 *
 * @param[in] obj      The uart object
 * @param[in] event    The uart IRQ type, this argument supports the bitwise-or of multiple enum flag values
 * @param[in] enable   True to turn on interrupts, False to turn off
 */
void cyhal_uart_irq_enable(cyhal_uart_t *obj, cyhal_uart_irq_event_t event, bool enable);

/** \} group_hal_uart_functions */

#if defined(__cplusplus)
}
#endif

/** \} group_hal_uart */