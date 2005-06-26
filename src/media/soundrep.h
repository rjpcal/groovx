///////////////////////////////////////////////////////////////////////
//
// soundrep.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Oct 20 11:53:57 2004
// commit: $Id$
// $HeadURL$
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

#ifndef SOUNDREP_H_DEFINED
#define SOUNDREP_H_DEFINED

namespace media
{
  /// sound_rep is provides a trivial platform-independent sound interface.
  /** Different concrete subclasses are defined elsewhere to encapsulate
      different platform-dependent sound APIs. */
  class sound_rep
  {
  public:
    virtual ~sound_rep() throw();

    virtual void play() = 0;

  protected:
    /// Checks that the filename points to a readable file.
    /** Throws an exception in case of any failure. */
    static void check_filename(const char* filename);
  };
}

static const char vcid_soundrep_h[] = "$Id$ $URL$";
#endif // !SOUNDREP_H_DEFINED
