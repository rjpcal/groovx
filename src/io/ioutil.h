///////////////////////////////////////////////////////////////////////
//
// ioutil.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 11 21:43:43 1999
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

#ifndef IOUTIL_H_DEFINED
#define IOUTIL_H_DEFINED

class fstring;

namespace Util
{
  template <class T> class Ref;
}

namespace IO
{
  class IoObject;

  // LGX -- "Legacy" compact format
  fstring  writeLGX(Util::Ref<IO::IoObject> obj);
  void     readLGX(Util::Ref<IO::IoObject> obj, const char* buf);

  // ASW -- AsciiStreamWriter format
  fstring  writeASW(Util::Ref<IO::IoObject> obj);
  void     readASW(Util::Ref<IO::IoObject> obj, const char* buf);

  void saveASW(Util::Ref<IO::IoObject> obj, fstring filename);
  void loadASW(Util::Ref<IO::IoObject> obj, fstring filename);

  Util::Ref<IO::IoObject> retrieveASW(fstring filename);

  // GVX -- GroovX XML format
  fstring  writeGVX(Util::Ref<IO::IoObject> obj);

  void saveGVX(Util::Ref<IO::IoObject> obj, fstring filename);
}

static const char vcid_ioutil_h[] = "$Header$";
#endif // !IOUTIL_H_DEFINED
