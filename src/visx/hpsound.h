///////////////////////////////////////////////////////////////////////
//
// hpsound.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Oct 12 13:03:47 1999
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

#ifndef HPSOUND_H_DEFINED
#define HPSOUND_H_DEFINED

#include "io/ioerror.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"
#include "util/strings.h"

#include <Alib.h>

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  Audio* theAudio = 0;

  static long audioErrorHandler(Audio* audio, AErrorEvent *errEvent)
  {
  DOTRACE("audioErrorhandler");

    static char buf[128];
    AGetErrorText(audio, errEvent->error_code, buf, 127);

    dbg_eval_nl(3, buf);

    throw rutz::error(fstring("HP Audio Error: ", buf), SRC_POS);

    // we'll never get here, but we need to placate the compiler
    return 0;
  }
}

//  ###################################################################
//  ===================================================================
//
/// HpAudioSound implements the Sound interface using HP's audio API.
//
//  ===================================================================

class HpAudioSoundRep : public SoundRep
{
public:
  HpAudioSoundRep(const char* filename = 0);
  virtual ~HpAudioSoundRep() throw();

  virtual void play();

private:
  SBucket* itsSBucket;
  SBPlayParams itsPlayParams;
};

HpAudioSoundRep::HpAudioSoundRep(const char* filename) :
  itsSBucket(0),
  itsPlayParams()
{
DOTRACE("HpAudioSoundRep::HpAudioSoundRep");
  if ( !theAudio )
    throw rutz::error("invalid HP audio server connection", SRC_POS);

  SoundRep::checkFilename(filename);

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

  AFileFormat fileFormat = AFFUnknown;
  AudioAttrMask AttribsMask = 0;
  AudioAttributes Attribs;

  itsSBucket = ALoadAFile(theAudio, const_cast<char *>(filename),
                          fileFormat, AttribsMask, &Attribs, NULL);
}

HpAudioSoundRep::~HpAudioSoundRep() throw()
{
DOTRACE("HpAudioSoundRep::~HpAudioSoundRep");
  if ( theAudio != 0 )
    {
      if (itsSBucket)
        ADestroySBucket( theAudio, itsSBucket, NULL );
    }
}

void HpAudioSoundRep::play()
{
DOTRACE("HpAudioSoundRep::play");
  if ( !theAudio )
    throw rutz::error("invalid audio server connection", SRC_POS);

  if (itsSBucket)
    ATransID xid = APlaySBucket( theAudio, itsSBucket, &itsPlayParams, NULL );
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

Sound* Sound::newPlatformSoundRep(const char* soundfile)
{
DOTRACE("Sound::newPlatformSoundRep");
  return new HpAudioSoundRep(soundfile);
}

static const char vcid_hpsound_h[] = "$Header$";
#endif // !HPSOUND_H_DEFINED
