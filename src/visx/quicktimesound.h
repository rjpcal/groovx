///////////////////////////////////////////////////////////////////////
//
// quicktimesound.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 19 07:38:09 2003
// written: Mon May 19 08:37:48 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef QUICKTIMESOUND_H_DEFINED
#define QUICKTIMESOUND_H_DEFINED

#include "io/ioerror.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/pointers.h"
#include "util/strings.h"

#include <Carbon/Carbon.h>
#include <QuickTime/Movies.h>

#include "util/trace.h"
#include "util/debug.h"

class QuickTimeSoundRep
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

/// QuickTimeSound plays sound files synchronously using Apple's QuickTime.
class QuickTimeSound : public Sound
{
public:
  /// Construct with a named sound file.
  QuickTimeSound(const char* filename = 0);

  /// Virtual destructor.
  virtual ~QuickTimeSound();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  /// Play the sound.
  virtual void play();

  /// Set to refer to a different sound file.
  virtual void setFile(const char* filename);

  /// Get the name of the associated sound file.
  virtual const char* getFile() const { return itsFilename.c_str(); }

  virtual fstring objTypename() const { return "Sound"; }

private:
  fstring itsFilename;
  shared_ptr<QuickTimeSoundRep> itsRep;
};

///////////////////////////////////////////////////////////////////////
//
// QuickTimeSound member definitions
//
///////////////////////////////////////////////////////////////////////

QuickTimeSound::QuickTimeSound(const char* filename) :
  itsFilename(""),
  itsRep(0)
{
DOTRACE("QuickTimeSound::QuickTimeSound");
  setFile(filename);
}

QuickTimeSound::~QuickTimeSound()
{
DOTRACE("QuickTimeSound::~QuickTimeSound");
}

void QuickTimeSound::readFrom(IO::Reader* reader)
{
DOTRACE("QuickTimeSound::readFrom");

  reader->readValue("filename", itsFilename);

  if (!itsFilename.is_empty())
    setFile(itsFilename.c_str());
}

void QuickTimeSound::writeTo(IO::Writer* writer) const
{
DOTRACE("QuickTimeSound::writeTo");

  writer->writeValue("filename", itsFilename);
}

void QuickTimeSound::play()
{
DOTRACE("QuickTimeSound::play");
  if (itsRep.get() != 0)
    itsRep->play();
}

void QuickTimeSound::setFile(const char* filename)
{
DOTRACE("QuickTimeSound::setFile");
  if (filename != 0 && filename[0] != '\0')
    {
      itsRep.reset(new QuickTimeSoundRep(filename));
      itsFilename = filename;
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

Sound* Sound::make()
{
DOTRACE("Sound::make");
  return new QuickTimeSound();
}

Sound* Sound::newPlatformSound(const char* soundfile)
{
DOTRACE("Sound::newPlatformSound");
  return new QuickTimeSound(soundfile);
}

static const char vcid_quicktimesound_h[] = "$Header$";
#endif // !QUICKTIMESOUND_H_DEFINED
