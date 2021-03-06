/** @file media/irixsound.h play sounds using SGI's dmedia API */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Oct 14 11:23:12 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_MEDIA_IRIXSOUND_H_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_IRIXSOUND_H_UTC20050626084018_DEFINED

#include "media/soundrep.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include <dmedia/audio.h>
#include <dmedia/audiofile.h>
#include <unistd.h>
#include <vector>

#include "rutz/trace.h"
#include "rutz/debug.h"

namespace media
{

  //  #######################################################
  //  =======================================================

  /// irix_audio_sound_rep implements Sound using SGI's "dmedia" audio API.

  class irix_audio_sound_rep : public sound_rep
  {
  public:
    irix_audio_sound_rep(const char* filename = 0);
    virtual ~irix_audio_sound_rep() noexcept;

    virtual void play() override;

  private:
    ALconfig m_audio_config;
    std::vector<unsigned char> m_samples;
    AFframecount m_frame_count;
  };

}

media::irix_audio_sound_rep::irix_audio_sound_rep(const char* filename) :
  m_audio_config(alNewConfig()),
  m_samples(),
  m_frame_count(0)
{
GVX_TRACE("media::irix_audio_sound_rep::irix_audio_sound_rep");

  if (m_audio_config == nullptr)
    throw rutz::error("error creating an ALconfig "
                      "while creating Sound", SRC_POS);

  sound_rep::check_filename(filename);

  // Open filename as an audio file for reading. We pass a nullptr
  // AFfilesetup to indicate that file setup parameters should be
  // taken from the file itself.
  AFfilehandle audiofile = afOpenFile(filename, "r", (AFfilesetup) 0);
  if (audiofile == AF_NULL_FILEHANDLE)
    {
      throw rutz::error(rutz::sfmt("couldn't open sound file %s", filename),
                        SRC_POS);
    }

  // Read important parameters from the audio file, and use them to
  // set the corresponding parameters in m_audio_config.

  // Number of audio channels (i.e. mono == 1, stereo == 2)
  const int num_channels = afGetChannels(audiofile, AF_DEFAULT_TRACK);
  if (num_channels == -1)
    {
      throw rutz::error(rutz::sfmt("error reading the number of channels "
                                   "in sound file %s", filename),
                        SRC_POS);
    }
  alSetChannels(m_audio_config, num_channels);

  // Frame count
  m_frame_count = afGetFrameCount(audiofile, AF_DEFAULT_TRACK);
  if (m_frame_count < 0)
    {
      throw rutz::error(rutz::sfmt("error reading the frame count "
                                   "in sound file %s", filename),
                        SRC_POS);
    }

  // Sample format and width
  int sample_format = 0;
  int sample_width = 0;
  afGetSampleFormat(audiofile, AF_DEFAULT_TRACK,
                    &sample_format, &sample_width);
  alSetSampFmt(m_audio_config, sample_format);
  alSetWidth(m_audio_config, sample_width);

  // Sample size and rate
  const int bytes_per_sample = (sample_width + 7)/8;

  const double sampling_rate = afGetRate(audiofile, AF_DEFAULT_TRACK);

  ALpv pv;
  pv.param = AL_RATE;
  pv.value.ll = alDoubleToFixed(sampling_rate);

  alSetParams(AL_DEFAULT_DEVICE, &pv, 1);

  dbg_eval(3, num_channels);
  dbg_eval(3, m_frame_count);
  dbg_eval(3, sample_width);
  dbg_eval(3, bytes_per_sample);
  dbg_eval_nl(3, sampling_rate);

  // Allocate space for the sound samples
  m_samples.resize(m_frame_count*num_channels*bytes_per_sample);

  const int read_result = afReadFrames(audiofile, AF_DEFAULT_TRACK,
                                       static_cast<void*>(&m_samples[0]),
                                       m_frame_count);

  const int close_result = afCloseFile(audiofile);

  if (read_result == -1)
    {
      throw rutz::error(rutz::sfmt("error reading sound data "
                                   "from file %s", filename), SRC_POS);
    }

  if (close_result == -1)
    {
      throw rutz::error(rutz::sfmt("error closing sound file %s",
                                   filename), SRC_POS);
    }
}

media::irix_audio_sound_rep::~irix_audio_sound_rep() noexcept
{
GVX_TRACE("media::irix_audio_sound_rep::~irix_audio_sound_rep");
  if (m_audio_config != nullptr)
    {
      alFreeConfig(m_audio_config);
    }
}

void media::irix_audio_sound_rep::play()
{
GVX_TRACE("media::irix_audio_sound_rep::play");
  if (m_samples.size() == 0) return;

  ALport audio_port = alOpenPort("Sound::play", "w", m_audio_config);
  dbg_eval_nl(3, (void*) audio_port);
  if (audio_port == 0)
    {
      throw rutz::error("error opening an audio port "
                        "during Sound::play", SRC_POS);
    }

  int writeResult =
    alWriteFrames(audio_port, static_cast<void*>(&m_samples[0]), m_frame_count);
  if (writeResult == -1)
    {
      throw rutz::error("error writing to the audio port "
                        "during Sound::play", SRC_POS);
    }

  while (alGetFilled(audio_port) > 0)
    {
      usleep(5000);
    }

  int close_result = alClosePort(audio_port);
  if (close_result == -1)
    {
      throw rutz::error("error closing the audio port "
                        "during Sound::play", SRC_POS);
    }
}

#endif // !GROOVX_MEDIA_IRIXSOUND_H_UTC20050626084018_DEFINED
