///////////////////////////////////////////////////////////////////////
//
// iodecls.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 27 14:44:42 2000
// written: Wed Aug  8 13:28:43 2001
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
