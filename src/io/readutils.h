///////////////////////////////////////////////////////////////////////
//
// readutils.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Nov 16 14:25:40 1999
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

#ifndef READUTILS_H_DEFINED
#define READUTILS_H_DEFINED

#include "util/error.h"
#include "util/ref.h"    // for dynamicCast

#include "io/reader.h"

namespace IO
{
/// Utilities for reading sequences from a \c IO::Reader.
namespace ReadUtils
{
  fstring makeElementNameString(const fstring& seq_name,
                                int element_num);

  fstring makeSeqCountString(const fstring& seq_name);

  /// Get the number of elements in the stored sequence \a seq_name.
  inline int readSequenceCount(IO::Reader& reader, const fstring& seq_name)
    {
      int count = reader.readInt(makeSeqCountString(seq_name));
      if (0 > count)
        throw rutz::error("read negative value for sequence count",
                          SRC_POS);
      return count;
    }

  /** Provides a generic interface for handling containers, sequences,
      etc. of value types. If the count has already been read from the
      reader, this value can be passed as \c known_count, so that we
      avoid reading the value twice (this may be important if the
      reader does not support random access to the attributes). */
  template <class T, class Inserter>
  inline void readValueSeq(IO::Reader& reader, const fstring& seq_name,
                           Inserter inserter, int known_count = -1)
    {
      int count = (known_count == -1) ?
        readSequenceCount(reader, seq_name) : known_count;

      for (int i = 0; i < count; ++i)
        {
          T temp;
          reader.readValue(makeElementNameString(seq_name, i), temp);
          *inserter = temp;
          ++inserter;
        }
    }

  /** Provides a generic interface for handling containers, sequences,
      etc. of Value subtypes. If the count has already been read from
      the reader, this value can be passed as \c known_count, so that
      we avoid reading the value twice (this may be important if the
      reader does not support random access to the attributes). */
  template <class T, class Inserter>
  inline void readValueObjSeq(IO::Reader& reader, const fstring& seq_name,
                              Inserter inserter, int known_count = -1)
    {
      int count = (known_count == -1) ?
        readSequenceCount(reader, seq_name) : known_count;

      for (int i = 0; i < count; ++i)
        {
          T temp;
          reader.readValueObj(makeElementNameString(seq_name, i), temp);
          *inserter = temp;
          ++inserter;
        }
    }

  /** Provides a generic interface for handling containers, sequences,
      etc. of IO objects. If the count has already been read from
      the reader, this value can be passed as \c known_count, so that
      we avoid reading the value twice (this may be important if the
      reader does not support random access to the attributes). */
  template <class C, class Inserter>
  inline void readObjectSeq(IO::Reader& reader, const fstring& seq_name,
                            Inserter inserter, int known_count = -1)
    {
      int count = (known_count == -1) ?
        readSequenceCount(reader, seq_name) : known_count;

      for (int i = 0; i < count; ++i)
        {
          *inserter = dynamicCast<C>(
                 reader.readObject(makeElementNameString(seq_name, i)));

          ++inserter;
        }
    }

}} // end namespace IO::ReadUtils

static const char vcid_readutils_h[] = "$Header$";
#endif // !READUTILS_H_DEFINED
