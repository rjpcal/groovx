///////////////////////////////////////////////////////////////////////
//
// iodecls.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 27 14:44:42 2000
// written: Tue Oct 24 16:28:24 2000
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

  /// This type is used for IoObject unique identifiers
  typedef unsigned long UID;

  /// This type is used for verion ids during the read+write process.
  typedef signed long VersionId;
}

static const char vcid_iodecls_h[] = "$Header$";
#endif // !IODECLS_H_DEFINED
