///////////////////////////////////////////////////////////////////////
//
// writeutils.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 16 14:18:36 1999
// written: Fri Dec 20 10:00:49 2002
// $Id$
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
