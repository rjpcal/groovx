///////////////////////////////////////////////////////////////////////
//
// sound.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:43:07 1999
// written: Thu Oct 26 17:52:55 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUND_H_DEFINED
#define SOUND_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IDITEM_H_DEFINED)
#include "iditem.h"
#endif

class SoundError : public ErrorWithMsg {
public:
  SoundError();
  SoundError(const char* msg);
  virtual ~SoundError();
};

class Sound : public virtual IO::IoObject {
public:
  // This function will attempt to do any platform-dependent
  // initialization that is necessary to use Sound's, returning true
  // on success, or false on failure.
  static bool initSound();

  // This function returns true if the necessary initSound() has been
  // previously called with success, and returns false if initSound()
  // has not been called, or if initSound() failed.
  static bool haveSound();

  // This function shuts down sound capability in a
  // platform-appropriate way.
  static void closeSound();

  // Returns a pointer to a new platform-appropriate Sound object. The
  // caller is responsible for destroying the Sound object.
  static Sound* newPlatformSound(const char* soundfile);

  static void setOkSound(IdItem<Sound> ok_sound);
  static void setErrSound(IdItem<Sound> err_sound);
  static IdItem<Sound> getOkSound();
  static IdItem<Sound> getErrSound();

  virtual ~Sound();

  virtual void readFrom(IO::Reader* reader) = 0;
  virtual void writeTo(IO::Writer* writer) const = 0;

  virtual void play() = 0;
  virtual void setFile(const char* filename) = 0;
  virtual const char* getFile() const = 0;
};

static const char vcid_sound_h[] = "$Header$";
#endif // !SOUND_H_DEFINED
