///////////////////////////////////////////////////////////////////////
//
// sound.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  8 11:43:07 1999
// written: Wed Jun 26 12:58:19 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUND_H_DEFINED
#define SOUND_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

namespace Util
{
  template <class T> class Ref;
};

/// Sound is a platform-independent interface to playable sound snippets.
class Sound : public IO::IoObject
{
public:
  /** Attempts to do any platform-dependent initialization that is
      necessary to use Sound's, returning true on success, or false on
      failure. */
  static bool initSound();

  /** Returns true if the necessary initSound() has been previously called
      with success, and returns false if initSound() has not been called,
      or if initSound() failed. */
  static bool haveSound();

  /** Shuts down sound capability in a platform-appropriate way. */
  static void closeSound();

  /** Returns a pointer to a new platform-appropriate Sound object. The
      caller is responsible for destroying the Sound object. */
  static Sound* make();

  /** Returns a pointer to a new platform-appropriate Sound object. The
      caller is responsible for destroying the Sound object. */
  static Sound* newPlatformSound(const char* soundfile);

  static void setOkSound(Util::Ref<Sound> ok_sound);
  static void setErrSound(Util::Ref<Sound> err_sound);
  static Util::Ref<Sound> getOkSound();
  static Util::Ref<Sound> getErrSound();

  virtual ~Sound();

  virtual void readFrom(IO::Reader* reader) = 0;
  virtual void writeTo(IO::Writer* writer) const = 0;

  virtual void play() = 0;
  virtual void setFile(const char* filename) = 0;
  virtual const char* getFile() const = 0;
};

static const char vcid_sound_h[] = "$Header$";
#endif // !SOUND_H_DEFINED
