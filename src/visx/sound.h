/** @file visx/sound.h simple playable sound objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Jul  8 11:43:07 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_SOUND_H_UTC20050626084016_DEFINED
#define GROOVX_VISX_SOUND_H_UTC20050626084016_DEFINED

#include "io/io.h"

#include "rutz/fstring.h"
#include "rutz/shared_ptr.h"

namespace media
{
  class sound_rep;
}

namespace nub
{
  template <class T> class ref;
}

/// Sound is a generic interface to playable sound snippets.
class Sound : public io::serializable
{
public:
  /// Returns a pointer to a new platform-appropriate Sound object.
  /** The caller is responsible for destroying the Sound object. */
  static Sound* make();

  /// Returns a pointer to a new platform-appropriate Sound object.
  /** The new object will have the named sound file pre-loaded. The
      caller is responsible for destroying the Sound object. */
  static Sound* makeFrom(const char* filename);

  static void setOkSound(nub::ref<Sound> ok_sound);
  static void setErrSound(nub::ref<Sound> err_sound);
  static nub::ref<Sound> getOkSound();
  static nub::ref<Sound> getErrSound();

  /// Construct with the given sound file.
  Sound(const char* filename = 0);

  virtual ~Sound() noexcept;

  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

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
  rutz::shared_ptr<media::sound_rep> itsRep;
};

#endif // !GROOVX_VISX_SOUND_H_UTC20050626084016_DEFINED
