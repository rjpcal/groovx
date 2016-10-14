/** @file tcl/channelbuf.h bridge class between tcl channels and c++
    streams */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jun 30 17:01:08 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_TCL_CHANNELBUF_H_UTC20050628162420_DEFINED
#define GROOVX_TCL_CHANNELBUF_H_UTC20050628162420_DEFINED

#include <iostream>
#include <ostream>
#include <streambuf>
#include <tcl.h>

namespace rutz
{
  template <class T> class shared_ptr;
}

namespace tcl
{
  /// A std::streambuf implementation that wraps a tcl channel.
  class channel_buf : public std::streambuf
  {
  private:
    bool opened;
    bool owned;
    int mode;
    Tcl_Interp* m_interp;
    Tcl_Channel chan;

    channel_buf(const channel_buf&);
    channel_buf& operator=(const channel_buf&);

    static const int s_buf_size = 4092;
    static const int s_pback_size = 4;
    char m_buffer[s_buf_size];

    int flushoutput();

  public:
    channel_buf(Tcl_Interp* interp, const char* channame, int om);
    ~channel_buf() { close(); }

    bool is_open() { return opened; }

    void ensure_open();

    void close();

    virtual int underflow();

    virtual int overflow(int c);

    virtual int sync();
  };
}

namespace tcl
{
  rutz::shared_ptr<std::ostream>
  ochanopen(Tcl_Interp* interp,
            const char* channame,
            std::ios::openmode flags = std::ios::openmode(0));

  rutz::shared_ptr<std::istream>
  ichanopen(Tcl_Interp* interp,
            const char* channame,
            std::ios::openmode flags = std::ios::openmode(0));
}

#endif // !GROOVX_TCL_CHANNELBUF_H_UTC20050628162420_DEFINED
