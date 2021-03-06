/** @file io/attribcount.h Count the number of attributes in an io::serializable object */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Nov 11 17:15:50 2005
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

#ifndef GROOVX_IO_ATTRIBCOUNT_H_UTC20051112011550_DEFINED
#define GROOVX_IO_ATTRIBCOUNT_H_UTC20051112011550_DEFINED

namespace io
{
  class serializable;

  /** Returns the number of attributes that are written in the
      object's write_to() function. This implementation simply calls
      write_to() with a dummy writer and counts how many attributes
      are written. */
  unsigned int attrib_count(const io::serializable& obj);
}

#endif // !GROOVX_IO_ATTRIBCOUNT_H_UTC20051112011550DEFINED
