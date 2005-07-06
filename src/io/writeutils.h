/** @file io/writeutils.h helper functions for writing counted
    sequences of objects to an IO::Writer */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov 16 14:18:36 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_IO_WRITEUTILS_H_UTC20050626084021_DEFINED
#define GROOVX_IO_WRITEUTILS_H_UTC20050626084021_DEFINED

#include "io/writer.h"

#include "nub/ref.h"

#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace IO
{
/// Utilities for writing sequences to a \c IO::Writer.
namespace WriteUtils
{

  rutz::fstring makeElementNameString(const rutz::fstring& seq_name,
                                      int element_num);

  rutz::fstring makeSeqCountString(const rutz::fstring& seq_name);

  /** A generic interface for handling containers, sequences, etc. of
      value types. */
  template <class Itr>
  void writeValueSeq(IO::Writer& writer, const rutz::fstring& name,
                     Itr begin, Itr end, bool skip_count=false)
    {
      if (!skip_count)
        {
          writer.writeValue(makeSeqCountString(name).c_str(),
                            computeCount(begin, end));
        }

      int count = 0;

      while (begin != end)
        {
          writer.writeValue(makeElementNameString(name, count).c_str(),
                            *begin);
          ++begin;
          ++count;
        }
    }

  /** A generic interface for handling containers, sequences, etc. of
      objects of rutz::value subtypes */
  template <class Itr>
  void writeValueObjSeq(IO::Writer& writer, const rutz::fstring& name,
                        Itr begin, Itr end, bool skip_count=false)
    {
      if (!skip_count)
        {
          writer.writeValue(makeSeqCountString(name).c_str(),
                            computeCount(begin, end));
        }

      int count = 0;

      while (begin != end)
        {
          writer.writeValueObj(makeElementNameString(name, count).c_str(),
                               *begin);
          ++begin;
          ++count;
        }
    }

  /// A generic interface for handling containers, sequences, etc. of objects
  template <class Itr>
  void writeObjectSeq(IO::Writer& writer, const rutz::fstring& name,
                      Itr begin, Itr end, bool skip_count=false)
    {
      if (!skip_count)
        {
          writer.writeValue(makeSeqCountString(name).c_str(),
                            computeCount(begin, end));
        }

      int count = 0;

      while (begin != end)
        {
          dbg_eval(4, count); dbg_eval_nl(4, (*begin)->id());

          writer.writeObject(makeElementNameString(name, count).c_str(),
                             nub::soft_ref<const IO::IoObject>(*begin));
          ++begin;
          ++count;
        }
    }

  template <class Itr>
  int computeCount(Itr begin, Itr end)
    {
      int count = 0;
      while (begin != end) { ++count; ++begin; }
      return count;
    }

}} // end namespace IO::WriteUtils

static const char vcid_groovx_io_writeutils_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_WRITEUTILS_H_UTC20050626084021_DEFINED
