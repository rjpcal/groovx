///////////////////////////////////////////////////////////////////////
//
// soundlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:44:40 1999
// written: Thu Jul  8 11:47:22 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDLIST_H_DEFINED
#define SOUNDLIST_H_DEFINED

#ifndef PTRLIST_H_DEFINED
#include "ptrlist.h"
#endif

class Sound;

class SoundList : public PtrList<Sound>, public virtual IO {
private:
  typedef PtrList<Sound> Base;
protected:
  SoundList(int size);
public:
  static SoundList& theSoundList();
};

static const char vcid_soundlist_h[] = "$Header$";
#endif // !SOUNDLIST_H_DEFINED
