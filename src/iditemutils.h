///////////////////////////////////////////////////////////////////////
//
// iditemutils.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Oct 30 11:19:08 2000
// written: Sat May 19 11:13:12 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEMUTILS_H_DEFINED
#define IDITEMUTILS_H_DEFINED

namespace IO { class IoObject; }

namespace IdItemUtils {
  bool isValidId(int id);
  IO::IoObject* getCheckedItem(int id);

#ifndef ACC_COMPILER
#  define DYNCAST dynamic_cast
#else
  template <class T, class U>
  T DYNCAST(U& u) { return dynamic_cast<T>(u); }
#endif

  template <class T>
  inline T* getCastedItem(int id)
  {
	 IO::IoObject* obj = getCheckedItem(id);
	 T& t = DYNCAST<T&>(*obj);
	 return &t;
  }

#ifdef DYNCAST
#undef DYNCAST
#endif

  template <>
  inline IO::IoObject* getCastedItem<IO::IoObject>(int id)
  { return getCheckedItem(id); }
}

static const char vcid_iditemutils_h[] = "$Header$";
#endif // !IDITEMUTILS_H_DEFINED
