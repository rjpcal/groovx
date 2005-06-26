///////////////////////////////////////////////////////////////////////
//
// outputfile.h
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri May 23 10:01:57 2003
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

#ifndef OUTPUTFILE_H_DEFINED
#define OUTPUTFILE_H_DEFINED

#include "io/io.h"

#include "rutz/fstring.h"
#include "rutz/sharedptr.h"

#include <iosfwd>

/// Wraps a std::ofstream in an IoObject.
/** This allows a named file to be serialized across program runs, as well
    as shared among objects within a single program run. */
class OutputFile : public IO::IoObject
{
protected:
  OutputFile();

  virtual ~OutputFile() throw();

public:
  /// Default creator.
  static OutputFile* make() { return new OutputFile; }

  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  /// Get the name of the file that will be written to.
  rutz::fstring getFilename() const;

  /// Set the name of the file that will be written to.
  void setFilename(rutz::fstring fname);

  /// Check whether this object currently has a valid, writable stream.
  bool hasStream() const;

  /// Get the ostream associated with this object.
  std::ostream& stream();

private:
  rutz::fstring itsFilename;
  rutz::shared_ptr<std::ostream> itsStream;
};

static const char vcid_outputfile_h[] = "$Id$ $URL$";
#endif // !OUTPUTFILE_H_DEFINED
