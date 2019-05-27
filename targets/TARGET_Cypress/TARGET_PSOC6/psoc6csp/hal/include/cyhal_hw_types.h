/***************************************************************************//**
* \file cyhal_hw_types.h
*
* \brief
* Provides a struct definitions for configuration resources in the PDL.
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
* \addtogroup group_hal_hw_types PSoC 6 Hardware Types
* \ingroup group_hal_psoc6
* \{
* Struct definitions for configuration resources in the PDL.
*
* \defgroup group_hal_hw_types_macros Macros
* \defgroup group_hal_hw_types_functions Functions
* \defgroup group_hal_hw_types_data_structures Data Structures
* \defgroup group_hal_hw_types_enums Enumerated Types
*/

#pragma once

#include "cy_pdl.h"
#include "cyhal_hw_resources.h"
#include "cyhal_pin_package.h"
#include <stdbool.h>

#if defined(CY8C6247BZI_D54) /* TODO: BSP-525 */
    #include "SDIO_HOST.h"
#endif /* defined(CY8C6247BZI_D54) */

#ifdef __cplusplus
extern "C" {
#endif

/**
* \addtogroup group_hal_hw_types_data_structures
* \{
*/

typedef uint32_t cyhal_source_t; //!< Routable signal source
typedef uint32_t cyhal_dest_t; //!< Routable signal destination

/** Available clock divider types */
typedef cy_en_divider_types_t cyhal_clock_divider_types_t;

/** Clock divider object */
typedef struct {
    cyhal_clock_divider_types_t div_type;
    uint8_t                     div_num;
} cyhal_clock_divider_t;


/** ADC object */
typedef struct {
#ifdef CY_IP_MXS40PASS_SAR
    SAR_Type*                   base;
    cyhal_resource_inst_t       resource;
    cyhal_gpio_t                pin;
    cyhal_clock_divider_t       clock;
#endif
} cyhal_adc_t;

/** Comparator object */
typedef struct {
#if defined(CY_IP_MXLPCOMP_INSTANCES) || defined(PASS_NR_CTBS)
    /* TODO: define */
    void * TODO_define;
#else
    void *empty;
#endif
} cyhal_comp_t;

/** CRC object */
typedef struct {
#if defined(CY_IP_MXCRYPTO_INSTANCES) || defined(CPUSS_CRYPTO_PRESENT)
    CRYPTO_Type*                base;
    cyhal_resource_inst_t       resource;
    uint32_t                    crc_width;
#endif
} cyhal_crc_t;

/** DAC object */
typedef struct {
#ifdef CY_IP_MXS40PASS_CTDAC
    CTDAC_Type*                 base;
    cyhal_resource_inst_t       resource;
    cyhal_gpio_t                pin;
    cyhal_clock_divider_t       clock;
#else
    void *empty;
#endif
} cyhal_dac_t;

/** DMA object */
typedef struct {
#if defined(CY_IP_M4CPUSS_DMAC_INSTANCES) || defined(CY_IP_M4CPUSS_DMA_INSTANCES)
    cyhal_resource_inst_t     resource;
#else
    void *empty;
#endif
} cyhal_dma_t;

/** Flash object */
typedef struct {
#ifdef CY_IP_M4CPUSS
    cyhal_resource_inst_t       resource;
    uint32_t                    flashBaseAddr;
    uint32_t                    flashSize;
    uint32_t                    sectorSize;
    uint32_t                    pageSize;
    uint8_t                     eraseVal;
#else
    void *empty;
#endif /* CY_IP_M4CPUSS */
} cyhal_flash_t;

/** I2C object */
typedef struct {
#ifdef CY_IP_MXSCB
    CySCB_Type*                         base;
    cyhal_resource_inst_t               resource;
    cyhal_gpio_t                        pin_sda;
    cyhal_gpio_t                        pin_scl;
    cyhal_clock_divider_t               clock;
    cy_stc_scb_i2c_context_t            context;
    cy_stc_scb_i2c_master_xfer_config_t rx_config;
    cy_stc_scb_i2c_master_xfer_config_t tx_config;
    bool                                is_slave;
    uint32_t                            address;
    uint32_t                            irq_cause;
    uint16_t                            pending;
    uint16_t                            events;
    uint32_t                            handler;
#else
    void *empty;
#endif
} cyhal_i2c_t;

/** I2S object */
typedef struct {
#ifdef CY_IP_MXAUDIOSS_INSTANCES
    /* TODO: define */
    void * TODO_define;
#else
    void *empty;
#endif
} cyhal_i2s_t;

/** OpAmp object */
typedef struct {
#ifdef PASS_NR_CTBS
    /* TODO: define */
    void * TODO_define;
#else
    void *empty;
#endif
} cyhal_opamp_t;

/** PDM-PCM object */
typedef struct {
#ifdef CY_IP_MXAUDIOSS_INSTANCES
    /* TODO: define */
    void * TODO_define;
#else
    void *empty;
#endif
} cyhal_pdm_pcm_t;

/** PWM object */
typedef struct {
#ifdef CY_IP_MXTCPWM
    TCPWM_Type*                 base;
    cyhal_resource_inst_t       resource;
    cyhal_gpio_t                pin;
    cyhal_clock_divider_t       clock;
    uint32_t                    clock_hz;
    bool                        dedicated_clock;
#else
    void *empty;
#endif
} cyhal_pwm_t;

/** SMIF object */
typedef struct {
#ifdef CY_IP_MXSMIF
    SMIF_Type*                  base;
    cyhal_resource_inst_t       resource;   
    cyhal_gpio_t                pin_io0;
    cyhal_gpio_t                pin_io1;
    cyhal_gpio_t                pin_io2;
    cyhal_gpio_t                pin_io3;
    cyhal_gpio_t                pin_io4;
    cyhal_gpio_t                pin_io5;
    cyhal_gpio_t                pin_io6;
    cyhal_gpio_t                pin_io7;
    cyhal_gpio_t                pin_sclk;
    cyhal_gpio_t                pin_ssel;
    uint32_t                    frequency;
    uint8_t                     mode;
    cy_stc_smif_context_t       context;
    cy_en_smif_slave_select_t   slave_select;
    cy_en_smif_data_select_t    data_select;
    uint32_t                    irq_cause;
#else
    void *empty;
#endif /* ifdef CY_IP_MXSMIF */
} cyhal_qspi_t;

/** RNG object */
typedef struct {
#if defined(CY_IP_MXCRYPTO_INSTANCES) || defined(CPUSS_CRYPTO_PRESENT)
    CRYPTO_Type*                base;
    cyhal_resource_inst_t       resource;
#endif
} cyhal_trng_t;

/** RTC object */
typedef struct {
    uint8_t placeholder;
} cyhal_rtc_t;

/** SDHC object */
typedef struct {
#ifdef CY_IP_MXSDHC
    SDHC_Type*                  base;
    cyhal_resource_inst_t       resource;
    bool                        emmc;
    cy_en_sd_host_dma_type_t    dmaType;
    bool                        enableLedControl;
    cy_stc_sd_host_context_t    context;
    cyhal_gpio_t                pin_clk;
    cyhal_gpio_t                pin_cmd;
    cyhal_gpio_t                pin_data0;
    cyhal_gpio_t                pin_data1;
    cyhal_gpio_t                pin_data2;
    cyhal_gpio_t                pin_data3;
    cyhal_gpio_t                pin_data4;
    cyhal_gpio_t                pin_data5;
    cyhal_gpio_t                pin_data6;
    cyhal_gpio_t                pin_data7;
    cyhal_gpio_t                pin_cardDetect;
    cyhal_gpio_t                pin_ioVoltSel;
    cyhal_gpio_t                pin_cardIfPwrEn;
    cyhal_gpio_t                pin_cardMechWriteProt;
    cyhal_gpio_t                pin_ledCtrl;
    cyhal_gpio_t                pin_cardEmmcReset;
#else
    void *empty;
#endif
} cyhal_sdhc_t;

/** SDIO object */
typedef struct {
#if defined(CY_IP_MXSDHC)
    SDHC_Type*                  base;
    cyhal_resource_inst_t       resource;
    bool                        emmc;
    cy_en_sd_host_dma_type_t    dmaType;
    cy_stc_sd_host_context_t    context;
    cyhal_gpio_t                pin_clk;
    cyhal_gpio_t                pin_cmd;
    cyhal_gpio_t                pin_data0;
    cyhal_gpio_t                pin_data1;
    cyhal_gpio_t                pin_data2;
    cyhal_gpio_t                pin_data3;
    uint32_t                    frequencyhal_hz;
    uint16_t                    block_size;
    uint32_t                    irq_cause;

#elif defined(CY8C6247BZI_D54) /* TODO: BSP-525 */
    cyhal_resource_inst_t       resource;
    cyhal_gpio_t                pin_clk;
    cyhal_gpio_t                pin_cmd;
    cyhal_gpio_t                pin_data0;
    cyhal_gpio_t                pin_data1;
    cyhal_gpio_t                pin_data2;
    cyhal_gpio_t                pin_data3;

    cyhal_clock_divider_t       clock;

    cyhal_dma_t                 dma0Ch0;
    cyhal_dma_t                 dma0Ch1;
    cyhal_dma_t                 dma1Ch1;
    cyhal_dma_t                 dma1Ch3;
    
    uint32_t                    frequencyhal_hz;
    uint16_t                    block_size;
    
    stc_sdio_irq_cb_t*          pfuCb;
    uint32_t                    irq_cause;
#else
    void *empty;
#endif /* defined(CY_IP_MXSDHC) */
} cyhal_sdio_t;

/** SPI object */
typedef struct {
#ifdef CY_IP_MXSCB
    CySCB_Type*                 base;
    cyhal_resource_inst_t       resource;
    cyhal_gpio_t                pin_miso;
    cyhal_gpio_t                pin_mosi;
    cyhal_gpio_t                pin_sclk;
    cyhal_gpio_t                pin_ssel;
    cyhal_clock_divider_t       clock;
    cy_en_scb_spi_mode_t        ms_mode;
    cy_en_scb_spi_sclk_mode_t   clk_mode;
    uint8_t                     data_bits;
    cy_stc_scb_spi_context_t    context;
    uint32_t                    irq_cause;
    uint16_t                    pending;
    void                        *rx_buffer;
    uint32_t                    rx_buffer_size;
    void                        *tx_buffer;
    uint32_t                    tx_buffer_size;
#else
    void *empty;
#endif
} cyhal_spi_t;

/** Timer object */
typedef struct {
#ifdef CY_IP_MXTCPWM
    TCPWM_Type*                 base;
    cyhal_resource_inst_t       resource;
    cyhal_gpio_t                pin;
    cyhal_clock_divider_t       clock;
    uint32_t                    clock_hz;
    uint8_t                     direction;
    bool                        is_continuous;
    bool                        is_compare;
#else
    void *empty;
#endif
} cyhal_timer_t;

/** UART object */
typedef struct {
#ifdef CY_IP_MXSCB
    CySCB_Type*                 base;
    cyhal_resource_inst_t       resource;
    cyhal_gpio_t                pin_rx;
    cyhal_gpio_t                pin_tx;
    cyhal_gpio_t                pin_cts;
    cyhal_gpio_t                pin_rts;
    cyhal_clock_divider_t       clock;
    cy_stc_scb_uart_context_t   context;
    uint32_t                    irq_cause;
    cy_stc_syspm_callback_params_t pm_params;
    cy_stc_syspm_callback_t     pm_callback;
    en_hsiom_sel_t              saved_tx_hsiom;
    en_hsiom_sel_t              saved_rts_hsiom;
#else
    void *empty;
#endif
} cyhal_uart_t;

/** WDT object */
typedef struct {
#ifdef CY_IP_MXS40SRSS_MCWDT_INSTANCES
    MCWDT_STRUCT_Type *base;
    cyhal_resource_inst_t resource;
#else
    void *empty;
#endif
} cyhal_wdt_t;


/** USB Device object */
typedef struct {
#ifdef CY_IP_MXUSBFS
    USBFS_Type*                     base;
    cy_stc_usbfs_dev_drv_context_t* context;
    cyhal_resource_inst_t           resource;
    cyhal_gpio_t                    pin_dp;
    cyhal_gpio_t                    pin_dm;
    /* TODO: complete definition during implementation */
#else
    void *empty;
#endif
} cyhal_usb_dev_t;

/** \} group_hal_hw_types_data_structures */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/** \} group_hal_hw_types */
