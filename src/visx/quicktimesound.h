///////////////////////////////////////////////////////////////////////
//
// quicktimesound.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 19 07:38:09 2003
// written: Mon May 19 09:04:31 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef QUICKTIMESOUND_H_DEFINED
#define QUICKTIMESOUND_H_DEFINED

#include "util/strings.h"

#include <Carbon/Carbon.h>
#include <QuickTime/Movies.h>

#include "util/trace.h"
#include "util/debug.h"

/// QuickTimeSoundRep plays sound files synchronously using Apple's QuickTime.
class QuickTimeSoundRep : public SoundRep
{
public:
  QuickTimeSoundRep(const char* filename)
  {
    FSRef ref;

    OSErr err = FSPathMakeRef(filename, &ref, 0);

    if (noErr != err)
      throw Util::Error(fstring("error in FSPathMakeRef: ", err));

    FSSpec spec;

    err = FSGetCatalogInfo(&ref, kFSCatInfoNone,
                           NULL, NULL, &spec, NULL);

    if (noErr != err)
      throw Util::Error(fstring("error in FSGetCatalogInfo: ", err));

    err = OpenMovieFile(&spec, &itsFileRefNum, fsRdPerm);

    if (noErr != err)
      throw Util::Error(fstring("error in OpenMovieFile: ", err));

    err = NewMovieFromFile(&itsMovie, itsFileRefNum, 0, nil,
                           newMovieActive, nil);

    if (noErr != err)
      {
        CloseMovieFile(itsFileRefNum);
        throw Util::Error(fstring("error in NewMovieFromFile: ", err));
      }
  }

  ~QuickTimeSoundRep()
  {
    DisposeMovie(itsMovie);
    CloseMovieFile(itsFileRefNum);
  }

  void play()
  {
    GoToBeginningOfMovie(itsMovie);

    StartMovie(itsMovie);

    while (!IsMovieDone (itsMovie))
      {
        MoviesTask(itsMovie, 0);
      }
  }

  short itsFileRefNum;
  Movie itsMovie;
};

///////////////////////////////////////////////////////////////////////
//
// Sound static member definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  bool quicktimeInited = false;
}

bool Sound::initSound()
{
DOTRACE("Sound::initSound");
  if (!quicktimeInited)
    {
      const OSErr err = EnterMovies();

      if (noErr == err)
        quicktimeInited = true;
    }

  return quicktimeInited;
}

bool Sound::haveSound()
{
DOTRACE("Sound::haveSound");
  return quicktimeInited;
}

void Sound::closeSound()
{
DOTRACE("Sound::closeSound");
  if (quicktimeInited)
    {
      ExitMovies();
    }
}

SoundRep* Sound::newPlatformSoundRep(const char* soundfile)
{
DOTRACE("Sound::newPlatformSoundRep");
  return new QuickTimeSoundRep(soundfile);
}

static const char vcid_quicktimesound_h[] = "$Header$";
#endif // !QUICKTIMESOUND_H_DEFINED
