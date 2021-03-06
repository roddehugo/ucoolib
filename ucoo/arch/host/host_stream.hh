#ifndef ucoo_arch_host_stream_hh
#define ucoo_arch_host_stream_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include "ucoo/intf/stream.hh"

namespace ucoo {

class HostStream : public Stream
{
  public:
    /// Default constructor, use stdin/stdout.
    HostStream ();
    /// PTY constructor, will make a link to PTY at the given name.
    HostStream (const char *name);
    /// Close if needed.
    ~HostStream ();
    /// See Stream::block.
    void block (bool block = true);
    /// See Stream::read.
    int read (char *buf, int count);
    /// See Stream::write.
    int write (const char *buf, int count);
    /// See Stream::poll.
    int poll ();
  private:
    /// Input and output file descriptors.
    int fdi_, fdo_;
};

} // namespace ucoo

#endif // ucoo_arch_host_stream_hh
