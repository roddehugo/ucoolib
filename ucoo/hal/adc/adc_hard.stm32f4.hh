#ifndef ucoo_hal_adc_adc_hard_stm32f4_hh
#define ucoo_hal_adc_adc_hard_stm32f4_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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
#include "ucoo/intf/adc.hh"
#include "ucoo/common.hh"

#include "ucoo/arch/reg.hh"
#include "ucoo/arch/rcc.stm32.hh"

namespace ucoo {

class AdcHard;

/// Single ADC channel.
class AdcHardChannel : public Adc
{
  public:
    /// See Adc::read.
    inline int read ();
    /// See Adc::get_resolution.
    inline int get_resolution () const;
  private:
    /// Constructor.
    AdcHardChannel (AdcHard &adc, int channel)
        : adc_ (adc), channel_ (channel) { }
    friend AdcHard;
  private:
    AdcHard &adc_;
    int channel_;
};

/// ADC interface.  This control a full ADC, use AdcHardChannel for a single
/// channel.
class AdcHard
{
  public:
    static const int resolution = 1 << 12;
    /// Available ADC.
    enum class Instance
    {
        ADC1,
        ADC2,
        ADC3,
    };
  public:
    /// Constructor for an ADC instance.
    AdcHard (Instance inst);
    /// Shutdown.
    ~AdcHard ();
    /// Enable, power on.
    void enable ();
    /// Disable.
    void disable ();
    /// Make a single measure.
    int read (int channel);
    /// Return an ADC channel.
    AdcHardChannel operator[] (int channel)
        { return AdcHardChannel (*this, channel); }
  private:
    /// ADC base address.
    ADC_TypeDef * const base_;
    /// ADC RCC identifier.
    const Rcc rcc_;
};

inline int
AdcHardChannel::read ()
{
    return adc_.read (channel_);
}

inline int
AdcHardChannel::get_resolution () const
{
    return AdcHard::resolution;
}

} // namespace ucoo

#endif // ucoo_hal_adc_adc_hard_stm32f4_hh
