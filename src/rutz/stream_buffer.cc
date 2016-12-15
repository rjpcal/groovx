/** @file rutz/stream_buffer.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016-2016 Rob Peters
// Rob Peters <rjpeters at gmail dot com>
//
// created: Wed Dec 14 15:08:17 2016
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#include "rutz/stream_buffer.h"

#include "rutz/trace.h"
#include "rutz/debug.h"

rutz::stream_buffer::stream_buffer(std::ios::openmode om)
  :
  m_mode(om)
{
  if (om & std::ios::in)
    setg(m_buffer+s_pback_size,
         m_buffer+s_pback_size,
         m_buffer+s_pback_size);
  else if (om & std::ios::out)
    setp(m_buffer,
         m_buffer+(s_buf_size-1));
}

int rutz::stream_buffer::underflow()
{
GVX_TRACE("rutz::stream_buffer::underflow");
  // is read position before end of buffer?
  if (gptr() < egptr())
    return *gptr();

  off_t num_putback = 0;
  if (s_pback_size > 0 && gptr() > eback())
    {
      // process size of putback area
      // -use number of characters read
      // -but at most four
      num_putback = gptr() - eback();
      if (num_putback > 4)
        num_putback = 4;

      GVX_ASSERT(num_putback >= 0);

      // copy up to four characters previously read into the putback
      // buffer (area of first four characters)
      std::memcpy (m_buffer+(4-num_putback), gptr()-num_putback,
                   size_t(num_putback));
    }

  // read new characters
  const ssize_t num = this->do_read(m_buffer+s_pback_size,
                                    s_buf_size-s_pback_size);

  if (num <= 0) // error (0) or end-of-file (-1)
    return EOF;

  // reset buffer pointers
  setg (m_buffer+(s_pback_size-num_putback),
        m_buffer+s_pback_size,
        m_buffer+s_pback_size+num);

  // return next character Hrmph. We have to cast to unsigned char to avoid
  // problems with eof. Problem is, -1 is a valid char value to
  // return. However, without a cast, char(-1) (0xff) gets converted to
  // int(-1), which is 0xffffffff, which is EOF! What we want is
  // int(0x000000ff), which we have to get by int(unsigned char(-1)).
  return static_cast<unsigned char>(*gptr());
}

int rutz::stream_buffer::overflow(int c)
{
GVX_TRACE("rutz::stream_buffer::overflow");
  if (!(m_mode & std::ios::out)) return EOF;

  if (c != EOF)
    {
      // insert the character into the buffer
      *pptr() = char(c);
      pbump(1);
    }

  if (!flushoutput())
    return -1; // ERROR

  return c;
}

int rutz::stream_buffer::sync()
{
  return flushoutput() ? 0 : -1;
}

bool rutz::stream_buffer::flushoutput()
{
  if (!(m_mode & std::ios::out)) return EOF;

  ssize_t num = pptr() - pbase();
  GVX_ASSERT(num >= 0);
  ssize_t ret = this->do_write(pbase(), size_t(num));
  if (ret < 0)
    return false;

  GVX_ASSERT(ret < std::numeric_limits<int>::max());
  pbump(-int(ret));
  return true;
}
