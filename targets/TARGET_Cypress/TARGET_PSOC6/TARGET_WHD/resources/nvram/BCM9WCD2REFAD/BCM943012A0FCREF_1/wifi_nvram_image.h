/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *  NVRAM variables taken from bcm943012fcbga_hwoob.txt nvram file (2.4 GHz, 20 MHz BW mode)
 */

#ifndef INCLUDED_NVRAM_IMAGE_H_
#define INCLUDED_NVRAM_IMAGE_H_

#include <string.h>
#include <stdint.h>
#ifdef MODUSTOOLBOX
#include "generated_mac_address.txt"
#else
#include "../generated_mac_address.txt"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Character array of NVRAM image
 */

static const char wifi_nvram_image[] =
        // # Sample variables file for BCM943012 BU board
        "NVRAMRev=$Rev: 351687 $"                                           "\x00"
        "sromrev=11"                                                        "\x00"
        "cckdigfilttype=4"                                                  "\x00"
        "bphyscale=0x28"                                                    "\x00"
        "boardflags3=0x00000100"                                            "\x00"
        "vendid=0x14e4"                                                     "\x00"
        "devid=0xA804"                                                      "\x00"
        "manfid=0x2d0"                                                      "\x00"
        "prodid=0x052e"                                                     "\x00"
        NVRAM_GENERATED_MAC_ADDRESS                                         "\x00"
        "nocrc=1"                                                           "\x00"
        "muxenab=0x1"                                                       "\x00"
        "boardtype=0x07d4"                                                  "\x00"
        "boardrev=0x1101"                                                   "\x00"
        "lpflags=0x00000000"                                                "\x00"
        "xtalfreq=37400"                                                    "\x00"
        "boardflags2=0xc0000000"                                            "\x00"
        "boardflags=0x00000000"                                             "\x00"
        "extpagain2g=1"                                                     "\x00"
        "extpagain5g=1"                                                     "\x00"
        "ccode=0"                                                           "\x00"
        "regrev=0"                                                          "\x00"
        "antswitch=0"                                                       "\x00"
        "rxgains2gelnagaina0=0"                                             "\x00"
        "rxgains2gtrisoa0=15"                                               "\x00"
        "rxgains2gtrelnabypa0=0"                                            "\x00"
        "rxgains5gelnagaina0=0"                                             "\x00"
        "rxgains5gtrisoa0=9"                                                "\x00"
        "rxgains5gtrelnabypa0=0"                                            "\x00"
        "pdgain5g=0"                                                        "\x00"
        "pdgain2g=0"                                                        "\x00"
        "tworangetssi2g=0"                                                  "\x00"
        "tworangetssi5g=0"                                                  "\x00"
        "rxchain=1"                                                         "\x00"
        "txchain=1"                                                         "\x00"
        "aa2g=1"                                                            "\x00"
        "aa5g=1"                                                            "\x00"
        "tssipos5g=0"                                                       "\x00"
        "tssipos2g=0"                                                       "\x00"
        "tssisleep_en=0x5"                                                  "\x00"
        "femctrl=17"                                                        "\x00"
        "subband5gver=4"                                                    "\x00"
        "pa2ga0=-208,5895,-738"                                             "\x00"
        "pa5ga0=-1,7463,-1006,-9,7547,-1001,-26,7677,-1002,-32,8146,-1049"  "\x00"
        "cckpwroffset0=3"                                                   "\x00"
        "pdoffset40ma0=0"                                                   "\x00"
        "pdoffset80ma0=0"                                                   "\x00"
        "lowpowerrange2g=0"                                                 "\x00"
        "lowpowerrange5g=0"                                                 "\x00"
        "ed_thresh2g=-63"                                                   "\x00"
        "ed_thresh5g=-63"                                                   "\x00"
        "swctrlmap_2g=0x00000002,0x00000001,0x0000001,0x080008,0x3ff"       "\x00"
        "swctrlmapext_2g=0x00000000,0x00000000,0x00000000,0x100010,0x003"   "\x00"
        "swctrlmap_5g=0x00000004,0x00000001,0x00000001,0x190019,0x3ff"      "\x00"
        "swctrlmapext_5g=0x00000000,0x00000000,0x00000000,0x110011,0x003"   "\x00"
        "ulpnap=0"                                                          "\x00"
        "ssagc_en=0"                                                        "\x00"
        "ds1_nap=0"                                                         "\x00"
        "epacal2g=1"                                                        "\x00"
        "epacal2g_mask=0x3fff"                                              "\x00"
        "maxp2ga0=72"                                                       "\x00"
        "ofdmlrbw202gpo=0x0011"                                             "\x00"
        "dot11agofdmhrbw202gpo=0x7744"                                      "\x00"
        "mcsbw202gpo=0x99777441"                                            "\x00"
        "rssicorrnorm_c0=-6"                                                "\x00"
        "rssicorrnorm5g_c0=-5,0,0,-4,0,0,-4,0,0,-6,0,0"                     "\x00"
        "mac_clkgating=1"                                                   "\x00"
        //#mcsbw402gpo=0x99555533
        "maxp5ga0=76,76,76,76"                                              "\x00"
        "mcsbw205glpo=0x99777441"                                           "\x00"
        "mcsbw205gmpo=0x99777441"                                           "\x00"
        "mcsbw205ghpo=0x99777441"                                           "\x00"
        //#mcsbw405glpo=0x99555000
        //#mcsbw405gmpo=0x99555000
        //#mcsbw405ghpo=0x99555000
        //#mcsbw805glpo=0x99555000
        //#mcsbw805gmpo=0x99555000
        //#mcsbw805ghpo=0x99555000
        "txwbpapden=1"                                                      "\x00"

        //#tx papd cal params
        "wb_rxattn=0x0101"                                                  "\x00"
        "wb_txattn=0x0404"                                                  "\x00"
        "wb_papdcalidx=0x1414"                                              "\x00"
        "wb_eps_offset=0x01c601c6"                                          "\x00"
        "wb_bbmult=0x4040"                                                  "\x00"
        "wb_calref_db=0x1a1a"                                               "\x00"
        "wb_tia_gain_mode=0x0606"                                           "\x00"

        "nb_rxattn=0x0808"                                                  "\x00"
        "nb_txattn=0x0404"                                                  "\x00"
        "nb_papdcalidx=0x1414"                                              "\x00"
        "nb_eps_offset=0x01d601d6"                                          "\x00"
        "nb_bbmult=0x5A5A"                                                  "\x00"
        "nb_tia_gain_mode=0x0606"                                           "\x00"
        "\x00\x00";


#ifdef __cplusplus
} /* extern "C" */
#endif

#else /* ifndef INCLUDED_NVRAM_IMAGE_H_ */

#error Wi-Fi NVRAM image included twice

#endif /* ifndef INCLUDED_NVRAM_IMAGE_H_ */
