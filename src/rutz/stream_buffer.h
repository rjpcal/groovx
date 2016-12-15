/** @file rutz/stream_buffer.h */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016-2016 Rob Peters
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Dec 14 15:07:31 2016
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_RUTZ_STREAM_BUFFER_H_UTC20161214230731_DEFINED
#define GROOVX_RUTZ_STREAM_BUFFER_H_UTC20161214230731_DEFINED

#include <cstddef>
#include <streambuf>

namespace rutz
{
  class stream_buffer;
}

/// Intermediate base class for streambuf implementations based on underlying read()/write()-like APIs
/** With help from

    http://www.mr-edd.co.uk/blog/beginners_guide_streambuf
    Josuttis, p. 678
*/
class rutz::stream_buffer : public std::streambuf
{
private:
  static const int s_buf_size = 4092;
  static const int s_pback_size = 4;
  char m_buffer[s_buf_size];
  const std::ios::openmode m_mode;

  /// Get more data from the underlying data source by calling do_read().
  /** Called when the streambuf's buffer has run out of data. */
  virtual int underflow() override final;

  /// Send more data to the underlying buffer.
  /** Called when the streambuf's buffer has become full. */
  virtual int overflow(int c) override final;

  /// Flush the current buffer contents to the underlying data sink by calling do_write().
  bool flushoutput();

protected:
  stream_buffer(std::ios::openmode om);

  std::ios::openmode openmode() const { return m_mode; }

  virtual int sync() override final;

  virtual ssize_t do_read(char* mem, size_t n) = 0;
  virtual ssize_t do_write(const char* mem, size_t n) = 0;
};

#endif // !GROOVX_RUTZ_STREAM_BUFFER_H_UTC20161214230731DEFINED
