///////////////////////////////////////////////////////////////////////
//
// stderror.h
//
// Copyright (c) 2003-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat May 24 13:00:28 2003
// commit: $Id$
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

#ifndef STDERROR_H_DEFINED
#define STDERROR_H_DEFINED

#include <typeinfo>

// Provides a common interface for throwing some exceptions with
// standardized error messages.

namespace Util
{
  void throwBadCast(const std::type_info& to, const std::type_info& from);
}

static const char vcid_stderror_h[] = "$Header$";
#endif // !STDERROR_H_DEFINED