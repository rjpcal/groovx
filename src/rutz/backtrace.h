///////////////////////////////////////////////////////////////////////
//
// backtrace.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Oct 13 16:32:38 2004
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

#ifndef BACKTRACE_H_DEFINED
#define BACKTRACE_H_DEFINED

#include <iosfwd>

namespace rutz
{
  class backtrace;
  class fstring;
  class prof;
}

/// Represents an instantaneous state of the call stack.
class rutz::backtrace
{
public:
  /// Default construct an empty call stack.
  backtrace() throw();

  /// Copy constructor.
  backtrace(const backtrace& other) throw();

  /// Assignment operator.
  backtrace& operator=(const backtrace& other) throw();

  /// Destructor.
  ~backtrace() throw();

  /// Access the current call stack.
  static backtrace& current() throw();

  /// Push a new element onto the call stack. Returns true if successful.
  bool push(rutz::prof* p) throw();

  /// Pop the most recent element off of the call stack.
  void pop() throw();

  /// Get the number of elements in the call stack.
  unsigned int size() const throw();

  /// Get the top stack frame, or null if the backtrace is empty.
  rutz::prof* top() const throw();

  /// Will return a null pointer if i is out of range.
  rutz::prof* at(unsigned int i) const throw();

  /// Shorthand for at(i).
  rutz::prof* operator[](unsigned int i) const throw() { return at(i); }

  /// Print the call stack on stderr.
  void print() const throw();

  /// Print the call stack to the given stream.
  void print(std::ostream& os) const throw();

  /// Generate a human-readable string representation of the backtrace.
  rutz::fstring format() const;

private:
  struct impl;
  impl* const rep;
};

static const char vcid_backtrace_h[] = "$Id$ $URL$";
#endif // !BACKTRACE_H_DEFINED
