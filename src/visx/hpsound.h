///////////////////////////////////////////////////////////////////////
//
// hpsound.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Oct 12 13:03:47 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef HPSOUND_H_DEFINED
#define HPSOUND_H_DEFINED

#include "io/ioerror.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"
#include "util/strings.h"

#include <Alib.h>
#include <fstream.h>            // to check if files exist

#include "util/trace.h"
#include "util/debug.h"

//  ===================================================================
//
//  File scope definitions
//
//  ===================================================================

namespace HPSOUND_CC_LOCAL
{
  Audio* theAudio = 0;

  static long audioErrorHandler(Audio* audio, AErrorEvent *errEvent)
  {
  DOTRACE("audioErrorhandler");

    static char buf[128];
    AGetErrorText(audio, errEvent->error_code, buf, 127);

    dbgEvalNL(3, buf);

    throw Util::Error(fstring("HP Audio Error: ", buf));

    // we'll never get here, but we need to placate the compiler
    return 0;
  }
}

using namespace HPSOUND_CC_LOCAL;

//  ###################################################################
//  ===================================================================
//
/// HpAudioSound implements the Sound interface using HP's audio API.
//
//  ===================================================================

class HpAudioSound : public Sound
{
public:
  HpAudioSound(const char* filename = 0);
  virtual ~HpAudioSound();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  virtual void play();
  virtual void setFile(const char* filename);
  virtual const char* getFile() const { return itsFilename.c_str(); }

  virtual fstring objTypename() const { return "Sound"; }

  void swap(HpAudioSound& other)
    {
      itsFilename.swap(other.itsFilename);

      Util::swap2(itsSBucket,    other.itsSBucket);
      Util::swap2(itsPlayParams, other.itsPlayParams);
    }

private:
  fstring itsFilename;
  SBucket* itsSBucket;
  SBPlayParams itsPlayParams;
};

///////////////////////////////////////////////////////////////////////
//
// HpAudioSound member definitions
//
///////////////////////////////////////////////////////////////////////

HpAudioSound::HpAudioSound(const char* filename) :
  itsFilename(""),
  itsSBucket(0),
  itsPlayParams()
{
DOTRACE("HpAudioSound::HpAudioSound");
  if ( !theAudio )
    throw Util::Error("invalid HP audio server connection");

  itsPlayParams.pause_first = 0;
  itsPlayParams.start_offset.type = ATTSamples;
  itsPlayParams.start_offset.u.samples = 0;
  itsPlayParams.loop_count = 0;
  itsPlayParams.previous_transaction = 0;
  itsPlayParams.gain_matrix = *ASimplePlayer(theAudio);
  itsPlayParams.priority = APriorityNormal;
  itsPlayParams.play_volume = AUnityGain;
  itsPlayParams.duration.type = ATTFullLength;
  itsPlayParams.event_mask = 0;

  setFile(filename);
}

HpAudioSound::~HpAudioSound()
{
DOTRACE("HpAudioSound::~HpAudioSound");
  if ( theAudio != 0 )
    {
      if (itsSBucket)
        ADestroySBucket( theAudio, itsSBucket, NULL );
    }
}

void HpAudioSound::readFrom(IO::Reader* reader)
{
DOTRACE("HpAudioSound::readFrom");

  reader->readValue("filename", itsFilename);

  dbgEval(3, itsFilename.length()); dbgEvalNL(3, itsFilename);

  if (!itsFilename.empty())
    setFile(itsFilename.c_str());
}

void HpAudioSound::writeTo(IO::Writer* writer) const
{
DOTRACE("HpAudioSound::writeTo");

  writer->writeValue("filename", itsFilename);
}

void HpAudioSound::play()
{
DOTRACE("HpAudioSound::play");
  if ( !theAudio )
    throw Util::Error("invalid audio server connection");

  if (itsSBucket)
    ATransID xid = APlaySBucket( theAudio, itsSBucket, &itsPlayParams, NULL );
}

void HpAudioSound::setFile(const char* filename)
{
DOTRACE("HpAudioSound::setFile");

  if (filename != 0 && filename[0] != '\0')
    {
      if ( !theAudio )
        throw Util::Error("invalid audio server connection");

      STD_IO::ifstream ifs(filename);
      if (ifs.fail())
        {
          throw IO::FilenameError(filename);
        }
      ifs.close();

      AFileFormat fileFormat = AFFUnknown;
      AudioAttrMask AttribsMask = 0;
      AudioAttributes Attribs;

      itsSBucket = ALoadAFile(theAudio, const_cast<char *>(filename),
                              fileFormat, AttribsMask, &Attribs, NULL);

      itsFilename = filename;
    }
}


///////////////////////////////////////////////////////////////////////
//
// Sound static member definitions
//
///////////////////////////////////////////////////////////////////////

bool Sound::initSound()
{
DOTRACE("Sound::initSound");
  if (theAudio != 0) return true;

  ASetErrorHandler(audioErrorHandler);

  bool retVal = false;

  // Open an audio connection to the default audio server, then
  // check to make sure connection succeeded. If the connection
  // fails, 'audio' is set to NULL.
  const char* ServerName = "";
  long status = 0;
  theAudio = AOpenAudio( const_cast<char *>(ServerName), &status );
  if ( status != 0 )
    {
      theAudio = NULL;
      retVal = false;
    }
  else
    {
      ASetCloseDownMode( theAudio, AKeepTransactions, NULL );
      retVal = true;
    }

  return retVal;
}

bool Sound::haveSound()
{
DOTRACE("Sound::haveSound");
  return (theAudio != 0);
}

void Sound::closeSound()
{
DOTRACE("Sound::closeSound");
  if ( theAudio )
    {
      ACloseAudio( theAudio, NULL );
      theAudio = 0;
    }
}

Sound* Sound::make()
{
DOTRACE("Sound::make");
  return new HpAudioSound();
}

Sound* Sound::newPlatformSound(const char* soundfile)
{
DOTRACE("Sound::newPlatformSound");
  return new HpAudioSound(soundfile);
}

static const char vcid_hpsound_h[] = "$Header$";
#endif // !HPSOUND_H_DEFINED
