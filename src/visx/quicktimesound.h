///////////////////////////////////////////////////////////////////////
//
// quicktimesound.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 19 07:38:09 2003
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
  QuickTimeSoundRep(const char* filename);

  virtual ~QuickTimeSoundRep() throw();

  virtual void play();

  short itsFileRefNum;
  Movie itsMovie;
};

QuickTimeSoundRep::QuickTimeSoundRep(const char* filename)
{
DOTRACE("QuickTimeSoundRep::QuickTimeSoundRep");

  // (1) Get an FSRef from the filename
  FSRef ref;

  OSErr err = FSPathMakeRef(reinterpret_cast<const UInt8*>(filename),
                            &ref, 0);

  if (noErr != err)
    throw Util::Error(fstring("error in FSPathMakeRef: ", err));

  // (2) Get an FSSpec from the FSRef
  FSSpec spec;

  err = FSGetCatalogInfo(&ref, kFSCatInfoNone,
                         NULL, NULL, &spec, NULL);

  if (noErr != err)
    throw Util::Error(fstring("error in FSGetCatalogInfo: ", err));

  // (3) Get a movie file descriptor from the FSSpec
  err = OpenMovieFile(&spec, &itsFileRefNum, fsRdPerm);

  if (noErr != err)
    throw Util::Error(fstring("error in OpenMovieFile: ", err));

  // (4) Get a movie object from the movie file
  err = NewMovieFromFile(&itsMovie, itsFileRefNum, 0, nil,
                         newMovieActive, nil);

  if (noErr != err)
    {
      CloseMovieFile(itsFileRefNum);
      throw Util::Error(fstring("error in NewMovieFromFile: ", err));
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
