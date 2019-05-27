/***************************************************************************//**
* \file cyhal_usb_dev.h
*
* \brief
* Provides a high level interface for interacting with the Cypress USB Device.
* This interface abstracts out the chip specific details. 
* If any chip specific functionality is necessary, or performance is critical 
* the low level functions can be used directly.
* 
********************************************************************************
* Copyright (c) 2019 Cypress Semiconductor.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "cy_result.h"
#include "cy_hal_implementation.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef uint8_t cyhal_usb_dev_ep_t;

/** Service Callback Events (this enumerated type is used by middleware) */
typedef enum
{
    CYHAL_USB_DEV_EVENT_BUS_RESET,  /**< Callback hooked to bus reset interrupt */
    CYHAL_USB_DEV_EVENT_EP0_SETUP,  /**< Callback hooked to endpoint 0 SETUP packet interrupt */
    CYHAL_USB_DEV_EVENT_EP0_IN,     /**< Callback hooked to endpoint 0 IN packet interrupt */
    CYHAL_USB_DEV_EVENT_EP0_OUT,    /**< Callback hooked to endpoint 0 OUT packet interrupt */
    CYHAL_USB_DEV_EVENT_SOF,        /**< Callback hooked to SOF interrupt */
} cyhal_usb_dev_event_t;

/** Service Callback Events (this enumerated type is used by middleware) */
typedef enum
{
    CYHAL_USB_DEV_MODE1_16_BIT,  /**< Endpoint management mode 1 using 16-bit registers to access endpoint data */
    
    /* CYHAL for USB Device v1.0 release supports only MODE 1 and 16-bit access!!!
    * The other enum value provided for understanding how future extension can be handled.
    */
    //CYHAL_USB_DEV_MODE2_16_BIT,  /**< Endpoint management mode 2 using 16-bit registers to access endpoint data */
    //CYHAL_USB_DEV_MODE3_16_BIT,  /**< Endpoint management mode 3 using 16-bit registers to access endpoint data */
    //CYHAL_USB_DEV_MODE1_16_BIT,  /**< Endpoint management mode 1 using 8-bit registers to access endpoint data */
    //CYHAL_USB_DEV_MODE2_16_BIT,  /**< Endpoint management mode 2 using 8-bit registers to access endpoint data */
    //CYHAL_USB_DEV_MODE3_16_BIT,  /**< Endpoint management mode 3 using 8-bit registers to access endpoint data */
} cyhal_usb_dev_mode_t;

/** Driver Configuration Structure */
typedef struct
{
    /** Endpoints management mode */
    cyhal_usb_dev_mode_t mode;
    
    /** The pointer to the buffer allocated for the USB Device endpoints. 
    * Reserved for future use.
    */
    uint8_t  *epBuffer;
    
    /** The size of the buffer for the USB Device endpoints. 
    * Reserved for future use.
    */
    uint16_t  epBufferSize;
} cyhal_usb_dev_cfg_t;


/** Handler for USB Device interrupt  */
typedef void (*cyhal_usb_dev_irq_handler_t)(void);

/** Handler for the transfer completion event for data endpoints (not applicable for endpoint 0)*/
typedef void (* cyhal_usb_dev_endpoint_handler_t)(cyhal_usb_dev_ep_t endpoint);

/** Handler for the events for USB Device */
typedef void (*cyhal_usb_dev_event_handler_t)(void);

/**
 * Initialize this USBPhy instance.
 *
 * @param[in,out] obj The usb device object
 * @param[in] dp      The D+ pin to initialize
 * @param[in] dm      The D- pin to initialize
 * @param[in] clkPll  The PLL clock to use can be shared, if not provided a new clock will be allocated
 * @param[in] clkDiv  The clock to use can be shared, if not provided a new clock will be allocated
 * @param[in]  cfg    The usb device configuration data, if not provided, default configuration is applied.
 *
 * @return The status of the initialization request
 */
 cy_rslt_t cyhal_usb_dev_init(cyhal_usb_dev_t *obj, cyhal_gpio_t dp, cyhal_gpio_t dm, 
                              const cyhal_clock_divider_t *clkPll, const cyhal_clock_divider_t *clkDiv, 
                              const cyhal_usb_dev_cfg_t *cfg);

/**
 * Enable USBPhy instance
 *
 * @param[in,out] obj The usb device object
 */
 void cyhal_usb_dev_enable(cyhal_usb_dev_t *obj);

/**
 * Power down this USBPhy instance
 *
 * Disable interrupts and stop sending events.
 *
 * @param[in,out] obj The usb device object
 */
 void cyhal_usb_dev_free(cyhal_usb_dev_t *obj);

/**
 * Make the USB phy visible to the USB host
 *
 * Enable either the D+ or D- pull-up so the host can detect
 * the presence of this device.
 *
 * @param[in,out] obj The usb device object
 *
 * @return The status of the connect request
 */
 cy_rslt_t cyhal_usb_dev_connect(cyhal_usb_dev_t *obj);

/**
 * Detach the USB phy
 *
 * Disable the D+ and D- pull-up and stop responding to
 * USB traffic.
 *
 * @param[in,out] obj The usb device object
 *
 * @return The status of the disconnect request
 */
 cy_rslt_t cyhal_usb_dev_disconnect(cyhal_usb_dev_t *obj);

/**
 * Set this device to the configured state
 *
 * Enable added endpoints if they are not enabled
 * already.
 *
 * @param[in,out] obj The usb device object
 *
 * @return The status of the configure request
 */
 cy_rslt_t cyhal_usb_dev_configure(cyhal_usb_dev_t *obj);

/**
 * Leave the configured state
 *
 * This is a notification to the USBPhy indicating that the device
 * is leaving the configured state. The USBPhy can disable all
 * endpoints other than endpoint 0.
 *
 * @param[in,out] obj The usb device object
 *
 * @return The status of the unconfigure request
 */
 cy_rslt_t cyhal_usb_dev_unconfigure(cyhal_usb_dev_t *obj);

/**
 * Configure start of frame interrupt enablement.
 *
 * @param[in,out] obj The usb device object
 * @param[in] enable  True to turn on interrupt and start calling sof callback on every frame, 
 *                    False to turn off interrupt and stop calling sof callback.
 *
 * @return The status of the sof enable request
 */
 cy_rslt_t cyhal_usb_dev_sof_enable(cyhal_usb_dev_t *obj, bool enable);

/**
 * Set the USBPhy's address
 *
 * @param[in,out] obj The usb device object
 * @param[in] address This device's USB address
 *
 * @return The status of the set address request
 */
 cy_rslt_t cyhal_usb_dev_set_address(cyhal_usb_dev_t *obj, uint8_t address);

/**
 * Get wMaxPacketSize of endpoint 0. 
 * The endpoint 0 has dedicated buffer.
 *
 * @param[in,out] obj       The usb device object
 *
 * @return The size allocated for endpoint 0
 */
uint32_t cyhal_usb_dev_ep0_get_max_packet(cyhal_usb_dev_t *obj);

/**
 * Read the contents of the SETUP packet
 *
 * @param[in,out] obj The usb device object
 * @param[in] buffer  Buffer to fill with data
 * @param[in] size    Size of buffer passed in
 * 
 * @return The status of the read endpoint 0 setup packet request
 */
 cy_rslt_t cyhal_usb_dev_ep0_setup_read_result(cyhal_usb_dev_t *obj, uint8_t *buffer, uint32_t size);

/**
 * Start receiving a packet of up to wMaxPacketSize on endpoint 0
 *
 * @param[in,out] obj The usb device object
 * @param[in] buffer  Buffer to fill with the data read
 * @param[in] size    Size of buffer
 *
 * @return The status of the read endpoint 0 request
 */
 cy_rslt_t cyhal_usb_dev_ep0_read(cyhal_usb_dev_t *obj, uint8_t *buffer, uint32_t size);

/**
 * Read the contents of a received packet
 *
 * @param[in,out] obj  The usb device object
 * @param[out] actSize Actual number of bytes that was read
 *
 * @return The status of the read results endpoint 0 request
 */
 cy_rslt_t cyhal_usb_dev_ep0_read_result(cyhal_usb_dev_t *obj, uint32_t *actSize);

/**
 * Write a packet on endpoint 0
 *
 * @param[in,out] obj The usb device object
 * @param[in] buffer  Buffer fill with data to send
 * @param[in] size    Size of data to send
 *
 * @return The status of the write endpoint 0 request
 */
 cy_rslt_t cyhal_usb_dev_ep0_write(cyhal_usb_dev_t *obj, uint8_t *buffer, uint32_t size);

/**
 * Protocol stall on endpoint 0.
 * Stall all IN and OUT packets on endpoint 0 until a SETUP packet is received.
 *
 * @param[in,out] obj The usb device object
 *
 * @return The status of the stall endpoint 0 request
 *
 * @note The stall is cleared automatically when a setup packet is received
 */
 cy_rslt_t cyhal_usb_dev_ep0_stall(cyhal_usb_dev_t *obj);

/**
 * Configure and enable an endpoint
 *
 * @param[in,out] obj   The usb device object
 * @param[in] endpoint  Endpoint to configure and enable
 * @param[in] maxPacket The maximum packet size that can be sent or received
 * @param[in] type      The type of endpoint
 *
 * @return The status of the endpoint add request
 * 
 * @note This function cannot be used to configure endpoint 0. That must be done
 * with ep0_set_max_packet
 */
 cy_rslt_t cyhal_usb_dev_endpoint_add(cyhal_usb_dev_t *obj, cyhal_usb_dev_ep_t endpoint, uint32_t maxPacket, cyhal_usb_dev_ep_type_t type);

/**
 * Disable an endpoint
 *
 * @param[in,out] obj  The usb device object
 * @param[in] endpoint Endpoint to disable
 */
 cy_rslt_t cyhal_usb_dev_endpoint_remove(cyhal_usb_dev_t *obj, cyhal_usb_dev_ep_t endpoint);

/**
 * Perform a functional stall on the given endpoint
 *
 * Set the HALT feature for this endpoint so that all further
 * communication is aborted.
 *
 * @param[in,out] obj  The usb device object
 * @param[in] endpoint Endpoint to stall
 */
 cy_rslt_t cyhal_usb_dev_endpoint_stall(cyhal_usb_dev_t *obj, cyhal_usb_dev_ep_t endpoint);

/**
 * Unstall the endpoint
 *
 * Clear the HALT feature on this endpoint so communication can
 * resume.
 *
 * @param[in,out] obj  The usb device object
 * @param[in] endpoint Endpoint to stall
 */
 cy_rslt_t cyhal_usb_dev_endpoint_unstall(cyhal_usb_dev_t *obj, cyhal_usb_dev_ep_t endpoint);

/**
 * Return the endpoint stall state
 *
 * @param[in,out] obj    The usb device object
 * @param[in] endpoint   Endpoint to check stall state
 * 
 * @return rue if endpoint stalled, false otherwise.
 */
 bool cyhal_usb_dev_endpoint_is_stalled(cyhal_usb_dev_t *obj, cyhal_usb_dev_ep_t endpoint);

/**
 * Start a read on the given endpoint
 *
 * @param[in,out] obj  The usb device object
 * @param[in] endpoint Endpoint to start the read on
 * @param[in] data     Buffer to fill with data
 * @param[in] size     Size of the read buffer. This must be at least
 *                     the max packet size for this endpoint.
 * 
 * @return The status of start a read operation
 */
 cy_rslt_t cyhal_usb_dev_endpoint_read(cyhal_usb_dev_t *obj, cyhal_usb_dev_ep_t endpoint, uint8_t *data, uint32_t size);

/**
 * Finish a read on the given endpoint
 *
 * @param[in,out] obj  The usb device object
 * @param[in] endpoint Endpoint to check
 * @param[out] actSize Actual number of bytes that was read
 *
 * @return The status of a finish read
 */
 cy_rslt_t cyhal_usb_dev_endpoint_read_result(cyhal_usb_dev_t *obj, cyhal_usb_dev_ep_t endpoint, uint32_t *actSize);

/**
 * Start a write on the given endpoint
 *
 * @param[in,out] obj  The usb device object
 * @param[in] endpoint Endpoint to write to
 * @param[in] data     Buffer to write
 * @param[in] size     Size of data to write
 *
 * @return The status of a write request
 */
 cy_rslt_t cyhal_usb_dev_endpoint_write(cyhal_usb_dev_t *obj, cyhal_usb_dev_ep_t endpoint, uint8_t *data, uint32_t size);

/**
 * Abort the current transfer if it has not yet been sent
 *
 * @param[in,out] obj  The usb device object
 * @param[in] endpoint Endpoint to abort the transfer on. It is implementation defined
 * if this function has an effect on receive endpoints.
 *
 * @return The status of an abort request
 */
 cy_rslt_t cyhal_usb_dev_endpoint_abort(cyhal_usb_dev_t *obj, cyhal_usb_dev_ep_t endpoint);

 /** The USB Device interrupt handler registration
 *
 * @param[in,out] obj The usb device object
 * @param[in] handler The interrupt handler function which will be invoked when the interrupt fires
 *
 * @return The status of the register_irq request
 */
cy_rslt_t cyhal_usb_dev_register_irq(cyhal_usb_dev_t *obj, cyhal_usb_dev_irq_handler_t handler);

/** 
 * Configure USB Device interrupt enablement.
 *
 * @param[in,out] obj The usb device object
 * @param[in] enable  True to turn on interrupts, False to turn off
 *
 * @return The status of the irq_enable request
 */
cy_rslt_t cyhal_usb_dev_irq_enable(cyhal_usb_dev_t *obj, bool enable);

/** 
 * The USB Device endpoint complete callback handler registration
 *
 * @param[in,out] obj  The usb device object
 * @param[in] endpoint Endpoint to registers handler 
 * @param[in] handler  The callback handler which will be invoked when the endpoint comp
 */
void cyhal_usb_dev_register_endpoint_callback(cyhal_usb_dev_t *obj, cyhal_usb_dev_ep_t endpoint, cyhal_usb_dev_endpoint_handler_t handler);

/** 
 * The USB Device event complete callback handler registration. The events are defined by x type.
 *
 * @param[in,out] obj The usb device object
 * @param[in] event   The event that triggers the callback, see /ref cyhal_usb_dev_event_t
 * @param[in] handler The callback handler which will be invoked when the interrupt fires
 */
void cyhal_usb_dev_register_event_callback(cyhal_usb_dev_t *obj, cyhal_usb_dev_event_t event, cyhal_usb_dev_event_handler_t handler);


#if defined(__cplusplus)
}
#endif