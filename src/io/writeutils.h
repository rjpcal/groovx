///////////////////////////////////////////////////////////////////////
//
// writeutils.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Nov 16 14:18:36 1999
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

#ifndef WRITEUTILS_H_DEFINED
#define WRITEUTILS_H_DEFINED

#include "util/ref.h"

#include "io/writer.h"

#include "util/debug.h"
DBG_REGISTER

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
                             Util::SoftRef<const IO::IoObject>(*begin));
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

static const char vcid_writeutils_h[] = "$Header$";
#endif // !WRITEUTILS_H_DEFINED
