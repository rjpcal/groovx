///////////////////////////////////////////////////////////////////////
//
// sound.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  8 11:43:07 1999
// written: Thu Jul  8 12:11:15 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUND_H_DEFINED
#define SOUND_H_DEFINED

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

#ifndef IO_H_DEFINED
#include "io.h"
#endif

class SoundError : public ErrorWithMsg {
public:
  SoundError(const string& msg="") : ErrorWithMsg(msg) {}
};

class Sound : public virtual IO {
public:
  virtual ~Sound();

  virtual void serialize(ostream& os, IOFlag flag) const = 0;
  virtual void deserialize(istream& is, IOFlag flag) = 0;
  virtual int charCount() const = 0;

  virtual void play() = 0;
  virtual void setFile(const string& filename) = 0;
  virtual const string& getFile() const = 0;
};

static const char vcid_sound_h[] = "$Header$";
#endif // !SOUND_H_DEFINED
