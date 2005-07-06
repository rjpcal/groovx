/** @file media/esdsound.h play sounds using the esd API
    (Enlightenment Sound Daemon) */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu May 24 18:13:53 2001
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

#ifndef GROOVX_MEDIA_ESDSOUND_H_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_ESDSOUND_H_UTC20050626084018_DEFINED

#include "media/soundrep.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sharedptr.h"

#include <cstring> // for strncpy()
#include <esd.h>
#include <unistd.h> // for write(), close()

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

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

namespace
{
  struct af_error_info
  {
  private:
    af_error_info(const af_error_info&);
    af_error_info& operator=(const af_error_info&);

  public:
    af_error_info() throw() : error(false), code(0) {}

    void set(long v, const char* c) throw()
    {
      error = true;
      code = v;
      strncpy(&message[0], c, MSG_SIZE - 1);
      message[MSG_SIZE - 1] = '\0';
    }

    void reset() throw() { error = false; }

    rutz::fstring what() const
    {
      return rutz::fstring("audiofile error: ",
                           message,
                           " [error code = ", code, "]");
    }

    static const int MSG_SIZE = 512;

    bool  error;
    long  code;
    char  message[MSG_SIZE];
  };

  af_error_info last_error;

  void af_error_handler(long v, const char* c) throw()
  {
    dbg_eval_nl(3, v);
    dbg_eval_nl(3, c);

    last_error.set(v, c);
  }
}

///////////////////////////////////////////////////////////////////////
//
// esd_sound_rep member definitions
//
///////////////////////////////////////////////////////////////////////

media::esd_sound_rep::esd_sound_rep(const char* filename) :
  m_filename("")
{
GVX_TRACE("media::esd_sound_rep::esd_sound_rep");

  sound_rep::check_filename(filename);

  afSetErrorHandler(&af_error_handler);

  last_error.reset();

  // We just use afOpenFile to ensure that the filename refers to
  // a readable+valid file
  AFfilehandle audiofile = afOpenFile(filename, "r",
                                      static_cast<AFfilesetup>(0));

  if (audiofile == 0)
    {
      if (last_error.error == true)
        throw rutz::error(last_error.what(), SRC_POS);
      else
        throw rutz::error(rutz::fstring("couldn't open sound file '",
                                        filename, "'"), SRC_POS);
    }

  int close_result = afCloseFile(audiofile);

  if (close_result == -1)
    {
      if (last_error.error == true)
        throw rutz::error(last_error.what(), SRC_POS);
      else
        throw rutz::error(rutz::fstring("error closing sound file '",
                                        filename, "'"), SRC_POS);
    }

  m_filename = filename;
}

media::esd_sound_rep::~esd_sound_rep() throw() {}

void media::esd_sound_rep::play()
{
  GVX_TRACE("media::esd_sound_rep::play");

  if (m_filename.is_empty())
    return;

  const char* const fname = m_filename.c_str();

  dbg_eval_nl(3, fname);

  last_error.reset();

  // open the audio file
  const AFfilehandle in_file = afOpenFile(fname, "rb", NULL);

  if (in_file == 0)
    {
      if (last_error.error == true)
        throw rutz::error(last_error.what(), SRC_POS);
      else
        throw rutz::error(rutz::fstring("couldn't open sound file '",
                                        fname, "'"), SRC_POS);
    }

  // get audio file parameters
  const int frame_count = afGetFrameCount(in_file, AF_DEFAULT_TRACK);
  const int in_channels = afGetChannels(in_file, AF_DEFAULT_TRACK);
  const double in_rate = afGetRate(in_file, AF_DEFAULT_TRACK);
  int in_format = -1;
  int in_width = -1;
  afGetSampleFormat(in_file, AF_DEFAULT_TRACK, &in_format, &in_width);

  dbg_eval_nl(3, frame_count);
  dbg_eval_nl(3, in_channels);
  dbg_eval_nl(3, in_rate);
  dbg_eval_nl(3, in_format);
  dbg_eval_nl(3, in_width);

  // convert audiofile parameters to EsounD parameters
  int out_bits;
  if (in_width == 8)
    out_bits = ESD_BITS8;
  else if (in_width == 16)
    out_bits = ESD_BITS16;
  else
    {
      throw rutz::error
        (rutz::fstring("while attempting to play sound file '",
                       fname, "': only sample widths of 8 and 16 "
                       "are supported"), SRC_POS);
    }

  const int bytes_per_frame = (in_width * in_channels) / 8;

  dbg_eval_nl(3, bytes_per_frame);

  int out_channels;
  if (in_channels == 1)
    out_channels = ESD_MONO;
  else if (in_channels == 2)
    out_channels = ESD_STEREO;
  else
    {
      throw rutz::error
        (rutz::fstring("while attempting to play sound file '",
                       fname, "': only 1 or 2 channel samples "
                       "are supported"), SRC_POS);
    }

  const int out_mode = ESD_STREAM;
  const int out_func = ESD_PLAY;

  const esd_format_t out_format =
    out_bits | out_channels | out_mode | out_func;

  const int out_rate = int(in_rate);

  // connect to server
  const int out_sock = esd_play_stream(out_format, out_rate, NULL, fname);

  if (out_sock <= 0)
    {
      throw rutz::error
        (rutz::fstring("while attempting to play sound file '",
                       fname, "': couldn't open esd sound socket"),
         SRC_POS);
    }

  // play
  const int BUF_SIZE = 4096;
  char buf[BUF_SIZE];
  const int buf_frames = BUF_SIZE / bytes_per_frame;

  int frames_read = 0;

  int total_frames_read = 0;

#ifdef DEBUG_SAMPLES
  FILE* fout = fopen("samples.txt", "w");
#endif

  while ((frames_read = afReadFrames(in_file, AF_DEFAULT_TRACK,
                                     buf, buf_frames)) != 0)
    {
      if (last_error.error == true)
        throw rutz::error(last_error.what(), SRC_POS);

#ifdef DEBUG_SAMPLES
      if (fout != 0)
        for (int i = 0; i < frames_read; ++i)
          {
            int val = 0;

            for (int j = 0; j < bytes_per_frame; ++j)
              val += (buf[i * bytes_per_frame + j] << (j*8));

            fprintf(fout, "%d\n", val);
          }
#endif

      total_frames_read += frames_read;

      if (write(out_sock, buf, frames_read * bytes_per_frame) <= 0)
        break;
    }

#ifdef DEBUG_SAMPLES
  if (fout != 0)
    fclose(fout);
#endif

  // close up and go home
  close(out_sock);
  if (afCloseFile(in_file) != 0)
    {
      if (last_error.error == true)
        throw rutz::error(last_error.what(), SRC_POS);
      else
        throw rutz::error(rutz::fstring("error closing sound file '",
                                        fname, "'"), SRC_POS);
    }

  if (total_frames_read < frame_count)
    {
      dbg_print_nl(0, "low frame count; sound file is truncated?");
    }
}

static const char vcid_groovx_media_esdsound_h_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_MEDIA_ESDSOUND_H_UTC20050626084018_DEFINED
