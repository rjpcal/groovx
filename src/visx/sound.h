///////////////////////////////////////////////////////////////////////
//
// sound.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:43:07 1999
// written: Fri Mar  3 15:09:45 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUND_H_DEFINED
#define SOUND_H_DEFINED

#ifndef STRINGFWD_H_DEFINED
#include "stringfwd.h"
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

#ifndef IO_H_DEFINED
#include "io.h"
#endif

class SoundError : public ErrorWithMsg {
public:
  SoundError() : ErrorWithMsg() {}
  SoundError(const string& msg) : ErrorWithMsg(msg) {}
};

class Sound : public virtual IO {
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
  static Sound* newPlatformSound(const string& soundfile);

  virtual ~Sound();

  virtual void serialize(ostream& os, IOFlag flag) const = 0;
  virtual void deserialize(istream& is, IOFlag flag) = 0;

  virtual int charCount() const = 0;

  virtual void readFrom(Reader* reader) = 0;
  virtual void writeTo(Writer* writer) const = 0;

  virtual void play() = 0;
  virtual void setFile(const string& filename) = 0;
  virtual const string& getFile() const = 0;
};

static const char vcid_sound_h[] = "$Header$";
#endif // !SOUND_H_DEFINED
