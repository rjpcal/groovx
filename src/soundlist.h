///////////////////////////////////////////////////////////////////////
//
// soundlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:44:40 1999
// written: Wed Oct 25 16:52:02 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUNDLIST_H_DEFINED
#define SOUNDLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRLIST_H_DEFINED)
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

  static SoundList theInstance;

protected:
  /// Default constructor makes an empty list.
  SoundList();

  /// Virtual destructor.
  virtual ~SoundList();

public:
  /// Returns a reference to the singleton instance.
  static SoundList& theSoundList();
};

static const char vcid_soundlist_h[] = "$Header$";
#endif // !SOUNDLIST_H_DEFINED
