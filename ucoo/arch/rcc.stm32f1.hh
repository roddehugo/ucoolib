#ifndef ucoo_arch_rcc_stm32f1_hh
#define ucoo_arch_rcc_stm32f1_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2016 Nicolas Schodet
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// }}}
#include "ucoo/arch/arch.hh"
#include "ucoo/arch/reg.hh"

namespace ucoo {

enum class Bus
{
    AHB = 0,
    APB1 = 2,
    APB2 = 1,
};

namespace details {

constexpr uint32_t
rcc_enum (Bus bus, uint32_t bit, int pos = 0)
{
    return bit == 1
        ? (static_cast<uint32_t> (bus) << 5) | pos
        : rcc_enum (bus, (bit >> 1) | (bit << 31), pos + 1);
}

} // namespace details

/// Constants to handle reset and clock for each peripheral.
enum class Rcc
{
    DMA1 = details::rcc_enum (Bus::AHB, RCC_AHBENR_DMA1EN),
    DMA2 = details::rcc_enum (Bus::AHB, RCC_AHBENR_DMA2EN),
    SRAM = details::rcc_enum (Bus::AHB, RCC_AHBENR_SRAMEN),
    FLITF = details::rcc_enum (Bus::AHB, RCC_AHBENR_FLITFEN),
    CRC = details::rcc_enum (Bus::AHB, RCC_AHBENR_CRCEN),
    OTGFS = details::rcc_enum (Bus::AHB, RCC_AHBENR_OTGFSEN),
    AFIO = details::rcc_enum (Bus::APB2, RCC_APB2ENR_AFIOEN),
    GPIOA = details::rcc_enum (Bus::APB2, RCC_APB2ENR_IOPAEN),
    GPIOB = details::rcc_enum (Bus::APB2, RCC_APB2ENR_IOPBEN),
    GPIOC = details::rcc_enum (Bus::APB2, RCC_APB2ENR_IOPCEN),
    GPIOD = details::rcc_enum (Bus::APB2, RCC_APB2ENR_IOPDEN),
    GPIOE = details::rcc_enum (Bus::APB2, RCC_APB2ENR_IOPEEN),
    ADC1 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_ADC1EN),
    ADC2 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_ADC2EN),
    TIM1 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_TIM1EN),
    SPI1 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_SPI1EN),
    USART1 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_USART1EN),
    TIM2 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM2EN),
    TIM3 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM3EN),
    TIM4 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM4EN),
    TIM5 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM5EN),
    TIM6 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM6EN),
    TIM7 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM7EN),
    WWDG = details::rcc_enum (Bus::APB1, RCC_APB1ENR_WWDGEN),
    SPI2 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_SPI2EN),
    SPI3 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_SPI3EN),
    USART2 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_USART2EN),
    USART3 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_USART3EN),
    UART4 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_UART4EN),
    UART5 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_UART5EN),
    I2C1 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_I2C1EN),
    I2C2 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_I2C2EN),
    CAN1 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_CAN1EN),
    CAN2 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_CAN2EN),
    BKP = details::rcc_enum (Bus::APB1, RCC_APB1ENR_BKPEN),
    PWR = details::rcc_enum (Bus::APB1, RCC_APB1ENR_PWREN),
    DAC = details::rcc_enum (Bus::APB1, RCC_APB1ENR_DACEN),
};

/// Enable clock for given peripheral.
static inline void
rcc_peripheral_clock_enable (Rcc rcc)
{
    int bus_index = static_cast<uint32_t> (rcc) >> 5;
    int bit_index = static_cast<uint32_t> (rcc) & 0x1f;
    (&reg::RCC->AHBENR)[bus_index] |= 1 << bit_index;
    (&reg::RCC->AHBENR)[bus_index];
}

/// Disable clock for given peripheral.
static inline void
rcc_peripheral_clock_disable (Rcc rcc)
{
    int bus_index = static_cast<uint32_t> (rcc) >> 5;
    int bit_index = static_cast<uint32_t> (rcc) & 0x1f;
    (&reg::RCC->AHBENR)[bus_index] &= ~(1 << bit_index);
    (&reg::RCC->AHBENR)[bus_index];
}

/// Frequency of the main system clock.
extern int rcc_sys_freq_hz;

/// Frequency of AHB bus.
extern int rcc_ahb_freq_hz;

/// Frequency of APB1 bus.
extern int rcc_apb1_freq_hz;

/// Frequency of APB2 bus.
extern int rcc_apb2_freq_hz;

/// Frequency of timers on APB1 bus.
extern int rcc_apb1_timer_freq_hz;

/// Frequency of timers on APB2 bus.
extern int rcc_apb2_timer_freq_hz;

/// Frequency of the USB clock.
extern int rcc_usb_freq_hz;

/// Setup system clock using PLL, from HSE clock if not 0, else HSI clock.
/// Also setup flash access.  Only support configuration common to all F1
/// lines.
void
rcc_sys_clock_setup_pll (int sys_freq_hz, int hse_freq_hz,
                         int prediv1, int pllmul,
                         int apb1_pre, int apb2_pre, int adc_pre);

} // namespace ucoo

#endif // ucoo_arch_rcc_stm32f1_hh
