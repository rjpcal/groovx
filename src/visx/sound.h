///////////////////////////////////////////////////////////////////////
//
// sound.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Jul  8 11:43:07 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef SOUND_H_DEFINED
#define SOUND_H_DEFINED

#include "io/io.h"

#include "util/fstring.h"
#include "util/sharedptr.h"

namespace Nub
{
  template <class T> class Ref;
}

/// SoundRep is provides a trivial platform-independent sound interface.
/** Different concrete subclasses are defined elsewhere to encapsulate
    different platform-dependent sound APIs. */
class SoundRep
{
public:
  virtual ~SoundRep() throw();

  virtual void play() = 0;

protected:
  /// Checks that the filename points to a readable file.
  /** Throws an exception in case of any failure. */
  static void checkFilename(const char* filename);
};

/// Sound is a generic interface to playable sound snippets.
class Sound : public IO::IoObject
{
public:
  /// Attempts to do any platform-dependent initialization
  /** Returns true on success, or false on failure. */
  static bool initSound();

  /// Check whether the sound system has been successfully initialized.
  /** Returns true if the necessary initSound() has been previously
      called with success, and returns false if initSound() has not
      been called, or if initSound() failed. */
  static bool haveSound();

  /// Shuts down sound capability in a platform-appropriate way.
  static void closeSound();

  /// Returns a pointer to a new platform-appropriate Sound object.
  /** The caller is responsible for destroying the Sound object. */
  static Sound* make();

  /// Returns a pointer to a new platform-appropriate Sound object.
  /** The new object will have the named sound file pre-loaded. The
      caller is responsible for destroying the Sound object. */
  static Sound* makeFrom(const char* filename);

  /// Returns a pointer to a new platform-appropriate SoundRep object.
  /** The caller is responsible for destroying the Sound object. */
  static SoundRep* newPlatformSoundRep(const char* soundfile);

  static void setOkSound(Nub::Ref<Sound> ok_sound);
  static void setErrSound(Nub::Ref<Sound> err_sound);
  static Nub::Ref<Sound> getOkSound();
  static Nub::Ref<Sound> getErrSound();

  /// Construct with the given sound file.
  Sound(const char* filename = 0);

  virtual ~Sound() throw();

  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  /// Play the sound.
  /** This will load the sound file from disk if it has not yet been
      loaded; forceLoad() can be called first to avoid this initial
      latency. */
  void play();

  /// Loaded the sound file from disk if it has not yet been loaded.
  void forceLoad();

  /// Specify a new sound file.
  void setFile(const char* filename);

  /// Get the name of the current sound file.
  const char* getFile() const;

private:
  rutz::fstring itsFilename;
  rutz::shared_ptr<SoundRep> itsRep;
};

static const char vcid_sound_h[] = "$Header$";
#endif // !SOUND_H_DEFINED
