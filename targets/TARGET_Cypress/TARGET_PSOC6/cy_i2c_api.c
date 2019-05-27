/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
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

#include "i2c_api.h"
#include "cyhal_i2c.h"
#include "mbed_error.h"
#include "mbed_assert.h"

#if DEVICE_I2C

#define CY_I2C_DEFAULT_TIMEOUT 200

static inline struct i2c_s *cy_get_i2c(i2c_t *obj)
{
#if DEVICE_I2C_ASYNCH
    return &(obj->i2c);
#else
    return obj;
#endif
}

#ifdef __cplusplus
extern "C" {
#endif

void i2c_init(i2c_t *obj, PinName sda, PinName scl)
{
    struct i2c_s *i2c = cy_get_i2c(obj);
    if (CY_RSLT_SUCCESS != cyhal_i2c_init(&(i2c->hal_i2c), sda, scl, NULL))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_i2c_init");
    i2c->cfg.is_slave = false;
    i2c->cfg.address = 0;
    i2c->cfg.frequencyhal_hz = 400;
    i2c->async_handler = NULL;
}

void i2c_frequency(i2c_t *obj, int hz)
{
    struct i2c_s *i2c = cy_get_i2c(obj);
    i2c->cfg.frequencyhal_hz = (uint32_t)hz;
    if (CY_RSLT_SUCCESS != cyhal_i2c_set_config(&(i2c->hal_i2c), &(i2c->cfg)))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_i2c_set_config");
}

int i2c_start(i2c_t *obj)
{
    // TODO: not supported by HAL
    MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_UNSUPPORTED), "i2c_start");
    return -1;
}

int i2c_stop(i2c_t *obj)
{
    // TODO: not supported by HAL
    MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_UNSUPPORTED), "i2c_stop");
    return -1;
}

int i2c_read(i2c_t *obj, int address, char *data, int length, int stop)
{
    struct i2c_s *i2c = cy_get_i2c(obj);
    MBED_ASSERT(stop != 0); // TODO: stop==0 not supported by HAL
    if (CY_RSLT_SUCCESS != cyhal_i2c_master_recv(&(i2c->hal_i2c), address, (uint8_t *)data, (uint16_t)length, CY_I2C_DEFAULT_TIMEOUT))
        return (int)I2C_ERROR_NO_SLAVE;
    return length;
}

int i2c_write(i2c_t *obj, int address, const char *data, int length, int stop)
{
    struct i2c_s *i2c = cy_get_i2c(obj);
    MBED_ASSERT(stop != 0); // TODO: stop==0 not supported by HAL
    if (CY_RSLT_SUCCESS != cyhal_i2c_master_send(&(i2c->hal_i2c), address, (const uint8_t *)data, (uint16_t)length, CY_I2C_DEFAULT_TIMEOUT))
        return -1;
    // TODO: receiver may accept fewer bytes than sent but HAL does not report how many bytes were sent
    return length;
}

void i2c_reset(i2c_t *obj)
{
    // TODO: The action here. Most of the implementation sends stop()
    i2c_stop(obj);
}

int i2c_byte_read(i2c_t *obj, int last)
{
    // TODO: HAL does not provide a way to do this in master mode
    MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_UNSUPPORTED), "i2c_byte_read");
    return -1;
}

int i2c_byte_write(i2c_t *obj, int data)
{
    // TODO: HAL does not provide a way to do this in master mode
    MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_UNSUPPORTED), "i2c_byte_write");
    return -1;
}

const PinMap *i2c_master_sda_pinmap(void)
{
    return PinMap_I2C_SDA;
}

const PinMap *i2c_master_scl_pinmap(void)
{
    return PinMap_I2C_SCL;
}

const PinMap *i2c_slave_sda_pinmap(void)
{
    return PinMap_I2C_SDA;
}

const PinMap *i2c_slave_scl_pinmap(void)
{
    return PinMap_I2C_SCL;
}

#if DEVICE_I2CSLAVE

void i2c_slave_mode(i2c_t *obj, int enable_slave)
{
    struct i2c_s *i2c = cy_get_i2c(obj);
    i2c->cfg.is_slave = (0 != enable_slave);
    if (CY_RSLT_SUCCESS != cyhal_i2c_set_config(&(i2c->hal_i2c), &(i2c->cfg)))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_i2c_set_config");
}

int  i2c_slave_receive(i2c_t *obj)
{
    // TODO: not supported by HAL
    MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_UNSUPPORTED), "i2c_slave_receive");
}

int  i2c_slave_read(i2c_t *obj, char *data, int length)
{
    struct i2c_s *i2c = cy_get_i2c(obj);
    if (CY_RSLT_SUCCESS != cyhal_i2c_slave_recv(&(i2c->hal_i2c), (uint8_t *)data, (uint16_t)length, CY_I2C_DEFAULT_TIMEOUT))
        return 0;
    return 1;
}

int  i2c_slave_write(i2c_t *obj, const char *data, int length)
{
    struct i2c_s *i2c = cy_get_i2c(obj);
    if (CY_RSLT_SUCCESS != cyhal_i2c_slave_send(&(i2c->hal_i2c), (const uint8_t *)data, (uint16_t)length, CY_I2C_DEFAULT_TIMEOUT))
        return 0;
    return 1;
}

void i2c_slave_address(i2c_t *obj, int idx, uint32_t address, uint32_t mask)
{
    struct i2c_s *i2c = cy_get_i2c(obj);
    i2c->cfg.address = address;
    // TODO: mask is not configurable in HAL
    if (CY_RSLT_SUCCESS != cyhal_i2c_set_config(&(i2c->hal_i2c), &(i2c->cfg)))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_i2c_set_config");
}

#endif

#if DEVICE_I2C_ASYNCH

static void cy_i2c_async_handler(void *handler_arg, cyhal_i2c_irq_event_t event)
{
    struct i2c_s *i2c = cy_get_i2c((i2c_t *)handler_arg);
    i2c->event = event;
    void (*async_handler)(void) = i2c->async_handler;
    if (NULL != async_handler)
        (*async_handler)();
}

void i2c_transfer_asynch(i2c_t *obj, const void *tx, size_t tx_length, void *rx, size_t rx_length, uint32_t address, uint32_t stop, uint32_t handler, uint32_t event, DMAUsage hint)
{
    struct i2c_s *i2c = cy_get_i2c(obj);
    i2c->async_rx_size = rx_length;
    i2c->async_handler = (void (*)(void))handler;
    if (CY_RSLT_SUCCESS != cyhal_i2c_register_irq(&(i2c->hal_i2c), &cy_i2c_async_handler, obj))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_i2c_register_irq");
    if (CY_RSLT_SUCCESS != cyhal_i2c_irq_enable(&(i2c->hal_i2c), CYHAL_I2C_SLAVE_ERR_EVENT | CYHAL_I2C_SLAVE_RD_CMPLT_EVENT | CYHAL_I2C_SLAVE_WR_CMPLT_EVENT | CYHAL_I2C_MASTER_ERR_EVENT | CYHAL_I2C_MASTER_RD_CMPLT_EVENT | CYHAL_I2C_MASTER_WR_CMPLT_EVENT, true))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_i2c_irq_enable");
    MBED_ASSERT(stop != 0); // TODO: stop==0 not supported by HAL
    if (CY_RSLT_SUCCESS != cyhal_i2c_transfer_async(&(i2c->hal_i2c), tx, tx_length, rx, rx_length, address))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_i2c_transfer_async");
}

uint32_t i2c_irq_handler_asynch(i2c_t *obj)
{
    //#define I2C_EVENT_ALL                 (I2C_EVENT_ERROR |  I2C_EVENT_TRANSFER_COMPLETE | I2C_EVENT_ERROR_NO_SLAVE | I2C_EVENT_TRANSFER_EARLY_NACK)
    struct i2c_s *i2c = cy_get_i2c(obj);
    uint32_t event = 0;
    if (CYHAL_I2C_IRQ_NONE != (i2c->event & (CYHAL_I2C_SLAVE_ERR_EVENT | CYHAL_I2C_MASTER_ERR_EVENT)))
        event |= I2C_EVENT_ERROR;
    if (CYHAL_I2C_IRQ_NONE != (i2c->event & (CYHAL_I2C_SLAVE_RD_CMPLT_EVENT | CYHAL_I2C_SLAVE_WR_CMPLT_EVENT | CYHAL_I2C_MASTER_RD_CMPLT_EVENT | CYHAL_I2C_MASTER_WR_CMPLT_EVENT)))
        event |= I2C_EVENT_TRANSFER_COMPLETE;
    return event;
}

uint8_t i2c_active(i2c_t *obj)
{
    // TODO: not supported by HAL
    MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_UNSUPPORTED), "i2c_active");
    return 0;
}

void i2c_abort_asynch(i2c_t *obj)
{
    struct i2c_s *i2c = cy_get_i2c(obj);
    if (CY_RSLT_SUCCESS != cyhal_i2c_abort_async(&(i2c->hal_i2c)))
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_CODE_FAILED_OPERATION), "cyhal_i2c_abort_async");
}

#endif

#ifdef __cplusplus
}
#endif

#endif
