/** @file media/soundrep.h abstract interface for playable sounds;
    subclasses implement the interface using platform-specific sound
    APIs */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Oct 20 11:53:57 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_MEDIA_SOUNDREP_H_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_SOUNDREP_H_UTC20050626084018_DEFINED

namespace media
{
  /// sound_rep is provides a trivial platform-independent sound interface.
  /** Different concrete subclasses are defined elsewhere to encapsulate
      different platform-dependent sound APIs. */
  class sound_rep
  {
  public:
    virtual ~sound_rep() noexcept;

    virtual void play() = 0;

  protected:
    /// Checks that the filename points to a readable file.
    /** Throws an exception in case of any failure. */
    static void check_filename(const char* filename);
  };
}

#endif // !GROOVX_MEDIA_SOUNDREP_H_UTC20050626084018_DEFINED
