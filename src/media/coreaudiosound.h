/** @file media/coreaudiosound.h Mac OSX CoreAudio-based sound file implementation */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016-2016 Rob Peters
// Rob Peters <rjpcal at gmail dot com>
//
// created: Fri Nov 25 22:19:11 2016
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
// --------------------------------------------------------------------
//
// Audio-related code in this file was derived from Apple's "PlayFile"
// example:
// https://developer.apple.com/library/content/samplecode/PlayFile/
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_MEDIA_COREAUDIOSOUND_H_UTC20161126061911_DEFINED
#define GROOVX_MEDIA_COREAUDIOSOUND_H_UTC20161126061911_DEFINED

#include "media/soundrep.h"

#include "rutz/sfmt.h"

#include <AudioToolbox/AudioToolbox.h>
#include <CoreFoundation/CoreFoundation.h>

#include <cctype>
#include <cstring>

#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace media
{
  /// plays sound files synchronously using Apple's AudioToolbox / CoreAudio framework
  class coreaudio_sound_rep : public sound_rep
  {
  public:
    coreaudio_sound_rep(const char* filename);

    virtual ~coreaudio_sound_rep() noexcept
    {
      AudioFileClose (m_audio_file);
    }

    virtual void play() override;

  private:
    AudioFileID m_audio_file;
    AudioStreamBasicDescription m_file_format;
  };
}

namespace
{

  // An extended exception class that includes the name of the failed operation
  class CAXException
  {
  public:
    CAXException(const char *operation, OSStatus err) :
      mError(err)
    {
      if (operation == NULL)
        mOperation[0] = '\0';
      else if (strlen(operation) >= sizeof(mOperation))
        {
          memcpy(mOperation, operation, sizeof(mOperation) - 1);
          mOperation[sizeof(mOperation) - 1] = '\0';
        }
      else
        {
          strlcpy(mOperation, operation, sizeof(mOperation));
        }
      // see if it appears to be a 4-char-code
      UInt32 swapped = CFSwapInt32HostToBig(UInt32(mError));
      const char* ch = reinterpret_cast<const char*>(&swapped);
      if (isprint(ch[0]) && isprint(ch[1]) && isprint(ch[2]) && isprint(ch[3]))
        {
          mStr[1] = ch[0];
          mStr[2] = ch[1];
          mStr[3] = ch[2];
          mStr[4] = ch[3];
          mStr[0] = mStr[5] = '\'';
          mStr[6] = '\0';
        }
      else if (mError > -200000 && mError < 200000)
        {
          // no, format it as an integer
          sprintf(mStr, "%d", (int)mError);
        }
      else
        {
          sprintf(mStr, "0x%x", (int)mError);
        }
    }

    char mOperation[256];
    const OSStatus mError;
    char mStr[16];
  };

#define XThrowIfError(error, operation)         \
  do {                                          \
    OSStatus __err = error;                     \
    if (__err) {                                \
      throw CAXException(operation, __err);     \
    }                                           \
  } while (0)

  inline bool is_interleaved(const AudioStreamBasicDescription& desc)
  {
    return !(desc.mFormatFlags & kAudioFormatFlagIsNonInterleaved);
  }

  inline UInt32 number_interleaved_channels(const AudioStreamBasicDescription& desc)
  {
    return is_interleaved(desc) ? desc.mChannelsPerFrame : 1;
  }

  inline UInt32 sample_word_size(const AudioStreamBasicDescription& desc)
  {
    return (desc.mBytesPerFrame > 0 && number_interleaved_channels(desc))
      ? desc.mBytesPerFrame / number_interleaved_channels(desc)
      : 0;
  }

  inline bool packedness_is_significant(const AudioStreamBasicDescription& desc)
  {
    return (sample_word_size(desc) << 3) != desc.mBitsPerChannel;
  }

  inline bool alignment_is_significant(const AudioStreamBasicDescription& desc)
  {
    return packedness_is_significant(desc) || (desc.mBitsPerChannel & 7) != 0;
  }

  static char* string_for_ostype (OSType t, char* writeLocation)
  {
    char* p = writeLocation;
    UInt32 swapped = CFSwapInt32HostToBig(t);
    char* str = reinterpret_cast<char*>(&swapped);
    char* q = str;

    bool hasNonPrint = false;
    for (int i = 0; i < 4; ++i)
      {
        if (!(isprint(*q) && *q != '\\'))
          {
            hasNonPrint = true;
            break;
          }
        q++;
      }
    q = str;

    if (hasNonPrint)
      p += sprintf (p, "0x");
    else
      *p++ = '\'';

    for (int i = 0; i < 4; ++i)
      {
        if (hasNonPrint)
          p += sprintf(p, "%02X", *q++);
        else
          *p++ = *q++;
      }

    if (!hasNonPrint)
      *p++ = '\'';
    *p = '\0';
    return writeLocation;
  }

  char* as_string(const AudioStreamBasicDescription& desc, char* buf, size_t _bufsize)
  {
    int bufsize = (int)_bufsize;        // must be signed to protect against overflow
    char* theBuffer = buf;
    int nc;
    char formatID[24];
    string_for_ostype (desc.mFormatID, formatID);
    nc = snprintf(buf, size_t(bufsize), "%2d ch, %6.0f Hz, %s (0x%08X) ",
                  (int)desc.mChannelsPerFrame, desc.mSampleRate, formatID,
                  (int)desc.mFormatFlags);
    buf += nc;
    if ((bufsize -= nc) <= 0)
      goto exit;
    if (desc.mFormatID == kAudioFormatLinearPCM)
      {
        const bool isInt = !(desc.mFormatFlags & kLinearPCMFormatFlagIsFloat);
        UInt32 wordSize = sample_word_size(desc);
        const char* endian = (wordSize > 1)
          ? ((desc.mFormatFlags & kLinearPCMFormatFlagIsBigEndian)
             ? " big-endian"
             : " little-endian" )
          : "";
        const char* sign = isInt
          ? ((desc.mFormatFlags & kLinearPCMFormatFlagIsSignedInteger)
             ? " signed"
             : " unsigned")
          : "";
        const char* floatInt = isInt ? "integer" : "float";
        char packed[32];
        if (wordSize > 0 && packedness_is_significant(desc))
          {
            if (desc.mFormatFlags & kLinearPCMFormatFlagIsPacked)
              snprintf(packed, sizeof(packed), "packed in %u bytes", wordSize);
            else
              snprintf(packed, sizeof(packed), "unpacked in %u bytes", wordSize);
          }
        else
          packed[0] = '\0';

        const char* align = (wordSize > 0 && alignment_is_significant(desc))
          ? ((desc.mFormatFlags & kLinearPCMFormatFlagIsAlignedHigh)
             ? " high-aligned"
             : " low-aligned")
          : "";
        const char* deinter = (desc.mFormatFlags & kAudioFormatFlagIsNonInterleaved)
          ? ", deinterleaved" : "";
        const char* commaSpace = (packed[0]!='\0') || (align[0]!='\0') ? ", " : "";
        char bitdepth[20];

        int fracbits =
          (desc.mFormatFlags & kLinearPCMFormatFlagsSampleFractionMask)
          >> kLinearPCMFormatFlagsSampleFractionShift;
        if (fracbits > 0)
          snprintf(bitdepth, sizeof(bitdepth), "%d.%d",
                   (int)desc.mBitsPerChannel - fracbits, fracbits);
        else
          snprintf(bitdepth, sizeof(bitdepth), "%d",
                   (int)desc.mBitsPerChannel);

        snprintf(buf, size_t(bufsize), "%s-bit%s%s %s%s%s%s%s",
                 bitdepth, endian, sign, floatInt,
                 commaSpace, packed, align, deinter);
      }
#define CHAR4(s) (((s[0])<<24) | ((s[1])<<16) | ((s[2])<<8) | (s[3]))
    else if (desc.mFormatID == CHAR4("alac"))     //      kAudioFormatAppleLossless
      {
        int sourceBits = 0;
        switch (desc.mFormatFlags)
          {
          case 1: //      kAppleLosslessFormatFlag_16BitSourceData
            sourceBits = 16;
            break;
          case 2: //      kAppleLosslessFormatFlag_20BitSourceData
            sourceBits = 20;
            break;
          case 3: //      kAppleLosslessFormatFlag_24BitSourceData
            sourceBits = 24;
            break;
          case 4: //      kAppleLosslessFormatFlag_32BitSourceData
            sourceBits = 32;
            break;
          }
        if (sourceBits)
          nc = snprintf(buf, size_t(bufsize), "from %d-bit source, ", sourceBits);
        else
          nc = snprintf(buf, size_t(bufsize), "from UNKNOWN source bit depth, ");
        buf += nc;
        if ((bufsize -= nc) <= 0)
          goto exit;
        snprintf(buf, size_t(bufsize), "%d frames/packet",
                 (int)desc.mFramesPerPacket);
      }
    else
      snprintf(buf, size_t(bufsize),
               "%d bits/channel, %d bytes/packet, %d frames/packet, %d bytes/frame",
               (int)desc.mBitsPerChannel, (int)desc.mBytesPerPacket,
               (int)desc.mFramesPerPacket, (int)desc.mBytesPerFrame);
  exit:
    return theBuffer;
  }

  class oneshot_play
  {
    AUGraph m_graph;
    AudioUnit m_file_unit;
    Float64 m_file_duration;

  public:
    oneshot_play(AudioStreamBasicDescription& fileFormat, AudioFileID audioFile);

    void play();

  private:

    static OSStatus set_sample_rate (AudioUnit& unit,
                                     AudioUnitScope inScope,
                                     AudioUnitElement inEl,
                                     Float64 inRate);

    static OSStatus set_number_channels (AudioUnit& unit,
                                         AudioUnitScope inScope,
                                         AudioUnitElement inEl,
                                         UInt32 inChans);
  };

  oneshot_play::oneshot_play(AudioStreamBasicDescription& fileFormat,
                             AudioFileID audioFile)
  {
    XThrowIfError (NewAUGraph (&m_graph), "NewAUGraph");

    AudioComponentDescription cd;

    // output node
    cd.componentType = kAudioUnitType_Output;
    cd.componentSubType = kAudioUnitSubType_DefaultOutput;
    cd.componentManufacturer = kAudioUnitManufacturer_Apple;

    AUNode outputNode;
    XThrowIfError (AUGraphAddNode (m_graph, &cd, &outputNode), "AUGraphAddNode");

    // file AU node
    AUNode fileNode;
    cd.componentType = kAudioUnitType_Generator;
    cd.componentSubType = kAudioUnitSubType_AudioFilePlayer;

    XThrowIfError (AUGraphAddNode (m_graph, &cd, &fileNode), "AUGraphAddNode");

    // connect & setup
    XThrowIfError (AUGraphOpen (m_graph), "AUGraphOpen");

    // install overload listener to detect when something is wrong
    XThrowIfError (AUGraphNodeInfo(m_graph, fileNode, NULL, &m_file_unit),
                   "AUGraphNodeInfo");

    // prepare the file AU for playback
    // set its output channels
    XThrowIfError (set_number_channels
                   (m_file_unit, kAudioUnitScope_Output, 0,
                    fileFormat.mChannelsPerFrame), "set_number_channels");

    // set the output sample rate of the file AU to be the same as the file:
    XThrowIfError (set_sample_rate
                   (m_file_unit, kAudioUnitScope_Output, 0,
                    fileFormat.mSampleRate), "set_sample_rate");

    // load in the file
    XThrowIfError (AudioUnitSetProperty
                   (m_file_unit, kAudioUnitProperty_ScheduledFileIDs,
                    kAudioUnitScope_Global, 0, &audioFile, sizeof(audioFile)),
                   "SetScheduleFile");


    XThrowIfError (AUGraphConnectNodeInput
                   (m_graph, fileNode, 0, outputNode, 0),
                   "AUGraphConnectNodeInput");

    // AT this point we make sure we have the file player AU initialized
    // this also propogates the output format of the AU to the output unit
    XThrowIfError (AUGraphInitialize (m_graph), "AUGraphInitialize");

    // if we have a surround file, then we should try to tell the
    // output AU what the order of the channels will be
    if (fileFormat.mChannelsPerFrame > 2)
      {
        UInt32 layoutSize = 0;
        XThrowIfError (AudioFileGetPropertyInfo
                       (audioFile, kAudioFilePropertyChannelLayout, &layoutSize, NULL),
                       "kAudioFilePropertyChannelLayout");

        if (layoutSize)
          {
            char* layout = new char[layoutSize];

            XThrowIfError (AudioFileGetProperty
                           (audioFile, kAudioFilePropertyChannelLayout,
                            &layoutSize, layout), "Get Layout From AudioFile");

            // ok, now get the output AU and set its layout
            XThrowIfError (AUGraphNodeInfo(m_graph, outputNode, NULL, &m_file_unit),
                           "AUGraphNodeInfo");

            XThrowIfError (AudioUnitSetProperty
                           (m_file_unit, kAudioUnitProperty_AudioChannelLayout,
                            kAudioUnitScope_Input, 0, layout, layoutSize),
                           "kAudioUnitProperty_AudioChannelLayout");

            delete [] layout;
          }
      }

    //
    // calculate the duration
    UInt64 nPackets;
    UInt32 propsize = sizeof(nPackets);
    XThrowIfError (AudioFileGetProperty
                   (audioFile, kAudioFilePropertyAudioDataPacketCount,
                    &propsize, &nPackets), "kAudioFilePropertyAudioDataPacketCount");

    m_file_duration =
      (nPackets * fileFormat.mFramesPerPacket) / fileFormat.mSampleRate;

    ScheduledAudioFileRegion rgn;
    memset (&rgn.mTimeStamp, 0, sizeof(rgn.mTimeStamp));
    rgn.mTimeStamp.mFlags = kAudioTimeStampSampleTimeValid;
    rgn.mTimeStamp.mSampleTime = 0;
    rgn.mCompletionProc = NULL;
    rgn.mCompletionProcUserData = NULL;
    rgn.mAudioFile = audioFile;
    rgn.mLoopCount = 1;
    rgn.mStartFrame = 0;
    rgn.mFramesToPlay = UInt32(nPackets * fileFormat.mFramesPerPacket);

    // tell the file player AU to play all of the file
    XThrowIfError (AudioUnitSetProperty
                   (m_file_unit, kAudioUnitProperty_ScheduledFileRegion,
                    kAudioUnitScope_Global, 0,&rgn, sizeof(rgn)),
                   "kAudioUnitProperty_ScheduledFileRegion");

    // prime the fp AU with default values
    UInt32 defaultVal = 0;
    XThrowIfError (AudioUnitSetProperty
                   (m_file_unit, kAudioUnitProperty_ScheduledFilePrime,
                    kAudioUnitScope_Global, 0, &defaultVal, sizeof(defaultVal)),
                   "kAudioUnitProperty_ScheduledFilePrime");

    // tell the fp AU when to start playing (this ts is in the AU's
    // render time stamps; -1 means next render cycle)
    AudioTimeStamp startTime;
    memset (&startTime, 0, sizeof(startTime));
    startTime.mFlags = kAudioTimeStampSampleTimeValid;
    startTime.mSampleTime = -1;
    XThrowIfError (AudioUnitSetProperty
                   (m_file_unit, kAudioUnitProperty_ScheduleStartTimeStamp,
                    kAudioUnitScope_Global, 0, &startTime, sizeof(startTime)),
                   "kAudioUnitProperty_ScheduleStartTimeStamp");

    dbg_print_nl(0, rutz::sfmt("file duration: %f secs", m_file_duration));
  }

  void oneshot_play::play()
  {
    // start playing
    XThrowIfError (AUGraphStart (m_graph), "AUGraphStart");

    // sleep until the file is finished
    usleep ((unsigned int)(4 * m_file_duration * 1000. * 1000.));

    XThrowIfError (AUGraphStop (m_graph), "AUGraphStop");
    XThrowIfError (AUGraphUninitialize (m_graph), "AUGraphUninitialize");
    XThrowIfError (AUGraphClose (m_graph), "AUGraphClose");
  }

  OSStatus oneshot_play::set_sample_rate (AudioUnit& unit,
                                          AudioUnitScope inScope,
                                          AudioUnitElement inEl,
                                          Float64 inRate)
  {
    AudioStreamBasicDescription desc;
    UInt32 dataSize = sizeof (desc);
    OSStatus result = AudioUnitGetProperty (unit, kAudioUnitProperty_StreamFormat,
                                            inScope, inEl,
                                            &desc, &dataSize);
    if (result) return result;
    desc.mSampleRate = inRate;
    return AudioUnitSetProperty (unit, kAudioUnitProperty_StreamFormat,
                                 inScope, inEl,
                                 &desc, sizeof (AudioStreamBasicDescription));
  }

  OSStatus oneshot_play::set_number_channels (AudioUnit& unit,
                                              AudioUnitScope inScope,
                                              AudioUnitElement inEl,
                                              UInt32 inChans)
  {
    // set this as the output of the AU
    AudioStreamBasicDescription desc;
    UInt32 dataSize = sizeof (desc);
    OSStatus result = AudioUnitGetProperty (unit, kAudioUnitProperty_StreamFormat,
                                            inScope, inEl,
                                            &desc, &dataSize);
    if (result) return result;
    assert(desc.mFormatID == kAudioFormatLinearPCM);
    UInt32 wordSize = sample_word_size(desc);     // get this before changing ANYTHING
    if (wordSize == 0)
      wordSize = (desc.mBitsPerChannel + 7) / 8;
    desc.mChannelsPerFrame = inChans;
    desc.mFramesPerPacket = 1;
    if (is_interleaved(desc))
      {
        desc.mBytesPerPacket = desc.mBytesPerFrame = inChans * wordSize;
        desc.mFormatFlags &= ~kAudioFormatFlagIsNonInterleaved;
      }
    else
      {
        desc.mBytesPerPacket = desc.mBytesPerFrame = wordSize;
        desc.mFormatFlags |= kAudioFormatFlagIsNonInterleaved;
      }
    result = AudioUnitSetProperty (unit, kAudioUnitProperty_StreamFormat,
                                   inScope, inEl,
                                   &desc, sizeof (AudioStreamBasicDescription));
    return result;
  }
}

media::coreaudio_sound_rep::coreaudio_sound_rep(const char* filename)
{
  CFURLRef theURL = CFURLCreateFromFileSystemRepresentation
    (kCFAllocatorDefault, (UInt8*)filename, long(strlen(filename)), false);
  XThrowIfError (!theURL, "CFURLCreateFromFileSystemRepresentation");

  OSStatus err = AudioFileOpenURL (theURL, kAudioFileReadPermission,
                                   0, &m_audio_file);
  CFRelease(theURL);
  XThrowIfError (err, "AudioFileOpenURL");

  // get the number of channels of the file
  UInt32 propsize = sizeof(m_file_format);
  XThrowIfError (AudioFileGetProperty(m_audio_file, kAudioFilePropertyDataFormat,
                                      &propsize, &m_file_format),
                 "AudioFileGetProperty");

  dbg_print_nl(0, rutz::sfmt("playing file: %s", filename));
  char buf[256];
  dbg_print_nl(0, rutz::sfmt("format: AudioStreamBasicDescription: %s",
                             as_string(m_file_format, buf, sizeof(buf))));
}

void media::coreaudio_sound_rep::play()
{
  // load the file contents up for playback before we start playing
  // this has to be done when the AU is initialized and anytime it is
  // reset or uninitialized
  oneshot_play inst(m_file_format, m_audio_file);
  inst.play();
}

#endif // !GROOVX_MEDIA_COREAUDIOSOUND_H_UTC20161126061911DEFINED
