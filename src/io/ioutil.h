///////////////////////////////////////////////////////////////////////
//
// ioutil.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 11 21:43:43 1999
// written: Wed Mar 19 12:45:50 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOUTIL_H_DEFINED
#define IOUTIL_H_DEFINED

class fstring;

namespace Util
{
  template <class T> class Ref;
}

namespace IO
{
  class IoObject;

  fstring  stringify(Util::Ref<IO::IoObject> obj);
  void     destringify(Util::Ref<IO::IoObject> obj, const char* buf);

  fstring  write(Util::Ref<IO::IoObject> obj);
  void     read(Util::Ref<IO::IoObject> obj, const char* buf);

  void saveASW(Util::Ref<IO::IoObject> obj, fstring filename);
  void loadASR(Util::Ref<IO::IoObject> obj, fstring filename);

  Util::Ref<IO::IoObject> retrieveASR(fstring filename);
}

static const char vcid_ioutil_h[] = "$Header$";
#endif // !IOUTIL_H_DEFINED
