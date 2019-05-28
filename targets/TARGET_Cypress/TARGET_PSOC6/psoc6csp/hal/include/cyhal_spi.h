/***************************************************************************//**
* \file cyhal_spi.h
*
* \brief
* Provides a high level interface for interacting with the Cypress SPI.
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
* \addtogroup group_hal_spi SPI (Serial Peripheral Interface)
* \ingroup group_hal
* \{
* High level interface for interacting with the Cypress SPI.
*
* \defgroup group_hal_spi_macros Macros
* \defgroup group_hal_spi_functions Functions
* \defgroup group_hal_spi_data_structures Data Structures
* \defgroup group_hal_spi_enums Enumerated Types
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "cy_result.h"
#include "cyhal_implementation.h"

#if defined(__cplusplus)
extern "C" {
#endif


/**
* \addtogroup group_hal_spi_macros
* \{
*/

/** Bad argument */
#define CYHAL_SPI_RSLT_BAD_ARGUMENT    (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_PWM, 0))
/** Failed to initialize SPI clock */
#define CYHAL_SPI_RSLT_CLOCK_ERROR     (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_PWM, 1))
/** Failed to Transfer SPI data */
#define CYHAL_SPI_RSLT_TRANSFER_ERROR  (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_PWM, 2))

/** \} group_hal_spi_macros */


/** SPI interrupt triggers */
typedef enum {
    CYHAL_SPI_IRQ_NONE                = 0,      //!< Disable all interrupt call backs
    /** All transfer data has been moved into data FIFO */
    CYHAL_SPI_IRQ_DATA_IN_FIFO        = 1 << 1,
    /** Transfer complete. */
    CYHAL_SPI_IRQ_DONE                = 1 << 2,
    /** An error occurred while transferring data */
    CYHAL_SPI_IRQ_ERROR               = 1 << 3,
} cyhal_spi_irq_event_t;

/** Handler for SPI interrupts */
typedef void (*cyhal_spi_irq_handler_t)(void *handler_arg, cyhal_spi_irq_event_t event);

/** SPI operating modes */
typedef enum
{
    /** Standard motorola SPI with MSB first operation */
    CYHAL_SPI_MODE_00_MSB,
    /** Standard motorola SPI with LSB first operation */
    CYHAL_SPI_MODE_00_LSB,
    /** TI SPI operation, where the slave select is a pulse. In this mode,
      * the pulse coincides with the first bit. MSB first
      */
    CYHAL_SPI_MODE_01_MSB,
    /** TI SPI operation, where the slave select is a pulse. In this mode,
      * the pulse coincides with the first bit. LSB first
      */
    CYHAL_SPI_MODE_01_LSB,
    /** National SPI operation. This is a half-duplex mode of operation. MSB first */
    CYHAL_SPI_MODE_10_MSB,
    /** National SPI operation. This is a half-duplex mode of operation. LSB first */
    CYHAL_SPI_MODE_10_LSB,
    /** TI SPI operation, where the slave select is a pulse. In this mode,
      * the pulse precedes with the first bit. MSB first
      */
    CYHAL_SPI_MODE_11_MSB,
    /** TI SPI operation, where the slave select is a pulse. In this mode,
      * the pulse precedes with the first bit. LSB first
      */
    CYHAL_SPI_MODE_11_LSB,
} cyhal_spi_mode_t;

/** Initial SPI configuration. */
typedef struct
{
    cyhal_spi_mode_t mode; //!< The operating mode
    uint8_t data_bits; //!< The number of bits per transfer
    bool is_slave; //!< Whether the peripheral is operating as slave or master
} cyhal_spi_cfg_t;

/** The requested resource type is invalid */
#define CYHAL_SPI_RSLT_ERR_INVALID_PIN (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_SPI, 0))

/** Initialize the SPI peripheral
 *
 * Configures the pins used by SPI, sets a default format and frequency, and enables the peripheral
 * @param[out] obj  The SPI object to initialize
 * @param[in]  mosi The pin to use for MOSI
 * @param[in]  miso The pin to use for MISO
 * @param[in]  sclk The pin to use for SCLK
 * @param[in]  ssel The pin to use for SSEL
 * @param[in]  clk The clock to use can be shared, if not provided a new clock will be allocated
 * @param[in]  bits      The number of bits per frame
 * @param[in]  mode      The SPI mode (clock polarity, phase, and shift direction)
 * @param[in]  is_slave  false for master mode or true for slave mode operation
 * @return The status of the init request
 */
cy_rslt_t cyhal_spi_init(cyhal_spi_t *obj, cyhal_gpio_t mosi, cyhal_gpio_t miso, cyhal_gpio_t sclk, cyhal_gpio_t ssel, const cyhal_clock_divider_t *clk,
                        uint8_t bits, cyhal_spi_mode_t mode, bool is_slave);

/** Release a SPI object
 *
 * Return the peripheral, pins and clock owned by the SPI object to their reset state
 * @param[in,out] obj The SPI object to deinitialize
 * @return The status of the free request
 */
cy_rslt_t cyhal_spi_free(cyhal_spi_t *obj);

/** Set the SPI baud rate
 *
 * Actual frequency may differ from the desired frequency due to available dividers and bus clock
 * Configures the SPI peripheral's baud rate
 * @param[in,out] obj The SPI object to configure
 * @param[in]     hz  The baud rate in Hz
 * @return The status of the frequency request
 */
cy_rslt_t cyhal_spi_frequency(cyhal_spi_t *obj, uint32_t hz);

/** Get a received value out of the SPI receive buffer
 *
 * Blocks until a value is available
 * @param[in] obj   The SPI peripheral to read
 * @param[in] value The value received
 * @return The status of the read request
 */
cy_rslt_t cyhal_spi_read(cyhal_spi_t *obj, uint8_t* value);

/** Write a byte out
 *
 * @param[in] obj   The SPI peripheral to use for sending
 * @param[in] value The value to send
 * @return The status of the write request
 */
cy_rslt_t cyhal_spi_write(cyhal_spi_t *obj, uint32_t value);

/** Write a block out and receive a value
 *
 *  The total number of bytes sent and received will be the maximum of
 *  tx_length and rx_length. The bytes written will be padded with the
 *  value 0xff.
 *
 * @param[in] obj           The SPI peripheral to use for sending
 * @param[in] tx            Pointer to the byte-array of data to write to the device
 * @param[in,out] tx_length Number of bytes to write, updated with the number actually written
 * @param[out] rx           Pointer to the byte-array of data to read from the device
 * @param[in,out] rx_length Number of bytes to read, udpated with the number actually read
 * @param[in] write_fill    Default data transmitted while performing a read
 * @return The status of the transfer request
 */
cy_rslt_t cyhal_spi_transfer(cyhal_spi_t *obj, const uint8_t *tx, size_t tx_length, uint8_t *rx, size_t rx_length, uint8_t write_fill);

/** Begin the SPI transfer. Buffer pointers and lengths are specified in tx_buff and rx_buff
 *
 * @param[in] obj           The SPI object that holds the transfer information
 * @param[in] tx            The transmit buffer
 * @param[in,out] tx_length The number of bytes to transmit
 * @param[out] rx           The receive buffer
 * @param[in,out] rx_length The number of bytes to receive
 * @return The status of the transfer_async request
 */
cy_rslt_t cyhal_spi_transfer_async(cyhal_spi_t *obj, const uint8_t *tx, size_t tx_length, uint8_t *rx, size_t rx_length);

/** Checks if the specified SPI peripheral is in use
 *
 * @param[in] obj  The SPI peripheral to check
 * @param[in] busy Indication of whether the SPI is still transmitting
 * @return The status of the is_busy request
 */
cy_rslt_t cyhal_spi_is_busy(cyhal_spi_t *obj, bool *busy);

/** Abort an SPI transfer
 *
 * @param[in] obj The SPI peripheral to stop
 * @return The status of the abort_async request
 */
cy_rslt_t cyhal_spi_abort_async(cyhal_spi_t *obj);

/** The SPI interrupt handler registration
 *
 * @param[in] obj         The SPI object
 * @param[in] handler     The callback handler which will be invoked when the interrupt fires
 * @param[in] handler_arg Generic argument that will be provided to the handler when called
 * @return The status of the register_irq request
 */
cy_rslt_t cyhal_spi_register_irq(cyhal_spi_t *obj, cyhal_spi_irq_handler_t handler, void *handler_arg);

/** Configure SPI interrupt. This function is used for word-approach
 *
 * @param[in] obj      The SPI object
 * @param[in] event    The SPI IRQ type
 * @param[in] enable   True to turn on interrupts, False to turn off
 * @return The status of the irq_enable request
 */
cy_rslt_t cyhal_spi_irq_enable(cyhal_spi_t *obj, cyhal_spi_irq_event_t event, bool enable);

/** \} group_hal_spi_functions */

#if defined(__cplusplus)
}
#endif
