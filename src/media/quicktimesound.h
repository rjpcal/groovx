/** @file media/quicktimesound.h use QuickTime to play sounds */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon May 19 07:38:09 2003
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_MEDIA_QUICKTIMESOUND_H_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_QUICKTIMESOUND_H_UTC20050626084018_DEFINED

#include "media/soundrep.h"

#include "rutz/error.h"
#include "rutz/fstring.h"

#include <Carbon/Carbon.h>
#include <QuickTime/Movies.h>

#include "rutz/trace.h"
#include "rutz/debug.h"

namespace media
{
  /// quicktime_sound_rep plays sound files synchronously using Apple's QuickTime.
  class quicktime_sound_rep : public sound_rep
  {
  public:
    quicktime_sound_rep(const char* filename);

    virtual ~quicktime_sound_rep() throw();

    virtual void play();

    bool init_sound();
    void close_sound();

    short m_file_ref_num;
    Movie m_movie;

    static bool quicktime_inited;
  };
}

bool media::quicktime_sound_rep::quicktime_inited = false;

media::quicktime_sound_rep::quicktime_sound_rep(const char* filename)
{
GVX_TRACE("media::quicktime_sound_rep::quicktime_sound_rep");

  init_sound();

  // (1) Get an FSRef from the filename
  FSRef ref;

  OSErr err = FSPathMakeRef(reinterpret_cast<const UInt8*>(filename),
                            &ref, 0);

  if (noErr != err)
    throw rutz::error(rutz::cat("error in FSPathMakeRef: ", err),
                      SRC_POS);

  // (2) Get an FSSpec from the FSRef
  FSSpec spec;

  err = FSGetCatalogInfo(&ref, kFSCatInfoNone,
                         NULL, NULL, &spec, NULL);

  if (noErr != err)
    throw rutz::error(rutz::cat("error in FSGetCatalogInfo: ", err),
                      SRC_POS);

  // (3) Get a movie file descriptor from the FSSpec
  err = OpenMovieFile(&spec, &m_file_ref_num, fsRdPerm);

  if (noErr != err)
    throw rutz::error(rutz::cat("error in OpenMovieFile: ", err),
                      SRC_POS);

  // (4) Get a movie object from the movie file
  err = NewMovieFromFile(&m_movie, m_file_ref_num, 0, nil,
                         newMovieActive, nil);

  if (noErr != err)
    {
      CloseMovieFile(m_file_ref_num);
      throw rutz::error(rutz::cat("error in NewMovieFromFile: ", err),
                        SRC_POS);
    }
}

media::quicktime_sound_rep::~quicktime_sound_rep() throw()
{
GVX_TRACE("media::quicktime_sound_rep::~quicktime_sound_rep");
  DisposeMovie(m_movie);
  CloseMovieFile(m_file_ref_num);
}

void media::quicktime_sound_rep::play()
{
GVX_TRACE("media::quicktime_sound_rep::play");
  GoToBeginningOfMovie(m_movie);

  StartMovie(m_movie);

  while (!IsMovieDone (m_movie))
    {
      MoviesTask(m_movie, 0);
    }
}

bool media::quicktime_sound_rep::init_sound()
{
GVX_TRACE("media::quicktime_sound_rep::init_sound");
  if (!quicktime_inited)
    {
      const OSErr err = EnterMovies();

      if (noErr == err)
        quicktime_inited = true;
    }

  return quicktime_inited;
}

void media::quicktime_sound_rep::close_sound()
{
GVX_TRACE("quicktime_sound_rep::close_sound");
  if (quicktime_inited)
    {
      ExitMovies();
    }
}

static const char __attribute__((used)) vcid_groovx_media_quicktimesound_h_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_MEDIA_QUICKTIMESOUND_H_UTC20050626084018_DEFINED
