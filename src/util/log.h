///////////////////////////////////////////////////////////////////////
//
// log.h
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jun 20 17:47:13 2001
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

#ifndef LOG_H_DEFINED
#define LOG_H_DEFINED

namespace rutz
{
  class fstring;
}

namespace Nub
{
  class Object;

  namespace Log
  {
    void reset();

    void addScope(const rutz::fstring& name);
    void removeScope(const rutz::fstring& name);

    /// Add a scope named after the given object's type + id.
    void addObjScope(const Nub::Object& obj);

    /// Remove the scope named after the given object's type + id.
    void removeObjScope(const Nub::Object& obj);

    /// Specify the name of a file to which log info should be appended.
    void setLogFilename(const rutz::fstring& filename);

    /// Specify whether to copy log output to stdout (default = yes).
    void setCopyToStdout(bool shouldcopy);
  }

  void log(const char* msg);
  void log(const rutz::fstring& msg);
}

static const char vcid_log_h[] = "$Header$";
#endif // !LOG_H_DEFINED
