///////////////////////////////////////////////////////////////////////
//
// esdsound.h
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu May 24 18:13:53 2001
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

#ifndef ESDSOUND_H_DEFINED
#define ESDSOUND_H_DEFINED

#include "media/soundrep.h"

#include "util/error.h"
#include "util/fstring.h"

#include <esd.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace media
{
  /// esd_sound_rep implementats the Sound interface using the ESD API.
  class esd_sound_rep : public sound_rep
  {
  public:
    /// Construct with reference to the named sound file.
    esd_sound_rep(const char* filename = 0);

    /// Virtual destructor.
    virtual ~esd_sound_rep() throw();

    /// Play the sound (in this case using the ESD daemon).
    virtual void play();

  private:
    rutz::fstring m_filename;
  };

}

///////////////////////////////////////////////////////////////////////
//
// esd_sound_rep member definitions
//
///////////////////////////////////////////////////////////////////////

media::esd_sound_rep::esd_sound_rep(const char* filename) :
  m_filename("")
{
DOTRACE("media::esd_sound_rep::esd_sound_rep");

  sound_rep::check_filename(filename);

  // We just use afOpenFile to ensure that the filename refers to
  // a readable+valid file
  AFfilehandle audiofile = afOpenFile(filename, "r",
                                      static_cast<AFfilesetup>(0));

  if (audiofile == 0)
    {
      throw rutz::error(rutz::fstring("couldn't open sound file '",
                                      filename, "'"), SRC_POS);
    }

  int close_result = afCloseFile(audiofile);

  if (close_result == -1)
    {
      throw rutz::error(rutz::fstring("error closing sound file '",
                                      filename, "'"), SRC_POS);
    }

  m_filename = filename;
}

media::esd_sound_rep::~esd_sound_rep() throw() {}

namespace
{
  int my_play_stream(esd_format_t format, int rate, const char* name)
  {
    const char* host = getenv("ESPEAKER");

    int socket_out = esd_play_stream( format, rate, host, name );
    if ( socket_out >= 0 )
      return socket_out;

    /* if host is set, this is an error, bail out */
    if ( host )
      return -1;

    /* go for /dev/dsp */
    esd_audio_format = format;
    esd_audio_rate = rate;
    socket_out = esd_audio_open();

    /* we either got it, or we didn't */
    return socket_out;
  }

  void my_play_file(const char* filename)
  {
    /* open the audio file */
    const AFfilehandle in_file = afOpenFile( filename, "rb", NULL );

    if ( !in_file )
      throw rutz::error(rutz::fstring("couldn't open sound file '",
                                      filename, "'"), SRC_POS);

    /* get audio file parameters */
    const int frame_count = afGetFrameCount(in_file, AF_DEFAULT_TRACK);
    const int in_channels = afGetChannels(in_file, AF_DEFAULT_TRACK);
    const double in_rate = afGetRate(in_file, AF_DEFAULT_TRACK);
    int in_format = -1;
    int in_width = -1;
    afGetSampleFormat(in_file, AF_DEFAULT_TRACK, &in_format, &in_width);

    dbg_eval_nl(3, frame_count);

    /* convert audiofile parameters to EsounD parameters */
    int out_bits;
    if ( in_width == 8 )
      out_bits = ESD_BITS8;
    else if ( in_width == 16 )
      out_bits = ESD_BITS16;
    else
      {
        throw rutz::error
          (rutz::fstring("while attempting to play sound file '",
                         filename, "': only sample widths of 8 and 16 "
                         "are supported"), SRC_POS);
      }

    const int bytes_per_frame = ( in_width  * in_channels ) / 8;

    int out_channels;
    if ( in_channels == 1 )
      out_channels = ESD_MONO;
    else if ( in_channels == 2 )
      out_channels = ESD_STEREO;
    else
      {
        throw rutz::error
          (rutz::fstring("while attempting to play sound file '",
                         filename, "': only 1 or 2 channel samples "
                         "are supported"), SRC_POS);
      }

    const int out_mode = ESD_STREAM;
    const int out_func = ESD_PLAY;

    const esd_format_t out_format =
      out_bits | out_channels | out_mode | out_func;

    const int out_rate = int(in_rate);

    dbg_eval_nl(3, filename);

    /* connect to server and play stream */
    const int out_sock = my_play_stream(out_format, out_rate, filename);

    if ( out_sock <= 0 )
      {
        throw rutz::error
          (rutz::fstring("while attempting to play sound file '",
                         filename, "': couldn't open output socket"),
           SRC_POS);
      }

    /* play */
    esd_send_file( out_sock, in_file, bytes_per_frame );

    /* close up and go home */
    close( out_sock );
    if ( afCloseFile ( in_file ) )
      throw rutz::error(rutz::fstring("error closing sound file '",
                                      filename, "'"), SRC_POS);
  }
}

void media::esd_sound_rep::play()
{
  DOTRACE("media::esd_sound_rep::play");

  if (!m_filename.is_empty())
    {
      my_play_file(m_filename.c_str());
    }
}

static const char vcid_esdsound_h[] = "$Header$";
#endif // !ESDSOUND_H_DEFINED
