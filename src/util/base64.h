///////////////////////////////////////////////////////////////////////
//
// base64.h
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Oct  8 13:44:03 2004
// commit: $Id$
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

#ifndef BASE64_H_DEFINED
#define BASE64_H_DEFINED

#include <vector>

namespace rutz
{
  void base64_encode(const char* src,
                     unsigned int src_len,
                     std::vector<char>& dst);

  void base64_encode(const char* src_filename,
                     std::vector<char>& dst);

  void base64_decode(const char* src,
                     unsigned int in_len,
                     std::vector<char>& dst);
}

static const char vcid_base64_h[] = "$Header$";
#endif // !BASE64_H_DEFINED
