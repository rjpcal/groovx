///////////////////////////////////////////////////////////////////////
//
// soundrep.h
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Oct 20 11:53:57 2004
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

#ifndef SOUNDREP_H_DEFINED
#define SOUNDREP_H_DEFINED

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

static const char vcid_soundrep_h[] = "$Header$";
#endif // !SOUNDREP_H_DEFINED
