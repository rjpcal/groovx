///////////////////////////////////////////////////////////////////////
//
// iodecls.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Sep 27 14:44:42 2000
// written: Wed Mar 19 12:45:51 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IODECLS_H_DEFINED
#define IODECLS_H_DEFINED

namespace IO
{
  class IoObject;

  class Reader;
  class Writer;

  class FilenameError;

  class ReadError;
  class ReadVersionError;

  class WriteError;
  class WriteVersionError;

  /// This type is used for verion ids during the read+write process.
  typedef signed long VersionId;
}

static const char vcid_iodecls_h[] = "$Header$";
#endif // !IODECLS_H_DEFINED
