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

#include "media/soundrep.h"

#include "util/error.h"
#include "util/fstring.h"

#include <Alib.h>

#include "util/trace.h"
#include "util/debug.h"

namespace media
{
  //  #######################################################
  //  =======================================================

  /// hp_audio_sound_rep implements the Sound interface using HP's audio API.

  class hp_audio_sound_rep : public sound_rep
  {
  public:
    hp_audio_sound_rep(const char* filename = 0);
    virtual ~hp_audio_sound_rep() throw();

    virtual void play();

  private:
    SBucket* m_sbucket;
    SBPlayParams m_play_params;

    static Audio* s_audio = 0;

    static bool init_sound();
    static void close_sound();
    static long error_handler(Audio* audio, AErrorEvent* err);
  };

}

Audio* media::hp_audio_sound_rep::s_audio = 0;

media::hp_audio_sound_rep::hp_audio_sound_rep(const char* filename) :
  m_sbucket(0),
  m_play_params()
{
DOTRACE("hp_audio_sound_rep::hp_audio_sound_rep");
  init_sound();
  if ( !s_audio )
    throw rutz::error("invalid HP audio server connection", SRC_POS);

  sound_rep::check_filename(filename);

  m_play_params.pause_first = 0;
  m_play_params.start_offset.type = ATTSamples;
  m_play_params.start_offset.u.samples = 0;
  m_play_params.loop_count = 0;
  m_play_params.previous_transaction = 0;
  m_play_params.gain_matrix = *ASimplePlayer(s_audio);
  m_play_params.priority = APriorityNormal;
  m_play_params.play_volume = AUnityGain;
  m_play_params.duration.type = ATTFullLength;
  m_play_params.event_mask = 0;

  AFileFormat file_format = AFFUnknown;
  AudioAttrMask attr_mask = 0;
  AudioAttributes attribs;

  m_sbucket = ALoadAFile(s_audio, const_cast<char *>(filename),
                         file_format, attr_mask, &attribs, NULL);
}

media::hp_audio_sound_rep::~hp_audio_sound_rep() throw()
{
DOTRACE("hp_audio_sound_rep::~hp_audio_sound_rep");
  if ( s_audio != 0 )
    {
      if (m_sbucket)
        ADestroySBucket( s_audio, m_sbucket, NULL );
    }
}

void media::hp_audio_sound_rep::play()
{
DOTRACE("hp_audio_sound_rep::play");
  init_sound();
  if ( !s_audio )
    throw rutz::error("invalid audio server connection", SRC_POS);

  if (m_sbucket)
    ATransID xid = APlaySBucket( s_audio, m_sbucket, &m_play_params, NULL );
}

bool media::hp_audio_sound_rep::init_sound()
{
DOTRACE("hp_audio_sound_rep::init_sound");

  if (s_audio != 0) return true;

  ASetErrorHandler(&error_handler);

  bool retval = false;

  // Open an audio connection to the default audio server, then
  // check to make sure connection succeeded. If the connection
  // fails, 'audio' is set to NULL.
  const char* server_name = "";
  long status = 0;
  s_audio = AOpenAudio( const_cast<char*>(server_name), &status );
  if ( status != 0 )
    {
      s_audio = NULL;
      retval = false;
    }
  else
    {
      ASetCloseDownMode( s_audio, AKeepTransactions, NULL );
      retval = true;
    }

  return retval;
}

void media::hp_audio_sound_rep::close_sound()
{
DOTRACE("hp_audio_sound_rep::close_sound");
  if ( s_audio )
    {
      ACloseAudio( s_audio, NULL );
      s_audio = 0;
    }
}

long media::hp_audio_sound_rep::error_handler(Audio* audio, AErrorEvent *err)
{
DOTRACE("media::hp_audio_sound_rep::error_handler");

  static char buf[128];
  AGetErrorText(audio, err->error_code, buf, 127);

  dbg_eval_nl(3, buf);

  throw rutz::error(rutz::fstring("HP Audio Error: ", buf), SRC_POS);

  // we'll never get here, but we need to placate the compiler
  return 0;
}

static const char vcid_hpsound_h[] = "$Id$ $URL$";
#endif // !HPSOUND_H_DEFINED
