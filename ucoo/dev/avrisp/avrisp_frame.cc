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
#include "avrisp_frame.hh"

namespace ucoo {

static const uint8_t avrisp_frame_proto_start = 27;
static const uint8_t avrisp_frame_proto_token = 14;
static const uint8_t avrisp_frame_proto_answer_cksum_error = 0xb0;
static const uint8_t avrisp_frame_proto_status_cksum_error = 0xc1;

AvrIspFrame::AvrIspFrame (AvrIspProto &proto)
    : proto_ (proto), state_ (AVRISP_FRAME_STATE_START), buffer_send_index_ (-1)
{
}

void
AvrIspFrame::read_and_write (Stream &stream)
{
    // Design is inherited from byte based C interface.
    while (1)
    {
        if (buffer_send_index_ == -1)
        {
            // Read.
            int c = stream.getc ();
            if (c == -1)
                return;
            else
                accept_char (c);
        }
        else
        {
            // Write.
            int r = stream.write (reinterpret_cast<char *> (buffer_)
                                  + buffer_send_index_,
                                  buffer_len_ - buffer_send_index_);
            if (r <= 0)
                return;
            else
                buffer_send_index_ += r;
            if (buffer_send_index_ == buffer_len_)
                buffer_send_index_ = -1;
        }
    }
}

inline AvrIspFrameState
operator++ (AvrIspFrameState& state, int)
{
   const int i = static_cast<int> (state);
   state = static_cast<AvrIspFrameState> (i + 1);
   return state;
}

void
AvrIspFrame::accept_char (uint8_t c)
{
    switch (state_)
    {
    case AVRISP_FRAME_STATE_START:
        if (c == avrisp_frame_proto_start)
        {
            cksum_ = avrisp_frame_proto_start;
            state_++;
        }
        break;
    case AVRISP_FRAME_STATE_WAIT_SEQ:
        cksum_ ^= c;
        seq_ = c;
        state_++;
        break;
    case AVRISP_FRAME_STATE_WAIT_LEN_MSB:
        cksum_ ^= c;
        len_ = c << 8;
        state_++;
        break;
    case AVRISP_FRAME_STATE_WAIT_LEN_LSB:
        cksum_ ^= c;
        len_ |= c;
        buffer_len_ = 0;
        if (len_ == 0
            || len_ > (CONFIG_UCOO_DEV_AVRISP_FRAME_BUFFER_SIZE - proto_head_
                       - proto_tail_))
            state_ = AVRISP_FRAME_STATE_START;
        else
            state_++;
        break;
    case AVRISP_FRAME_STATE_WAIT_TOKEN:
        if (c == avrisp_frame_proto_token)
        {
            cksum_ ^= c;
            state_++;
        }
        else
            state_ = AVRISP_FRAME_STATE_START;
        break;
    case AVRISP_FRAME_STATE_DATA:
        cksum_ ^= c;
        buffer_[proto_head_ + buffer_len_++] = c;
        if (buffer_len_ == len_)
            state_++;
        break;
    case AVRISP_FRAME_STATE_WAIT_CKSUM:
        cksum_ ^= c;
        state_ = AVRISP_FRAME_STATE_START;
        process ();
        break;
    }
}

void
AvrIspFrame::send_frame (int len)
{
    int i = 0;
    uint8_t cksum;
    buffer_[i++] = avrisp_frame_proto_start;
    cksum = avrisp_frame_proto_start;
    buffer_[i++] = seq_;
    cksum ^= seq_;
    buffer_[i++] = len >> 8;
    cksum ^= len >> 8;
    buffer_[i++] = len & 0xff;
    cksum ^= len & 0xff;
    buffer_[i++] = avrisp_frame_proto_token;
    cksum ^= avrisp_frame_proto_token;
    for (; i < proto_head_ + len; i++)
        cksum ^= buffer_[i];
    buffer_[i] = cksum;
    buffer_len_ = proto_head_ + len + proto_tail_;
    buffer_send_index_ = 0;
}

void
AvrIspFrame::process ()
{
    if (cksum_ != 0)
    {
        /* Bad checksum. */
        buffer_[proto_head_] = avrisp_frame_proto_answer_cksum_error;
        buffer_[proto_head_ + 1] = avrisp_frame_proto_status_cksum_error;
        send_frame (2);
    }
    else
    {
        int r = proto_.accept (buffer_ + proto_head_, buffer_len_);
        if (r)
            send_frame (r);
    }
}

} // namespace ucoo
