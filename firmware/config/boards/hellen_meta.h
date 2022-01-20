/*
 * hellen_meta.h
 *
 * @date Oct 2, 2021
 * @author Andrey Belomutskiy, (c) 2012-2021
 */

#pragma once

void hellenWbo();
void setHellenDefaultVrThresholds();

void setHellen144LedPins();
void setHellen176LedPins();

void detectHellenBoardType();

// stm32 UART8
#define H144_UART1_RX GPIOE_0
#define H144_UART1_TX GPIOE_1

// stm32 UART2
#define H144_UART2_RX GPIOD_6
#define H144_UART2_TX GPIOD_5

#define H144_LED1_RED GPIOG_0
#define H176_LED1_RED GPIOH_8

#define H144_LS_1 GPIOG_7
#define H144_LS_2 GPIOG_8
#define H144_LS_3 GPIOD_11
#define H144_LS_4 GPIOD_10
#define H144_LS_5 GPIOD_9
#define H144_LS_6 GPIOF_12
#define H144_LS_7 GPIOF_13
#define H144_LS_8 GPIOF_14

#define H144_IGN_1 GPIOC_13
#define H144_IGN_2 GPIOE_5
#define H144_IGN_3 GPIOE_4
#define H144_IGN_4 GPIOE_3
#define H144_IGN_5 GPIOE_2
#define H144_IGN_6 GPIOB_8

#define H144_OUT_PWM1 GPIOD_13
#define H144_OUT_PWM2 GPIOC_6
#define H144_OUT_PWM3 GPIOC_7
#define H144_OUT_PWM4 GPIOC_8
#define H144_OUT_PWM5 GPIOC_9
#define H144_OUT_PWM6 GPIOD_14
#define H144_OUT_PWM7 GPIOD_15
#define H144_OUT_PWM8 GPIOD_12


#define H144_OUT_IO1 GPIOD_3
#define H144_OUT_IO2 GPIOA_9
#define H144_OUT_IO3 GPIOG_14
#define H144_OUT_IO4 GPIOG_5
#define H144_OUT_IO5 GPIOD_2
#define H144_OUT_IO6 GPIOG_11
#define H144_OUT_IO7 GPIOG_3
#define H144_OUT_IO8 GPIOG_4
#define H144_OUT_IO9 GPIOG_13
#define H144_OUT_IO10 GPIOG_12
#define H144_OUT_IO11 GPIOG_2
#define H144_OUT_IO12 GPIOA_8
#define H144_OUT_IO13 GPIOG_6

// A24
#define H144_IN_CRANK GPIOB_1
// A19
#define H144_IN_CAM GPIOA_6
// DIN5
#define H144_IN_VSS GPIOF_11

// IN_CLT AIN11 PC2
#define H144_IN_CLT EFI_ADC_12
// IN_IAT AIN14 PC3
#define H144_IN_IAT EFI_ADC_13

// IN_PPS AIN18 PA3
#define H144_IN_PPS EFI_ADC_3
// IN_TPS AIN17 PA4
#define H144_IN_TPS EFI_ADC_4
// VIGN PA5
#define H144_IN_VBATT EFI_ADC_5

// IN_MAP1 AIN9 PC0
#define H144_IN_MAP1 EFI_ADC_10
// AIN10 PC1
#define H144_IN_MAP2 EFI_ADC_11
// AIN15 A15 PA2
#define H144_IN_MAP3 EFI_ADC_2

// AIN7
#define H144_IN_RES1 GPIOF_9

// AIN16
#define H144_IN_RES2 GPIOF_10

// AIN8
#define H144_IN_RES3 GPIOF_8


#define H144_IN_AUX1 EFI_ADC_8
//
#define H144_IN_AUX2 EFI_ADC_14

// AIN22 PC5
#define H144_IN_AUX3 EFI_ADC_15
// AIN20 PA7
#define H144_IN_AUX4 EFI_ADC_7

// A1 AIN1 no code to use ADC3 as analog
#define H144_IN_SENS1 GPIOF_3
// A2 AIN2 no code to use ADC3 as analog
#define H144_IN_SENS2 GPIOF_4
// A3 AIN3 no code to use ADC3 as analog
#define H144_IN_SENS3 GPIOF_5
// A4 AIN4 no code to use ADC3 as analog
#define H144_IN_SENS4 GPIOF_6

// IN_O2S2 AIN12 PA1
#define H144_IN_O2S2 EFI_ADC_1

#define H144_IN_D_1 GPIOE_12
#define H144_IN_D_2 GPIOE_13
#define H144_IN_D_3 GPIOE_14
#define H144_IN_D_4 GPIOE_15

// A22
#define H144_IN_D_AUX3 GPIOC_5
// A20 AIN20
#define H144_IN_D_AUX4 GPIOA_7

#define H176_LS_1 GPIOG_7
#define H176_LS_2 GPIOG_8
#define H176_LS_3 GPIOD_11
#define H176_LS_4 GPIOD_10
#define H176_LS_5 GPIOD_9
#define H176_LS_6 GPIOF_12
#define H176_LS_7 GPIOF_13
#define H176_LS_8 GPIOF_14

#define H176_OUT_PWM1 GPIOD_13
#define H176_OUT_PWM2 GPIOC_6
#define H176_OUT_PWM3 GPIOC_7

#define H176_OUT_IO6 GPIOH_15
#define H176_OUT_IO9 GPIOI_1
#define H176_OUT_IO10 GPIOI_0

// same pins 144 and 176
#define H176_CAN_RX GPIOD_0
#define H176_CAN_TX GPIOD_1

#define H176_IN_A16 GPIOF_10


// looks like same pins 144 and 176
#define H_SPI2_MOSI GPIOB_15
#define H_SPI2_MISO GPIOB_14
#define H_SPI2_SCK GPIOB_13
#define H_SPI2_CS GPIOB_12
