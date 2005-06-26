///////////////////////////////////////////////////////////////////////
//
// tclchannelbuf.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jun 30 17:01:08 2004
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_TCL_TCLCHANNELBUF_H_UTC20050626084018_DEFINED
#define GROOVX_TCL_TCLCHANNELBUF_H_UTC20050626084018_DEFINED

#include <iostream>

#include <tcl.h>

namespace rutz
{
  template <class T> class shared_ptr;
}

namespace Tcl
{
  /// A std::streambuf implementation that handles gzip-encoded data.
  class ChannelBuf : public std::streambuf
  {
  private:
    bool opened;
    bool owned;
    int mode;
    Tcl_Interp* itsInterp;
    Tcl_Channel chan;

    ChannelBuf(const ChannelBuf&);
    ChannelBuf& operator=(const ChannelBuf&);

    static const int bufSize = 4092;
    static const int pbackSize = 4;
    char buffer[bufSize];

    int flushoutput();

  public:
    ChannelBuf(Tcl_Interp* interp, const char* channame, int om);
    ~ChannelBuf() { close(); }

    bool is_open() { return opened; }

    void ensure_open();

    void close();

    virtual int underflow();

    virtual int overflow(int c);

    virtual int sync();
  };
}

namespace Tcl
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

static const char vcid_groovx_tcl_tclchannelbuf_h_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_TCLCHANNELBUF_H_UTC20050626084018_DEFINED
