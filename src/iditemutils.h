///////////////////////////////////////////////////////////////////////
//
// iditemutils.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct 30 11:19:08 2000
// written: Mon Oct 30 11:19:27 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEMUTILS_H_DEFINED
#define IDITEMUTILS_H_DEFINED

namespace IO { class IoObject; }

namespace IdItemUtils {
  bool isValidId(int id);
  void insertItem(IO::IoObject* obj);
  IO::IoObject* getCheckedItem(int id);

  template <class T>
  inline T* getCastedItem(int id)
  {
	 IO::IoObject* obj = getCheckedItem(id);
	 T& t = dynamic_cast<T&>(*obj);
	 return &t;
  }
}

static const char vcid_iditemutils_h[] = "$Header$";
#endif // !IDITEMUTILS_H_DEFINED
