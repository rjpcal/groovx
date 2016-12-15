/** @file tcl/channelbuf.cc bridge class between tcl channels and c++
    streams */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Jun 30 17:01:22 2004
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

#include "tcl/channelbuf.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"
#include "rutz/stream_buffer.h"

#include <memory>

#include "rutz/trace.h"

namespace tcl
{
  /// A std::streambuf implementation that wraps a tcl channel.
  class channel_buf : public rutz::stream_buffer
  {
  private:
    bool owned;
    Tcl_Interp* m_interp;
    Tcl_Channel chan;

  public:
    channel_buf(Tcl_Interp* interp, const char* channame, std::ios::openmode om);
    ~channel_buf();

    channel_buf(const channel_buf&) = delete;
    channel_buf& operator=(const channel_buf&) = delete;

    virtual ssize_t do_read(char* mem, size_t n) override;
    virtual ssize_t do_write(const char* mem, size_t n) override;
  };
}

tcl::channel_buf::channel_buf(Tcl_Interp* interp, const char* channame,
                              std::ios::openmode om)
  :
  rutz::stream_buffer(om),
  owned(false), m_interp(interp), chan(nullptr)
{
  int origmode = 0;
  chan = Tcl_GetChannel(interp, channame, &origmode);
  if (chan == nullptr)
    {
      throw rutz::error(rutz::sfmt("no channel named '%s'",
                                   channame), SRC_POS);
    }

  if (om & std::ios::in)
    if (!(origmode & TCL_READABLE))
      throw rutz::error(rutz::sfmt("tcl channel '%s' is not readable",
                                   channame), SRC_POS);
  if (om & std::ios::out)
    if (!(origmode & TCL_WRITABLE))
      throw rutz::error(rutz::sfmt("tcl channel '%s' is not writable",
                                   channame), SRC_POS);
}

tcl::channel_buf::~channel_buf()
{
  this->sync();
  if (owned)
    Tcl_Close(m_interp, chan);
}

ssize_t tcl::channel_buf::do_read(char* mem, size_t n)
{
GVX_TRACE("tcl::channel_buf::do_read");

  GVX_ASSERT(n < std::numeric_limits<int>::max());
  return Tcl_Read(chan, mem, int(n));
}

ssize_t tcl::channel_buf::do_write(const char* mem, size_t n)
{
GVX_TRACE("tcl::channel_buf::do_write");

  GVX_ASSERT(n < std::numeric_limits<int>::max());

  if ( Tcl_Write(chan, mem, int(n)) != int(n) )
    {
      return EOF;
    }
  return ssize_t(n);
}

namespace
{
  class tcl_stream : public std::iostream
  {
  private:
    tcl::channel_buf m_buf;
  public:
    tcl_stream(Tcl_Interp* interp,
               const char* channame, std::ios::openmode mode)
      :
      std::iostream(0),
      m_buf(interp, channame, mode)
    {
      rdbuf(&m_buf);
    }
  };
}

using std::unique_ptr;

unique_ptr<std::ostream> tcl::ochanopen(Tcl_Interp* interp,
                                        const char* channame,
                                        std::ios::openmode flags)
{
  return std::make_unique<tcl_stream>(interp, channame, std::ios::out|flags);
}

unique_ptr<std::istream> tcl::ichanopen(Tcl_Interp* interp,
                                        const char* channame,
                                        std::ios::openmode flags)
{
  return std::make_unique<tcl_stream>(interp, channame, std::ios::in|flags);
}

