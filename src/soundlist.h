///////////////////////////////////////////////////////////////////////
//
// soundlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:44:40 1999
// written: Wed Mar 22 16:47:02 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDLIST_H_DEFINED
#define SOUNDLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRLIST_H)
#include "ptrlist.h"
#endif

class Sound;

///////////////////////////////////////////////////////////////////////
/**
 *
 * SoundList is a singleton wrapper for \c PtrList<Sound>.
 *
 **/
///////////////////////////////////////////////////////////////////////

class SoundList : public PtrList<Sound> {
private:
  typedef PtrList<Sound> Base;

protected:
  /// Construct and reserve space for \a size objects. 
  SoundList(int size);

public:
  /// Returns a reference to the singleton instance.
  static SoundList& theSoundList();
};

static const char vcid_soundlist_h[] = "$Header$";
#endif // !SOUNDLIST_H_DEFINED
