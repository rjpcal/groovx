///////////////////////////////////////////////////////////////////////
//
// quicktimesound.h
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon May 19 07:38:09 2003
// $Id$
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

#ifndef QUICKTIMESOUND_H_DEFINED
#define QUICKTIMESOUND_H_DEFINED

#include "media/soundrep.h"

#include "util/error.h"
#include "util/fstring.h"

#include <Carbon/Carbon.h>
#include <QuickTime/Movies.h>

#include "util/trace.h"
#include "util/debug.h"

/// QuickTimeSoundRep plays sound files synchronously using Apple's QuickTime.
class QuickTimeSoundRep : public SoundRep
{
public:
  QuickTimeSoundRep(const char* filename);

  virtual ~QuickTimeSoundRep() throw();

  virtual void play();

  bool initSound();
  void closeSound();

  short itsFileRefNum;
  Movie itsMovie;
};

QuickTimeSoundRep::QuickTimeSoundRep(const char* filename)
{
DOTRACE("QuickTimeSoundRep::QuickTimeSoundRep");

  initSound();

  // (1) Get an FSRef from the filename
  FSRef ref;

  OSErr err = FSPathMakeRef(reinterpret_cast<const UInt8*>(filename),
                            &ref, 0);

  if (noErr != err)
    throw rutz::error(rutz::fstring("error in FSPathMakeRef: ", err),
                      SRC_POS);

  // (2) Get an FSSpec from the FSRef
  FSSpec spec;

  err = FSGetCatalogInfo(&ref, kFSCatInfoNone,
                         NULL, NULL, &spec, NULL);

  if (noErr != err)
    throw rutz::error(rutz::fstring("error in FSGetCatalogInfo: ", err),
                      SRC_POS);

  // (3) Get a movie file descriptor from the FSSpec
  err = OpenMovieFile(&spec, &itsFileRefNum, fsRdPerm);

  if (noErr != err)
    throw rutz::error(rutz::fstring("error in OpenMovieFile: ", err),
                      SRC_POS);

  // (4) Get a movie object from the movie file
  err = NewMovieFromFile(&itsMovie, itsFileRefNum, 0, nil,
                         newMovieActive, nil);

  if (noErr != err)
    {
      CloseMovieFile(itsFileRefNum);
      throw rutz::error(rutz::fstring("error in NewMovieFromFile: ", err),
                        SRC_POS);
    }
}

QuickTimeSoundRep::~QuickTimeSoundRep() throw()
{
DOTRACE("QuickTimeSoundRep::~QuickTimeSoundRep");
  DisposeMovie(itsMovie);
  CloseMovieFile(itsFileRefNum);
}

void QuickTimeSoundRep::play()
{
DOTRACE("QuickTimeSoundRep::play");
  GoToBeginningOfMovie(itsMovie);

  StartMovie(itsMovie);

  while (!IsMovieDone (itsMovie))
    {
      MoviesTask(itsMovie, 0);
    }
}

namespace
{
  bool quicktimeInited = false;
}

bool QuickTimeSoundRep::initSound()
{
DOTRACE("QuickTimeSoundRep::initSound");
  if (!quicktimeInited)
    {
      const OSErr err = EnterMovies();

      if (noErr == err)
        quicktimeInited = true;
    }

  return quicktimeInited;
}

void QuickTimeSoundRep::closeSound()
{
DOTRACE("QuickTimeSoundRep::closeSound");
  if (quicktimeInited)
    {
      ExitMovies();
    }
}

static const char vcid_quicktimesound_h[] = "$Header$";
#endif // !QUICKTIMESOUND_H_DEFINED
