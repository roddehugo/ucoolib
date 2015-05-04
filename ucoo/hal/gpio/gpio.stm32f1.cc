// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2015 Nicolas Schodet
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
#include "gpio.stm32f1.hh"

namespace ucoo {

void
Gpio::set ()
{
    GPIO_BSRR (port_) = mask_;
}

void
Gpio::reset ()
{
    GPIO_BRR (port_) = mask_;
}

/// Helper to avoid virtual dance.
static inline void
Gpio_set (uint32_t port, uint16_t mask, bool state)
{
    if (state)
        GPIO_BSRR (port) = mask;
    else
        GPIO_BRR (port) = mask;
}

void
Gpio::set (bool state)
{
    Gpio_set (port_, mask_, state);
}

void
Gpio::toggle ()
{
    // Avoid read/modify/write ODR, to achieve atomic operation.
    Gpio_set (port_, mask_, !(GPIO_ODR (port_) & mask_));
}

bool
Gpio::get () const
{
    return GPIO_IDR (port_) & mask_;
}

/// Set four bits in a register for the corresponding one-bit mask.
static uint32_t
qmask_set (uint32_t mask, uint32_t reg, uint32_t bits)
{
    uint32_t dmask = mask * mask;
    uint32_t qmask = dmask * dmask;
    uint32_t qmask2 = qmask | qmask << 1;
    reg &= ~(qmask2 | qmask2 << 2);
    reg |= bits * qmask;
    return reg;
}

void
Gpio::input ()
{
    uint32_t cnf_mode = static_cast<uint8_t> (input_cnf_) | GPIO_MODE_INPUT;
    if (mask_ & 0xff)
        GPIO_CRL (port_) = qmask_set (mask_, GPIO_CRL (port_), cnf_mode);
    else
        GPIO_CRH (port_) = qmask_set (mask_ >> 8, GPIO_CRH (port_), cnf_mode);
    output_ = false;
}

void
Gpio::output ()
{
    uint32_t cnf_mode = static_cast<uint8_t> (output_cnf_)
        | static_cast<uint8_t> (speed_);
    if (mask_ & 0xff)
        GPIO_CRL (port_) = qmask_set (mask_, GPIO_CRL (port_), cnf_mode);
    else
        GPIO_CRH (port_) = qmask_set (mask_ >> 8, GPIO_CRH (port_), cnf_mode);
    output_ = true;
}

void
Gpio::input_cnf (InputCnf cnf)
{
    input_cnf_ = cnf;
    if (!output_)
        input ();
}

void
Gpio::output_cnf (OutputCnf cnf)
{
    output_cnf_ = cnf;
    if (output_)
        output ();
}

void
Gpio::speed (Speed s)
{
    speed_ = s;
    if (output_)
        output ();
}

} // namespace ucoo
