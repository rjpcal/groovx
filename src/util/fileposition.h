///////////////////////////////////////////////////////////////////////
//
// fileposition.h
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Oct  5 19:58:48 2004
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

#ifndef FILEPOSITION_H_DEFINED
#define FILEPOSITION_H_DEFINED

/// Represent a position (line number) within a source file.
struct FilePosition
{
  FilePosition(const char* file_name, int line_no) :
    fileName(file_name), lineNo(line_no)
  {}

  const char* const fileName;
  int         const lineNo;
};

/// This macro can be used to capture the current source filename and line-number.
#define SRC_POS FilePosition(__FILE__, __LINE__)

static const char vcid_fileposition_h[] = "$Header$";
#endif // !FILEPOSITION_H_DEFINED
