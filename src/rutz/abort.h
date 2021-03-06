/** @file rutz/abort.h low-level GVX_ABORT macro */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Jun 30 15:26:45 2005
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

#ifndef GROOVX_RUTZ_ABORT_H_UTC20050630222645_DEFINED
#define GROOVX_RUTZ_ABORT_H_UTC20050630222645_DEFINED

namespace rutz { namespace debug {
  /// Helper function called frmo the GVX_ABORT() macro.
  [[noreturn]] void abort_aux (const char* what, const char* where,
                               int line_no) noexcept;
}}

/// Print a message and then abort().
/** You probably want to use GVX_PANIC() instead of this macro,
    GVX_PANIC() will also print a rutz::backtrace if there is one
    available. However, for truly low-level code that needs to avoid a
    dependency on rutz::backtrace, there is this bare-bones
    GVX_ABORT() depends on nothing except std library code. */
#define GVX_ABORT(message) \
  rutz::debug::abort_aux(message, __FILE__, __LINE__)

#endif // !GROOVX_RUTZ_ABORT_H_UTC20050630222645DEFINED
