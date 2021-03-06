/** @file rutz/fileposition.h SRC_POS macro wraps __FILE__ and
    __LINE__, so that it's easy to communicate source-code location
    information throughout code and ultimately back to the user */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Oct  5 19:58:48 2004
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

#ifndef GROOVX_RUTZ_FILEPOSITION_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_FILEPOSITION_H_UTC20050626084020_DEFINED

namespace rutz
{
  /// Represent a position (line number) within a source file.
  struct file_pos
  {
    file_pos(const char* file_name, int line_no) :
      m_file_name(file_name), m_line_no(line_no)
    {}

    const char* const m_file_name;
    int         const m_line_no;
  };
}

/// This macro can be used to capture the current source filename and line-number.
#define SRC_POS rutz::file_pos(__FILE__, __LINE__)

#endif // !GROOVX_RUTZ_FILEPOSITION_H_UTC20050626084020_DEFINED
