///////////////////////////////////////////////////////////////////////
//
// tclerror.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sun Jun 20 15:10:26 1999
// written: Wed Mar 19 17:58:06 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLERROR_H_DEFINED
#define TCLERROR_H_DEFINED

#include "util/error.h"

namespace Tcl
{
  class TclError;
}

/// An exception class for Tcl errors.
class Tcl::TclError : public Util::Error
{
public:
  TclError(const fstring& msg) : Util::Error(msg) {};

  virtual ~TclError();
};

static const char vcid_tclerror_h[] = "$Header$";
#endif // !TCLERROR_H_DEFINED
