///////////////////////////////////////////////////////////////////////
//
// stringifycmd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 21:43:43 1999
// written: Thu Jul 19 14:14:04 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGIFYCMD_H_DEFINED
#define STRINGIFYCMD_H_DEFINED

class fixed_string;

namespace Util
{
  template <class T> class Ref;
}

namespace IO
{
  class IoObject;

  fixed_string  stringify(Util::Ref<IO::IoObject> obj);
  void        destringify(Util::Ref<IO::IoObject> obj, const char* buf);

  fixed_string write(Util::Ref<IO::IoObject> obj);
  void          read(Util::Ref<IO::IoObject> obj, const char* buf);

  void saveASW(Util::Ref<IO::IoObject> obj, fixed_string filename);
  void loadASR(Util::Ref<IO::IoObject> obj, fixed_string filename);
}

static const char vcid_stringifycmd_h[] = "$Header$";
#endif // !STRINGIFYCMD_H_DEFINED
