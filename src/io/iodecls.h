///////////////////////////////////////////////////////////////////////
//
// iodecls.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 27 14:44:42 2000
// written: Wed Sep 27 14:44:54 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IODECLS_H_DEFINED
#define IODECLS_H_DEFINED

namespace IO {
  class IoObject;

  class Reader;
  class Writer;

  class IoError;
  class InputError;
  class OutputError;
  class LogicError;
  class ValueError;
  class FilenameError;

  /** The symbolic constants of type \c IOFlag flags may be OR'ed
   * together and passed to the flag argument of \c legacySrlz() or \c
   * legacyDesrlz() to control different aspects of the formatting used
   * to read and write objects. In general, the same flags must be
   * used to read an object as were used to write it. */
  typedef int IOFlag;

  /// Neither the class's name or its bases will be written/read
  const IOFlag NO_FLAGS   = 0;
  /// The class's name is written/read
  const IOFlag TYPENAME   = 1 << 0;
  /// The class's bases are written/read
  const IOFlag BASES      = 1 << 1;

  /// This type is used for verion ids during the read+write process.
  typedef unsigned long VersionId;

  /// A default separator to be used between elements in a legacySrlzd object
  const char SEP = ' ';
}

static const char vcid_iodecls_h[] = "$Header$";
#endif // !IODECLS_H_DEFINED
