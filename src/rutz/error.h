/** @file rutz/error.h exception base class, derives from
    std::exception */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Jun 22 14:59:47 1999
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

#ifndef GROOVX_RUTZ_ERROR_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_ERROR_H_UTC20050626084020_DEFINED

#include "rutz/fileposition.h"
#include "rutz/fstring.h"

#include <exception>

namespace rutz
{
  class backtrace;
  class error;
}

//  ########################################################
/// \c rutz::error is a basic exception class.
/** It carries a string message describing the error as well as
    information about the location in the source code in which the
    exception was generated. */

class rutz::error : public std::exception
{
public:
  /// Default construct with an empty message string.
  error(const rutz::file_pos& pos);

  /// Construct with an error message.
  error(const rutz::fstring& msg, const rutz::file_pos& pos);

  /// Copy constructor.
  error(const error& other) noexcept;

  /// Virtual destructor.
  virtual ~error() noexcept;

  /// Get the decorated error message as a C-style string.
  virtual const char* what() const noexcept override;

  /// Get the source file position where the error was generated.
  const rutz::file_pos& src_pos() const noexcept { return m_file_pos; }

  /// Get the stack back trace associated with this exception.
  const rutz::backtrace& get_backtrace() const noexcept { return *m_backtrace; }

  /// Copy out the back trace most recently used in constructing a rutz::error.
  static void get_last_backtrace(rutz::backtrace& dst);

protected:
  /// Reset the error message.
  void set_msg(const rutz::fstring& new_msg) noexcept { m_msg = new_msg; }

  /// Get the (un-decorated) error message.
  const rutz::fstring& get_msg() const noexcept { return m_msg; }

private:
  error& operator=(const error& other);

  rutz::fstring          m_msg; // holds the user-provided info
  rutz::fstring          m_context; // holds text from a rutz::error_context
  mutable rutz::fstring  m_what; // holds m_msg plus decoration
  rutz::file_pos         m_file_pos;
  const rutz::backtrace* m_backtrace;
};

#endif // !GROOVX_RUTZ_ERROR_H_UTC20050626084020_DEFINED
