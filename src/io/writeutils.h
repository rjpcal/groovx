///////////////////////////////////////////////////////////////////////
//
// writeutils.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Nov 16 14:18:36 1999
// written: Wed Mar 19 17:55:53 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITEUTILS_H_DEFINED
#define WRITEUTILS_H_DEFINED

#include "util/ref.h"

#include "io/writer.h"

#include "util/debug.h"

namespace IO
{

/// Utilities for writing sequences to a \c IO::Writer.
class WriteUtils
{
public:
  /** A generic interface for handling containers, sequences, etc. of
      value types. */
  template <class Itr>
  static void writeValueSeq(IO::Writer* writer, const fstring& name,
                            Itr begin, Itr end, bool skip_count=false)
    {
      if (!skip_count)
        {
          writer->writeValue(makeSeqCountString(name).c_str(),
                             computeCount(begin, end));
        }

      int count = 0;

      while (begin != end)
        {
          writer->writeValue(makeElementNameString(name, count).c_str(),
                             *begin);
          ++begin;
          ++count;
        }
    }

  /** A generic interface for handling containers, sequences, etc. of
      objects of Value subtypes */
  template <class Itr>
  static void writeValueObjSeq(IO::Writer* writer, const fstring& name,
                               Itr begin, Itr end, bool skip_count=false)
    {
      if (!skip_count)
        {
          writer->writeValue(makeSeqCountString(name).c_str(),
                             computeCount(begin, end));
        }

      int count = 0;

      while (begin != end)
        {
          writer->writeValueObj(makeElementNameString(name, count).c_str(),
                                *begin);
          ++begin;
          ++count;
        }
    }


  /// A generic interface for handling containers, sequences, etc. of objects
  template <class Itr>
  static void writeObjectSeq(IO::Writer* writer, const fstring& name,
                             Itr begin, Itr end, bool skip_count=false)
    {
      if (!skip_count)
        {
          writer->writeValue(makeSeqCountString(name).c_str(),
                             computeCount(begin, end));
        }

      int count = 0;

      while (begin != end)
        {
          dbgEval(4, count); dbgEvalNL(4, (*begin)->id());

          writer->writeObject(makeElementNameString(name, count).c_str(),
                              SoftRef<const IO::IoObject>(*begin));
          ++begin;
          ++count;
        }
    }

private:
  template <class Itr>
  static int computeCount(Itr begin, Itr end)
    {
      int count = 0;
      while (begin != end) { ++count; ++begin; }
      return count;
    }

  static fstring makeElementNameString(const fstring& seq_name,
                                       int element_num);

  static fstring makeSeqCountString(const fstring& seq_name);
};

} // end namespace IO

static const char vcid_writeutils_h[] = "$Header$";
#endif // !WRITEUTILS_H_DEFINED
