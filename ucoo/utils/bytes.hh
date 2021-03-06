#ifndef ucoo_utils_bytes_hh
#define ucoo_utils_bytes_hh
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

namespace ucoo {

/// Pack two bytes to one 16 bit word.
static inline uint16_t
bytes_pack (uint8_t b1, uint8_t b0)
{
    return b1 << 8 | b0;
}

/// Pack four bytes to one 32 bit word.
static inline uint32_t
bytes_pack (uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0)
{
    return b3 << 24 | b2 << 16 | b1 << 8 | b0;
}

/// Pack a number of bytes to one word of any size, little endian.
template<typename T>
static inline T
bytes_pack (const uint8_t *b)
{
    struct pack_struct
    {
        T value;
    } __attribute__ ((packed));
    return reinterpret_cast<const pack_struct *> (b)->value;
}

/// Pack a number of bytes to one word of any size, little endian, char
/// version.
template<typename T>
static inline T
bytes_pack (const char *b)
{
    return bytes_pack<T> (reinterpret_cast<const uint8_t *> (b));
}

/// Unpack one byte from one 32 bit word.
static inline uint8_t
bytes_unpack (uint32_t w, int n)
{
    return (w >> (n * 8)) & 0xff;
}

/// Unpack a word of any size to a number of bytes, little endian.
template<typename T>
static inline void
bytes_unpack (uint8_t *b, T value)
{
    struct pack_struct
    {
        T value;
    } __attribute__ ((packed, aligned(1)));
    reinterpret_cast<pack_struct *> (b)->value = value;
}

/// Unpack a word of any size to a number of bytes, little endian, char
/// version
template<typename T>
static inline void
bytes_unpack (char *b, T value)
{
    bytes_unpack (reinterpret_cast<uint8_t *> (b), value);
}

} // namespace ucoo

#endif // ucoo_utils_bytes_hh
