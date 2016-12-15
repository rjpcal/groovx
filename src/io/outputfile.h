/** @file io/outputfile.h wraps a std::ofstream in an io::serializable,
    allowing named files to be serialized and restored across multiple
    program runs */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri May 23 10:01:57 2003
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

#ifndef GROOVX_IO_OUTPUTFILE_H_UTC20050626084021_DEFINED
#define GROOVX_IO_OUTPUTFILE_H_UTC20050626084021_DEFINED

#include "io/io.h"

#include "rutz/fstring.h"

#include <iosfwd>
#include <memory>

/// Wraps a std::ofstream in an serializable.
/** This allows a named file to be serialized across program runs, as
    well as shared among objects within a single program run. */
class output_file : public io::serializable
{
protected:
  output_file();

  virtual ~output_file() noexcept;

public:
  /// Default creator.
  static output_file* make() { return new output_file; }

  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  /// Get the name of the file that will be written to.
  rutz::fstring get_filename() const;

  /// Set the name of the file that will be written to.
  void set_filename(rutz::fstring fname);

  /// Check whether this object currently has a valid, writable stream.
  bool has_stream() const;

  /// Get the ostream associated with this object.
  std::ostream& stream();

private:
  rutz::fstring m_filename;
  std::unique_ptr<std::ostream> m_stream;
};

#endif // !GROOVX_IO_OUTPUTFILE_H_UTC20050626084021_DEFINED
