/*******************************************************************************
* File Name: cycfg_pins.h
*
* Description:
* Pin configuration
* This file was automatically generated and should not be modified.
* 
********************************************************************************
* Copyright 2017-2019 Cypress Semiconductor Corporation
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
********************************************************************************/

#if !defined(CYCFG_PINS_H)
#define CYCFG_PINS_H

#include "cycfg_notices.h"
#include "cy_gpio.h"
#include "cycfg_routing.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define CYBSP_WCO_IN_PORT GPIO_PRT0
#define CYBSP_WCO_IN_PIN 0U
#define CYBSP_WCO_IN_NUM 0U
#define CYBSP_WCO_IN_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_WCO_IN_INIT_DRIVESTATE 1
#ifndef ioss_0_port_0_pin_0_HSIOM
	#define ioss_0_port_0_pin_0_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_WCO_IN_HSIOM ioss_0_port_0_pin_0_HSIOM
#define CYBSP_WCO_IN_IRQ ioss_interrupts_gpio_0_IRQn
#define CYBSP_WCO_OUT_PORT GPIO_PRT0
#define CYBSP_WCO_OUT_PIN 1U
#define CYBSP_WCO_OUT_NUM 1U
#define CYBSP_WCO_OUT_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_WCO_OUT_INIT_DRIVESTATE 1
#ifndef ioss_0_port_0_pin_1_HSIOM
	#define ioss_0_port_0_pin_1_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_WCO_OUT_HSIOM ioss_0_port_0_pin_1_HSIOM
#define CYBSP_WCO_OUT_IRQ ioss_interrupts_gpio_0_IRQn
#define CYBSP_LED_RED_PORT GPIO_PRT0
#define CYBSP_LED_RED_PIN 3U
#define CYBSP_LED_RED_NUM 3U
#define CYBSP_LED_RED_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_LED_RED_INIT_DRIVESTATE 1
#ifndef ioss_0_port_0_pin_3_HSIOM
	#define ioss_0_port_0_pin_3_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_LED_RED_HSIOM ioss_0_port_0_pin_3_HSIOM
#define CYBSP_LED_RED_IRQ ioss_interrupts_gpio_0_IRQn
#define CYBSP_BTN2_PORT GPIO_PRT0
#define CYBSP_BTN2_PIN 4U
#define CYBSP_BTN2_NUM 4U
#define CYBSP_BTN2_DRIVEMODE CY_GPIO_DM_PULLUP
#define CYBSP_BTN2_INIT_DRIVESTATE 1
#ifndef ioss_0_port_0_pin_4_HSIOM
	#define ioss_0_port_0_pin_4_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_BTN2_HSIOM ioss_0_port_0_pin_4_HSIOM
#define CYBSP_BTN2_IRQ ioss_interrupts_gpio_0_IRQn
#define CYBSP_LED_BLUE_PORT GPIO_PRT11
#define CYBSP_LED_BLUE_PIN 1U
#define CYBSP_LED_BLUE_NUM 1U
#define CYBSP_LED_BLUE_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_LED_BLUE_INIT_DRIVESTATE 1
#ifndef ioss_0_port_11_pin_1_HSIOM
	#define ioss_0_port_11_pin_1_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_LED_BLUE_HSIOM ioss_0_port_11_pin_1_HSIOM
#define CYBSP_LED_BLUE_IRQ ioss_interrupts_gpio_11_IRQn
#define CYBSP_QSPI_SS_PORT GPIO_PRT11
#define CYBSP_QSPI_SS_PIN 2U
#define CYBSP_QSPI_SS_NUM 2U
#define CYBSP_QSPI_SS_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_QSPI_SS_INIT_DRIVESTATE 1
#ifndef ioss_0_port_11_pin_2_HSIOM
	#define ioss_0_port_11_pin_2_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_QSPI_SS_HSIOM ioss_0_port_11_pin_2_HSIOM
#define CYBSP_QSPI_SS_IRQ ioss_interrupts_gpio_11_IRQn
#define CYBSP_QSPI_D3_PORT GPIO_PRT11
#define CYBSP_QSPI_D3_PIN 3U
#define CYBSP_QSPI_D3_NUM 3U
#define CYBSP_QSPI_D3_DRIVEMODE CY_GPIO_DM_STRONG
#define CYBSP_QSPI_D3_INIT_DRIVESTATE 1
#ifndef ioss_0_port_11_pin_3_HSIOM
	#define ioss_0_port_11_pin_3_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_QSPI_D3_HSIOM ioss_0_port_11_pin_3_HSIOM
#define CYBSP_QSPI_D3_IRQ ioss_interrupts_gpio_11_IRQn
#define CYBSP_QSPI_D2_PORT GPIO_PRT11
#define CYBSP_QSPI_D2_PIN 4U
#define CYBSP_QSPI_D2_NUM 4U
#define CYBSP_QSPI_D2_DRIVEMODE CY_GPIO_DM_STRONG
#define CYBSP_QSPI_D2_INIT_DRIVESTATE 1
#ifndef ioss_0_port_11_pin_4_HSIOM
	#define ioss_0_port_11_pin_4_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_QSPI_D2_HSIOM ioss_0_port_11_pin_4_HSIOM
#define CYBSP_QSPI_D2_IRQ ioss_interrupts_gpio_11_IRQn
#define CYBSP_QSPI_D1_PORT GPIO_PRT11
#define CYBSP_QSPI_D1_PIN 5U
#define CYBSP_QSPI_D1_NUM 5U
#define CYBSP_QSPI_D1_DRIVEMODE CY_GPIO_DM_STRONG
#define CYBSP_QSPI_D1_INIT_DRIVESTATE 1
#ifndef ioss_0_port_11_pin_5_HSIOM
	#define ioss_0_port_11_pin_5_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_QSPI_D1_HSIOM ioss_0_port_11_pin_5_HSIOM
#define CYBSP_QSPI_D1_IRQ ioss_interrupts_gpio_11_IRQn
#define CYBSP_QSPI_D0_PORT GPIO_PRT11
#define CYBSP_QSPI_D0_PIN 6U
#define CYBSP_QSPI_D0_NUM 6U
#define CYBSP_QSPI_D0_DRIVEMODE CY_GPIO_DM_STRONG
#define CYBSP_QSPI_D0_INIT_DRIVESTATE 1
#ifndef ioss_0_port_11_pin_6_HSIOM
	#define ioss_0_port_11_pin_6_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_QSPI_D0_HSIOM ioss_0_port_11_pin_6_HSIOM
#define CYBSP_QSPI_D0_IRQ ioss_interrupts_gpio_11_IRQn
#define CYBSP_QSPI_SCK_PORT GPIO_PRT11
#define CYBSP_QSPI_SCK_PIN 7U
#define CYBSP_QSPI_SCK_NUM 7U
#define CYBSP_QSPI_SCK_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_QSPI_SCK_INIT_DRIVESTATE 1
#ifndef ioss_0_port_11_pin_7_HSIOM
	#define ioss_0_port_11_pin_7_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_QSPI_SCK_HSIOM ioss_0_port_11_pin_7_HSIOM
#define CYBSP_QSPI_SCK_IRQ ioss_interrupts_gpio_11_IRQn
#define CYBSP_LED9_PORT GPIO_PRT13
#define CYBSP_LED9_PIN 7U
#define CYBSP_LED9_NUM 7U
#define CYBSP_LED9_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_LED9_INIT_DRIVESTATE 1
#ifndef ioss_0_port_13_pin_7_HSIOM
	#define ioss_0_port_13_pin_7_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_LED9_HSIOM ioss_0_port_13_pin_7_HSIOM
#define CYBSP_LED9_IRQ ioss_interrupts_gpio_13_IRQn
#define ioss_0_port_14_pin_0_PORT GPIO_PRT14
#define ioss_0_port_14_pin_0_PIN 0U
#define ioss_0_port_14_pin_0_NUM 0U
#define ioss_0_port_14_pin_0_DRIVEMODE CY_GPIO_DM_ANALOG
#define ioss_0_port_14_pin_0_INIT_DRIVESTATE 1
#ifndef ioss_0_port_14_pin_0_HSIOM
	#define ioss_0_port_14_pin_0_HSIOM HSIOM_SEL_GPIO
#endif
#define ioss_0_port_14_pin_0_IRQ ioss_interrupts_gpio_14_IRQn
#define ioss_0_port_14_pin_1_PORT GPIO_PRT14
#define ioss_0_port_14_pin_1_PIN 1U
#define ioss_0_port_14_pin_1_NUM 1U
#define ioss_0_port_14_pin_1_DRIVEMODE CY_GPIO_DM_ANALOG
#define ioss_0_port_14_pin_1_INIT_DRIVESTATE 1
#ifndef ioss_0_port_14_pin_1_HSIOM
	#define ioss_0_port_14_pin_1_HSIOM HSIOM_SEL_GPIO
#endif
#define ioss_0_port_14_pin_1_IRQ ioss_interrupts_gpio_14_IRQn
#define CYBSP_CSD_TX_PORT GPIO_PRT1
#define CYBSP_CSD_TX_PIN 0U
#define CYBSP_CSD_TX_NUM 0U
#define CYBSP_CSD_TX_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_CSD_TX_INIT_DRIVESTATE 1
#ifndef ioss_0_port_1_pin_0_HSIOM
	#define ioss_0_port_1_pin_0_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_CSD_TX_HSIOM ioss_0_port_1_pin_0_HSIOM
#define CYBSP_CSD_TX_IRQ ioss_interrupts_gpio_1_IRQn
#define CYBSP_LED_GREEN_PORT GPIO_PRT1
#define CYBSP_LED_GREEN_PIN 1U
#define CYBSP_LED_GREEN_NUM 1U
#define CYBSP_LED_GREEN_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_LED_GREEN_INIT_DRIVESTATE 1
#ifndef ioss_0_port_1_pin_1_HSIOM
	#define ioss_0_port_1_pin_1_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_LED_GREEN_HSIOM ioss_0_port_1_pin_1_HSIOM
#define CYBSP_LED_GREEN_IRQ ioss_interrupts_gpio_1_IRQn
#define CYBSP_LED8_PORT GPIO_PRT1
#define CYBSP_LED8_PIN 5U
#define CYBSP_LED8_NUM 5U
#define CYBSP_LED8_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_LED8_INIT_DRIVESTATE 1
#ifndef ioss_0_port_1_pin_5_HSIOM
	#define ioss_0_port_1_pin_5_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_LED8_HSIOM ioss_0_port_1_pin_5_HSIOM
#define CYBSP_LED8_IRQ ioss_interrupts_gpio_1_IRQn
#define CYBSP_WIFI_SDIO_D0_PORT GPIO_PRT2
#define CYBSP_WIFI_SDIO_D0_PIN 0U
#define CYBSP_WIFI_SDIO_D0_NUM 0U
#define CYBSP_WIFI_SDIO_D0_DRIVEMODE CY_GPIO_DM_STRONG
#define CYBSP_WIFI_SDIO_D0_INIT_DRIVESTATE 1
#ifndef ioss_0_port_2_pin_0_HSIOM
	#define ioss_0_port_2_pin_0_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_WIFI_SDIO_D0_HSIOM ioss_0_port_2_pin_0_HSIOM
#define CYBSP_WIFI_SDIO_D0_IRQ ioss_interrupts_gpio_2_IRQn
#define CYBSP_WIFI_SDIO_D1_PORT GPIO_PRT2
#define CYBSP_WIFI_SDIO_D1_PIN 1U
#define CYBSP_WIFI_SDIO_D1_NUM 1U
#define CYBSP_WIFI_SDIO_D1_DRIVEMODE CY_GPIO_DM_STRONG
#define CYBSP_WIFI_SDIO_D1_INIT_DRIVESTATE 1
#ifndef ioss_0_port_2_pin_1_HSIOM
	#define ioss_0_port_2_pin_1_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_WIFI_SDIO_D1_HSIOM ioss_0_port_2_pin_1_HSIOM
#define CYBSP_WIFI_SDIO_D1_IRQ ioss_interrupts_gpio_2_IRQn
#define CYBSP_WIFI_SDIO_D2_PORT GPIO_PRT2
#define CYBSP_WIFI_SDIO_D2_PIN 2U
#define CYBSP_WIFI_SDIO_D2_NUM 2U
#define CYBSP_WIFI_SDIO_D2_DRIVEMODE CY_GPIO_DM_STRONG
#define CYBSP_WIFI_SDIO_D2_INIT_DRIVESTATE 1
#ifndef ioss_0_port_2_pin_2_HSIOM
	#define ioss_0_port_2_pin_2_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_WIFI_SDIO_D2_HSIOM ioss_0_port_2_pin_2_HSIOM
#define CYBSP_WIFI_SDIO_D2_IRQ ioss_interrupts_gpio_2_IRQn
#define CYBSP_WIFI_SDIO_D3_PORT GPIO_PRT2
#define CYBSP_WIFI_SDIO_D3_PIN 3U
#define CYBSP_WIFI_SDIO_D3_NUM 3U
#define CYBSP_WIFI_SDIO_D3_DRIVEMODE CY_GPIO_DM_STRONG
#define CYBSP_WIFI_SDIO_D3_INIT_DRIVESTATE 1
#ifndef ioss_0_port_2_pin_3_HSIOM
	#define ioss_0_port_2_pin_3_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_WIFI_SDIO_D3_HSIOM ioss_0_port_2_pin_3_HSIOM
#define CYBSP_WIFI_SDIO_D3_IRQ ioss_interrupts_gpio_2_IRQn
#define CYBSP_WIFI_SDIO_CMD_PORT GPIO_PRT2
#define CYBSP_WIFI_SDIO_CMD_PIN 4U
#define CYBSP_WIFI_SDIO_CMD_NUM 4U
#define CYBSP_WIFI_SDIO_CMD_DRIVEMODE CY_GPIO_DM_STRONG
#define CYBSP_WIFI_SDIO_CMD_INIT_DRIVESTATE 1
#ifndef ioss_0_port_2_pin_4_HSIOM
	#define ioss_0_port_2_pin_4_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_WIFI_SDIO_CMD_HSIOM ioss_0_port_2_pin_4_HSIOM
#define CYBSP_WIFI_SDIO_CMD_IRQ ioss_interrupts_gpio_2_IRQn
#define CYBSP_WIFI_SDIO_CLK_PORT GPIO_PRT2
#define CYBSP_WIFI_SDIO_CLK_PIN 5U
#define CYBSP_WIFI_SDIO_CLK_NUM 5U
#define CYBSP_WIFI_SDIO_CLK_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_WIFI_SDIO_CLK_INIT_DRIVESTATE 1
#ifndef ioss_0_port_2_pin_5_HSIOM
	#define ioss_0_port_2_pin_5_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_WIFI_SDIO_CLK_HSIOM ioss_0_port_2_pin_5_HSIOM
#define CYBSP_WIFI_SDIO_CLK_IRQ ioss_interrupts_gpio_2_IRQn
#define CYBSP_WIFI_WL_REG_ON_PORT GPIO_PRT2
#define CYBSP_WIFI_WL_REG_ON_PIN 6U
#define CYBSP_WIFI_WL_REG_ON_NUM 6U
#define CYBSP_WIFI_WL_REG_ON_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_WIFI_WL_REG_ON_INIT_DRIVESTATE 0
#ifndef ioss_0_port_2_pin_6_HSIOM
	#define ioss_0_port_2_pin_6_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_WIFI_WL_REG_ON_HSIOM ioss_0_port_2_pin_6_HSIOM
#define CYBSP_WIFI_WL_REG_ON_IRQ ioss_interrupts_gpio_2_IRQn
#define CYBSP_WIFI_HOST_WAKE_PORT GPIO_PRT2
#define CYBSP_WIFI_HOST_WAKE_PIN 7U
#define CYBSP_WIFI_HOST_WAKE_NUM 7U
#define CYBSP_WIFI_HOST_WAKE_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_WIFI_HOST_WAKE_INIT_DRIVESTATE 0
#ifndef ioss_0_port_2_pin_7_HSIOM
	#define ioss_0_port_2_pin_7_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_WIFI_HOST_WAKE_HSIOM ioss_0_port_2_pin_7_HSIOM
#define CYBSP_WIFI_HOST_WAKE_IRQ ioss_interrupts_gpio_2_IRQn
#define CYBSP_BT_UART_RX_PORT GPIO_PRT3
#define CYBSP_BT_UART_RX_PIN 0U
#define CYBSP_BT_UART_RX_NUM 0U
#define CYBSP_BT_UART_RX_DRIVEMODE CY_GPIO_DM_HIGHZ
#define CYBSP_BT_UART_RX_INIT_DRIVESTATE 1
#ifndef ioss_0_port_3_pin_0_HSIOM
	#define ioss_0_port_3_pin_0_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_BT_UART_RX_HSIOM ioss_0_port_3_pin_0_HSIOM
#define CYBSP_BT_UART_RX_IRQ ioss_interrupts_gpio_3_IRQn
#define CYBSP_BT_UART_TX_PORT GPIO_PRT3
#define CYBSP_BT_UART_TX_PIN 1U
#define CYBSP_BT_UART_TX_NUM 1U
#define CYBSP_BT_UART_TX_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_BT_UART_TX_INIT_DRIVESTATE 1
#ifndef ioss_0_port_3_pin_1_HSIOM
	#define ioss_0_port_3_pin_1_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_BT_UART_TX_HSIOM ioss_0_port_3_pin_1_HSIOM
#define CYBSP_BT_UART_TX_IRQ ioss_interrupts_gpio_3_IRQn
#define CYBSP_BT_UART_RTS_PORT GPIO_PRT3
#define CYBSP_BT_UART_RTS_PIN 2U
#define CYBSP_BT_UART_RTS_NUM 2U
#define CYBSP_BT_UART_RTS_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_BT_UART_RTS_INIT_DRIVESTATE 1
#ifndef ioss_0_port_3_pin_2_HSIOM
	#define ioss_0_port_3_pin_2_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_BT_UART_RTS_HSIOM ioss_0_port_3_pin_2_HSIOM
#define CYBSP_BT_UART_RTS_IRQ ioss_interrupts_gpio_3_IRQn
#define CYBSP_BT_UART_CTS_PORT GPIO_PRT3
#define CYBSP_BT_UART_CTS_PIN 3U
#define CYBSP_BT_UART_CTS_NUM 3U
#define CYBSP_BT_UART_CTS_DRIVEMODE CY_GPIO_DM_HIGHZ
#define CYBSP_BT_UART_CTS_INIT_DRIVESTATE 1
#ifndef ioss_0_port_3_pin_3_HSIOM
	#define ioss_0_port_3_pin_3_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_BT_UART_CTS_HSIOM ioss_0_port_3_pin_3_HSIOM
#define CYBSP_BT_UART_CTS_IRQ ioss_interrupts_gpio_3_IRQn
#define CYBSP_BT_POWER_PORT GPIO_PRT3
#define CYBSP_BT_POWER_PIN 4U
#define CYBSP_BT_POWER_NUM 4U
#define CYBSP_BT_POWER_DRIVEMODE CY_GPIO_DM_OD_DRIVESHIGH_IN_OFF
#define CYBSP_BT_POWER_INIT_DRIVESTATE 1
#ifndef ioss_0_port_3_pin_4_HSIOM
	#define ioss_0_port_3_pin_4_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_BT_POWER_HSIOM ioss_0_port_3_pin_4_HSIOM
#define CYBSP_BT_POWER_IRQ ioss_interrupts_gpio_3_IRQn
#define CYBSP_BT_HOST_WAKE_PORT GPIO_PRT3
#define CYBSP_BT_HOST_WAKE_PIN 5U
#define CYBSP_BT_HOST_WAKE_NUM 5U
#define CYBSP_BT_HOST_WAKE_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_BT_HOST_WAKE_INIT_DRIVESTATE 0
#ifndef ioss_0_port_3_pin_5_HSIOM
	#define ioss_0_port_3_pin_5_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_BT_HOST_WAKE_HSIOM ioss_0_port_3_pin_5_HSIOM
#define CYBSP_BT_HOST_WAKE_IRQ ioss_interrupts_gpio_3_IRQn
#define CYBSP_BT_DEVICE_WAKE_PORT GPIO_PRT4
#define CYBSP_BT_DEVICE_WAKE_PIN 0U
#define CYBSP_BT_DEVICE_WAKE_NUM 0U
#define CYBSP_BT_DEVICE_WAKE_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_BT_DEVICE_WAKE_INIT_DRIVESTATE 0
#ifndef ioss_0_port_4_pin_0_HSIOM
	#define ioss_0_port_4_pin_0_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_BT_DEVICE_WAKE_HSIOM ioss_0_port_4_pin_0_HSIOM
#define CYBSP_BT_DEVICE_WAKE_IRQ ioss_interrupts_gpio_4_IRQn
#define CYBSP_UART_RX_PORT GPIO_PRT5
#define CYBSP_UART_RX_PIN 0U
#define CYBSP_UART_RX_NUM 0U
#define CYBSP_UART_RX_DRIVEMODE CY_GPIO_DM_HIGHZ
#define CYBSP_UART_RX_INIT_DRIVESTATE 1
#ifndef ioss_0_port_5_pin_0_HSIOM
	#define ioss_0_port_5_pin_0_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_UART_RX_HSIOM ioss_0_port_5_pin_0_HSIOM
#define CYBSP_UART_RX_IRQ ioss_interrupts_gpio_5_IRQn
#define CYBSP_UART_TX_PORT GPIO_PRT5
#define CYBSP_UART_TX_PIN 1U
#define CYBSP_UART_TX_NUM 1U
#define CYBSP_UART_TX_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_UART_TX_INIT_DRIVESTATE 1
#ifndef ioss_0_port_5_pin_1_HSIOM
	#define ioss_0_port_5_pin_1_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_UART_TX_HSIOM ioss_0_port_5_pin_1_HSIOM
#define CYBSP_UART_TX_IRQ ioss_interrupts_gpio_5_IRQn
#define CYBSP_I2C_SCL_PORT GPIO_PRT6
#define CYBSP_I2C_SCL_PIN 0U
#define CYBSP_I2C_SCL_NUM 0U
#define CYBSP_I2C_SCL_DRIVEMODE CY_GPIO_DM_OD_DRIVESLOW
#define CYBSP_I2C_SCL_INIT_DRIVESTATE 1
#ifndef ioss_0_port_6_pin_0_HSIOM
	#define ioss_0_port_6_pin_0_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_I2C_SCL_HSIOM ioss_0_port_6_pin_0_HSIOM
#define CYBSP_I2C_SCL_IRQ ioss_interrupts_gpio_6_IRQn
#define CYBSP_I2C_SDA_PORT GPIO_PRT6
#define CYBSP_I2C_SDA_PIN 1U
#define CYBSP_I2C_SDA_NUM 1U
#define CYBSP_I2C_SDA_DRIVEMODE CY_GPIO_DM_OD_DRIVESLOW
#define CYBSP_I2C_SDA_INIT_DRIVESTATE 1
#ifndef ioss_0_port_6_pin_1_HSIOM
	#define ioss_0_port_6_pin_1_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_I2C_SDA_HSIOM ioss_0_port_6_pin_1_HSIOM
#define CYBSP_I2C_SDA_IRQ ioss_interrupts_gpio_6_IRQn
#define CYBSP_SWO_PORT GPIO_PRT6
#define CYBSP_SWO_PIN 4U
#define CYBSP_SWO_NUM 4U
#define CYBSP_SWO_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define CYBSP_SWO_INIT_DRIVESTATE 1
#ifndef ioss_0_port_6_pin_4_HSIOM
	#define ioss_0_port_6_pin_4_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_SWO_HSIOM ioss_0_port_6_pin_4_HSIOM
#define CYBSP_SWO_IRQ ioss_interrupts_gpio_6_IRQn
#define CYBSP_SWDIO_PORT GPIO_PRT6
#define CYBSP_SWDIO_PIN 6U
#define CYBSP_SWDIO_NUM 6U
#define CYBSP_SWDIO_DRIVEMODE CY_GPIO_DM_PULLUP
#define CYBSP_SWDIO_INIT_DRIVESTATE 1
#ifndef ioss_0_port_6_pin_6_HSIOM
	#define ioss_0_port_6_pin_6_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_SWDIO_HSIOM ioss_0_port_6_pin_6_HSIOM
#define CYBSP_SWDIO_IRQ ioss_interrupts_gpio_6_IRQn
#define CYBSP_SWDCK_PORT GPIO_PRT6
#define CYBSP_SWDCK_PIN 7U
#define CYBSP_SWDCK_NUM 7U
#define CYBSP_SWDCK_DRIVEMODE CY_GPIO_DM_PULLDOWN
#define CYBSP_SWDCK_INIT_DRIVESTATE 1
#ifndef ioss_0_port_6_pin_7_HSIOM
	#define ioss_0_port_6_pin_7_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_SWDCK_HSIOM ioss_0_port_6_pin_7_HSIOM
#define CYBSP_SWDCK_IRQ ioss_interrupts_gpio_6_IRQn
#define CYBSP_CINA_PORT GPIO_PRT7
#define CYBSP_CINA_PIN 1U
#define CYBSP_CINA_NUM 1U
#define CYBSP_CINA_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_CINA_INIT_DRIVESTATE 1
#ifndef ioss_0_port_7_pin_1_HSIOM
	#define ioss_0_port_7_pin_1_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_CINA_HSIOM ioss_0_port_7_pin_1_HSIOM
#define CYBSP_CINA_IRQ ioss_interrupts_gpio_7_IRQn
#define CYBSP_CINB_PORT GPIO_PRT7
#define CYBSP_CINB_PIN 2U
#define CYBSP_CINB_NUM 2U
#define CYBSP_CINB_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_CINB_INIT_DRIVESTATE 1
#ifndef ioss_0_port_7_pin_2_HSIOM
	#define ioss_0_port_7_pin_2_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_CINB_HSIOM ioss_0_port_7_pin_2_HSIOM
#define CYBSP_CINB_IRQ ioss_interrupts_gpio_7_IRQn
#define CYBSP_CMOD_PORT GPIO_PRT7
#define CYBSP_CMOD_PIN 7U
#define CYBSP_CMOD_NUM 7U
#define CYBSP_CMOD_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_CMOD_INIT_DRIVESTATE 1
#ifndef ioss_0_port_7_pin_7_HSIOM
	#define ioss_0_port_7_pin_7_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_CMOD_HSIOM ioss_0_port_7_pin_7_HSIOM
#define CYBSP_CMOD_IRQ ioss_interrupts_gpio_7_IRQn
#define CYBSP_CSD_BTN0_PORT GPIO_PRT8
#define CYBSP_CSD_BTN0_PIN 1U
#define CYBSP_CSD_BTN0_NUM 1U
#define CYBSP_CSD_BTN0_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_CSD_BTN0_INIT_DRIVESTATE 1
#ifndef ioss_0_port_8_pin_1_HSIOM
	#define ioss_0_port_8_pin_1_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_CSD_BTN0_HSIOM ioss_0_port_8_pin_1_HSIOM
#define CYBSP_CSD_BTN0_IRQ ioss_interrupts_gpio_8_IRQn
#define CYBSP_CSD_BTN1_PORT GPIO_PRT8
#define CYBSP_CSD_BTN1_PIN 2U
#define CYBSP_CSD_BTN1_NUM 2U
#define CYBSP_CSD_BTN1_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_CSD_BTN1_INIT_DRIVESTATE 1
#ifndef ioss_0_port_8_pin_2_HSIOM
	#define ioss_0_port_8_pin_2_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_CSD_BTN1_HSIOM ioss_0_port_8_pin_2_HSIOM
#define CYBSP_CSD_BTN1_IRQ ioss_interrupts_gpio_8_IRQn
#define CYBSP_CSD_SLD0_PORT GPIO_PRT8
#define CYBSP_CSD_SLD0_PIN 3U
#define CYBSP_CSD_SLD0_NUM 3U
#define CYBSP_CSD_SLD0_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_CSD_SLD0_INIT_DRIVESTATE 1
#ifndef ioss_0_port_8_pin_3_HSIOM
	#define ioss_0_port_8_pin_3_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_CSD_SLD0_HSIOM ioss_0_port_8_pin_3_HSIOM
#define CYBSP_CSD_SLD0_IRQ ioss_interrupts_gpio_8_IRQn
#define CYBSP_CSD_SLD1_PORT GPIO_PRT8
#define CYBSP_CSD_SLD1_PIN 4U
#define CYBSP_CSD_SLD1_NUM 4U
#define CYBSP_CSD_SLD1_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_CSD_SLD1_INIT_DRIVESTATE 1
#ifndef ioss_0_port_8_pin_4_HSIOM
	#define ioss_0_port_8_pin_4_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_CSD_SLD1_HSIOM ioss_0_port_8_pin_4_HSIOM
#define CYBSP_CSD_SLD1_IRQ ioss_interrupts_gpio_8_IRQn
#define CYBSP_CSD_SLD2_PORT GPIO_PRT8
#define CYBSP_CSD_SLD2_PIN 5U
#define CYBSP_CSD_SLD2_NUM 5U
#define CYBSP_CSD_SLD2_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_CSD_SLD2_INIT_DRIVESTATE 1
#ifndef ioss_0_port_8_pin_5_HSIOM
	#define ioss_0_port_8_pin_5_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_CSD_SLD2_HSIOM ioss_0_port_8_pin_5_HSIOM
#define CYBSP_CSD_SLD2_IRQ ioss_interrupts_gpio_8_IRQn
#define CYBSP_CSD_SLD3_PORT GPIO_PRT8
#define CYBSP_CSD_SLD3_PIN 6U
#define CYBSP_CSD_SLD3_NUM 6U
#define CYBSP_CSD_SLD3_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_CSD_SLD3_INIT_DRIVESTATE 1
#ifndef ioss_0_port_8_pin_6_HSIOM
	#define ioss_0_port_8_pin_6_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_CSD_SLD3_HSIOM ioss_0_port_8_pin_6_HSIOM
#define CYBSP_CSD_SLD3_IRQ ioss_interrupts_gpio_8_IRQn
#define CYBSP_CSD_SLD4_PORT GPIO_PRT8
#define CYBSP_CSD_SLD4_PIN 7U
#define CYBSP_CSD_SLD4_NUM 7U
#define CYBSP_CSD_SLD4_DRIVEMODE CY_GPIO_DM_ANALOG
#define CYBSP_CSD_SLD4_INIT_DRIVESTATE 1
#ifndef ioss_0_port_8_pin_7_HSIOM
	#define ioss_0_port_8_pin_7_HSIOM HSIOM_SEL_GPIO
#endif
#define CYBSP_CSD_SLD4_HSIOM ioss_0_port_8_pin_7_HSIOM
#define CYBSP_CSD_SLD4_IRQ ioss_interrupts_gpio_8_IRQn

extern const cy_stc_gpio_pin_config_t CYBSP_WCO_IN_config;
extern const cy_stc_gpio_pin_config_t CYBSP_WCO_OUT_config;
extern const cy_stc_gpio_pin_config_t CYBSP_LED_RED_config;
extern const cy_stc_gpio_pin_config_t CYBSP_BTN2_config;
extern const cy_stc_gpio_pin_config_t CYBSP_LED_BLUE_config;
extern const cy_stc_gpio_pin_config_t CYBSP_QSPI_SS_config;
extern const cy_stc_gpio_pin_config_t CYBSP_QSPI_D3_config;
extern const cy_stc_gpio_pin_config_t CYBSP_QSPI_D2_config;
extern const cy_stc_gpio_pin_config_t CYBSP_QSPI_D1_config;
extern const cy_stc_gpio_pin_config_t CYBSP_QSPI_D0_config;
extern const cy_stc_gpio_pin_config_t CYBSP_QSPI_SCK_config;
extern const cy_stc_gpio_pin_config_t CYBSP_LED9_config;
extern const cy_stc_gpio_pin_config_t ioss_0_port_14_pin_0_config;
extern const cy_stc_gpio_pin_config_t ioss_0_port_14_pin_1_config;
extern const cy_stc_gpio_pin_config_t CYBSP_CSD_TX_config;
extern const cy_stc_gpio_pin_config_t CYBSP_LED_GREEN_config;
extern const cy_stc_gpio_pin_config_t CYBSP_LED8_config;
extern const cy_stc_gpio_pin_config_t CYBSP_WIFI_SDIO_D0_config;
extern const cy_stc_gpio_pin_config_t CYBSP_WIFI_SDIO_D1_config;
extern const cy_stc_gpio_pin_config_t CYBSP_WIFI_SDIO_D2_config;
extern const cy_stc_gpio_pin_config_t CYBSP_WIFI_SDIO_D3_config;
extern const cy_stc_gpio_pin_config_t CYBSP_WIFI_SDIO_CMD_config;
extern const cy_stc_gpio_pin_config_t CYBSP_WIFI_SDIO_CLK_config;
extern const cy_stc_gpio_pin_config_t CYBSP_WIFI_WL_REG_ON_config;
extern const cy_stc_gpio_pin_config_t CYBSP_WIFI_HOST_WAKE_config;
extern const cy_stc_gpio_pin_config_t CYBSP_BT_UART_RX_config;
extern const cy_stc_gpio_pin_config_t CYBSP_BT_UART_TX_config;
extern const cy_stc_gpio_pin_config_t CYBSP_BT_UART_RTS_config;
extern const cy_stc_gpio_pin_config_t CYBSP_BT_UART_CTS_config;
extern const cy_stc_gpio_pin_config_t CYBSP_BT_POWER_config;
extern const cy_stc_gpio_pin_config_t CYBSP_BT_HOST_WAKE_config;
extern const cy_stc_gpio_pin_config_t CYBSP_BT_DEVICE_WAKE_config;
extern const cy_stc_gpio_pin_config_t CYBSP_UART_RX_config;
extern const cy_stc_gpio_pin_config_t CYBSP_UART_TX_config;
extern const cy_stc_gpio_pin_config_t CYBSP_I2C_SCL_config;
extern const cy_stc_gpio_pin_config_t CYBSP_I2C_SDA_config;
extern const cy_stc_gpio_pin_config_t CYBSP_SWO_config;
extern const cy_stc_gpio_pin_config_t CYBSP_SWDIO_config;
extern const cy_stc_gpio_pin_config_t CYBSP_SWDCK_config;
extern const cy_stc_gpio_pin_config_t CYBSP_CINA_config;
extern const cy_stc_gpio_pin_config_t CYBSP_CINB_config;
extern const cy_stc_gpio_pin_config_t CYBSP_CMOD_config;
extern const cy_stc_gpio_pin_config_t CYBSP_CSD_BTN0_config;
extern const cy_stc_gpio_pin_config_t CYBSP_CSD_BTN1_config;
extern const cy_stc_gpio_pin_config_t CYBSP_CSD_SLD0_config;
extern const cy_stc_gpio_pin_config_t CYBSP_CSD_SLD1_config;
extern const cy_stc_gpio_pin_config_t CYBSP_CSD_SLD2_config;
extern const cy_stc_gpio_pin_config_t CYBSP_CSD_SLD3_config;
extern const cy_stc_gpio_pin_config_t CYBSP_CSD_SLD4_config;

void init_cycfg_pins(void);

#if defined(__cplusplus)
}
#endif


#endif /* CYCFG_PINS_H */
