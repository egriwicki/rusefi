#pragma once

#define MM64_OUT_PWM1 B7
#define MM64_OUT_PWM2 C8
#define MM64_OUT_PWM3 C9
#define MM64_OUT_PWM4 A8
#define MM64_OUT_PWM5 A9
#define MM64_OUT_PWM6 A15

#define MM64_IN_ADC0 A0
#define MM64_IN_ADC0_ANALOG EFI_ADC_0

#define MM64_IN_ADC1 A1
#define MM64_IN_ADC1_ANALOG EFI_ADC_1

#define MM64_IN_ADC2 A2
#define MM64_IN_ADC2_ANALOG EFI_ADC_2

#define MM64_IN_ADC3 A3
#define MM64_IN_ADC3_ANALOG EFI_ADC_3

#define MM64_IN_ADC4 A4
#define MM64_IN_ADC4_ANALOG EFI_ADC_4

#define MM64_IN_ADC5 A5
#define MM64_IN_ADC5_ANALOG EFI_ADC_5

#define MM64_IN_ADC6 A6
#define MM64_IN_ADC6_ANALOG EFI_ADC_6

#define MM64_IN_ADC7 A7
#define MM64_IN_ADC7_ANALOG EFI_ADC_7

#define MM64_IN_ADC10 C0
#define MM64_IN_ADC10_ANALOG EFI_ADC_10

#define MM64_IN_ADC11 C1
#define MM64_IN_ADC11_ANALOG EFI_ADC_11

#define MM64_IN_ADC14 C4
#define MM64_IN_ADC14_ANALOG EFI_ADC_14

#define MM64_IN_ADC15 C5
#define MM64_IN_ADC15_ANALOG EFI_ADC_15

#define MM64_SPI3_MOSI C12
#define MM64_SPI3_MISO B4
#define MM64_SPI3_SCK B3
#define MM64_SPI3_CS D2

#define MM64_SPI2_MOSI B15
#define MM64_SPI2_MISO B14
#define MM64_SPI2_SCK B13
#define MM64_SPI2_CS B12

#define MM64_CAN_RX B8
#define MM64_CAN_TX B9

#define MM64_CAN2_RX B5
#define MM64_CAN2_TX B6

#define MM64_UART6_RX C7
#define MM64_UART6_TX C6

#define MM64_UART3_RX C11
#define MM64_UART3_TX C10

#define MM64_USB1ID A10

//GP1 = PWR_EN
#define MM64_GP1 C13

#define MM64_SPI3_CS D2

// todo: remove H64 usages!
#define H64_LED1_RED Gpio::B0
#define H64_LED2_BLUE Gpio::B1

#define MM64_LED1_RED B0
#define MM64_LED2_BLUE B1

#define MM64_BOOT1 B2

#define MM64_BOARD_ID1 C2
#define MM64_BOARD_ID2 C3