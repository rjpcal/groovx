///////////////////////////////////////////////////////////////////////
//
// readutils.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 16 14:25:40 1999
// written: Mon Jan 13 11:04:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READUTILS_H_DEFINED
#define READUTILS_H_DEFINED

#include "util/ref.h"

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
  inline int readSequenceCount(IO::Reader* reader, const fstring& seq_name)
    {
      int count = reader->readInt(makeSeqCountString(seq_name));
      if (0 > count)
        throw IO::ReadError("read negative value for sequence count");
      return count;
    }

  /** Provides a generic interface for handling containers, sequences,
      etc. of value types. If the count has already been read from the
      reader, this value can be passed as \c known_count, so that we
      avoid reading the value twice (this may be important if the
      reader does not support random access to the attributes). */
  template <class T, class Inserter>
  inline void readValueSeq(IO::Reader* reader, const fstring& seq_name,
                           Inserter inserter, int known_count = -1)
    {
      int count = (known_count == -1) ?
        readSequenceCount(reader, seq_name) : known_count;

      for (int i = 0; i < count; ++i)
        {
          T temp;
          reader->readValue(makeElementNameString(seq_name, i), temp);
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
  inline void readValueObjSeq(IO::Reader* reader, const fstring& seq_name,
                              Inserter inserter, int known_count = -1)
    {
      int count = (known_count == -1) ?
        readSequenceCount(reader, seq_name) : known_count;

      for (int i = 0; i < count; ++i)
        {
          T temp;
          reader->readValueObj(makeElementNameString(seq_name, i), temp);
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
  inline void readObjectSeq(IO::Reader* reader, const fstring& seq_name,
                            Inserter inserter, int known_count = -1)
    {
      int count = (known_count == -1) ?
        readSequenceCount(reader, seq_name) : known_count;

      for (int i = 0; i < count; ++i)
        {
          *inserter = dynamicCast<C>(
                 reader->readObject(makeElementNameString(seq_name, i)));

          ++inserter;
        }
    }
};

} // end namespace IO

static const char vcid_readutils_h[] = "$Header$";
#endif // !READUTILS_H_DEFINED
