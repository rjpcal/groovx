///////////////////////////////////////////////////////////////////////
//
// imgfile.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Apr 25 09:06:51 2002
// written: Fri May  2 16:31:36 2003
// $Id$
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

#ifndef IMGFILE_CC_DEFINED
#define IMGFILE_CC_DEFINED

#include "imgfile.h"

#include "gx/jpegparser.h"
#include "gx/pbm.h"
#include "gx/pngparser.h"

#include "util/error.h"
#include "util/log.h"
#include "util/stdiobuf.h"

#include <cctype>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "util/debug.h"
#include "util/trace.h"

namespace
{
  fstring tolower(const char* inp)
  {
    fstring result;

    while (*inp != '\0')
      {
        result.append(char(std::tolower(*inp++)));
      }

    return result;
  }

  enum ImageType
    {
      UNKNOWN,
      JPEG,
      PNM,
      PNG
    };


  ImageType getImageType(const char* filename)
  {
    // Conver to lowercase so we can do case-insensitive tests of the
    // filename extension.
    fstring lowername(tolower(filename));

    if (lowername.ends_with(".pbm")
        || lowername.ends_with(".pgm")
        || lowername.ends_with(".ppm")
        || lowername.ends_with(".pnm"))
      {
        return PNM;
      }
    else if (lowername.ends_with(".png"))
      {
        return PNG;
      }
    else if (lowername.ends_with(".jpg")
             || lowername.ends_with(".jpeg"))
      {
        return JPEG;
      }
    // else...
    return UNKNOWN;
  }

  class PipeFds
  {
  private:
    int itsFds[2]; // reading == fds[0], writing == fds[1]

  public:
    PipeFds()
    {
      if (pipe(itsFds) != 0)
        throw Util::Error("couldn't create pipe");
    }

    ~PipeFds() throw()
    {
      closeReader();
      closeWriter();
    }

    int reader() const throw() { return itsFds[0]; }
    int writer() const throw() { return itsFds[1]; }

    void closeReader() throw()
      { if (itsFds[0] >= 0) close(itsFds[0]); itsFds[0] = -1; }
    void closeWriter() throw()
      { if (itsFds[0] >= 0) close(itsFds[1]); itsFds[1] = -1; }
  };

  class ChildProcess
  {
  public:
    ChildProcess() :
      itsChildStatus(0),
      itsPid(fork())
    {
      if (itsPid == -1)
        throw Util::Error("couldn't fork child process");
    }

    ~ChildProcess() throw()
    {
      wait();
    }

    bool inParent() const throw() { return itsPid != 0; }

    int wait() throw()
    {
      if (itsPid != 0)
        {
          waitpid(itsPid, &itsChildStatus, /*options*/ 0);
          itsPid = 0;
        }

      return itsChildStatus;
    }

  private:
    int itsChildStatus;
    pid_t itsPid;
  };

  class ExecPipe
  {
  public:
    ExecPipe(const char* m, char* const* argv) :
      parentIsReader(isReadMode(m)),
      fds(),
      p(),
      strm(0)
    {
      if (p.inParent())
        {
          if (parentIsReader)
            {
              fds.closeWriter();

              strm = new Util::stdiostream(fds.reader(),
                                           std::ios::in|std::ios::binary);
            }
          else // parent is writer
            {
              fds.closeReader();

              strm = new Util::stdiostream(fds.writer(),
                                           std::ios::out|std::ios::binary);
            }

          if (strm == 0)
            throw Util::Error("couldn't open stream in parent process");
        }
      else // in child
        {
          if (parentIsReader) // ==> child is writer
            {
              fds.closeReader();

              if (dup2(fds.writer(), STDOUT_FILENO) == -1)
                {
                  fprintf(stderr, "dup2 failed in child process\n");
                  exit(-1);
                }
            }
          else // parent is writer, child is reader
            {
              fds.closeWriter();

              if (dup2(fds.reader(), STDIN_FILENO) == -1)
                {
                  fprintf(stderr, "dup2 failed in child process\n");
                  exit(-1);
                }
            }

          execv(argv[0], argv);

          fprintf(stderr, "execv failed in child process\n");
          exit(-1);
        }
    }

    ~ExecPipe() throw()
    {
      delete strm;
    }

    STD_IO::iostream& stream() throw()
    {
      Assert(strm != 0);
      return *strm;
    }

    int exitStatus() throw()
    {
      const int child_status = p.wait();

      // Check if the child process exited abnormally
      if (WIFEXITED(child_status) == 0) return -1;

      // Check if the child process gave an error exit code
      if (WEXITSTATUS(child_status) != 0) return -1;

      // OK, everything looks fine
      return 0;
    }

  private:
    static bool isReadMode(const char* m)
    {
      if (m == 0) throw Util::Error("invalid read/write mode");
      switch (m[0])
        {
        case 'r': return true;
        case 'm': return false;
        }

      throw Util::Error(fstring("invalid read/write mode '", m, "'"));
      return false; // "can't happen"
    }

    bool parentIsReader;
    PipeFds fds;
    ChildProcess p;
    Util::stdiostream* strm;
  };

  // A fallback function to try to read just about any image type, given
  // that the program "anytopnm" is installed on the host machine. In that
  // case, we can process the image file with anytopnm, and pipe the output
  // via a stream into a pnm parser.
  void genericLoad(const char* filename, Gfx::BmapData& data)
  {
  DOTRACE("<imgfile.cc>::genericLoad");

#ifndef ANYTOPNM_PROG
    throw Util::Error(fstring("unknown image file format: ", filename));
#else

    if (access(ANYTOPNM_PROG, R_OK|X_OK) != 0)
      throw Util::Error(fstring("couldn't access program '", ANYTOPNM_PROG, "'"));
    if (access(filename, R_OK)      != 0)
      throw Util::Error(fstring("couldn't read file '", filename, "'"));

    fstring nm_copy(filename);
    char* const argv[] = { (char*) ANYTOPNM_PROG, nm_copy.data(), (char*) 0 };

    ExecPipe p("r", argv);

    Pbm::load(p.stream(), data);

    if (p.exitStatus() != 0)
      throw Util::Error("child process exited abnormally");
#endif
  }
}

void ImgFile::load(const char* filename, Gfx::BmapData& data)
{
DOTRACE("ImgFile::load");

  switch (getImageType(filename))
    {
    case PNM:  Pbm::load(filename, data); break;
    case PNG:  Png::load(filename, data); break;
    case JPEG: Jpeg::load(filename, data); break;
    default:   genericLoad(filename, data); break;
    }

  Util::log(fstring("loaded image file ", filename));
}

void ImgFile::save(const char* filename, const Gfx::BmapData& data)
{
  switch (getImageType(filename))
    {
    case PNM:  Pbm::save(filename, data); break;
    default:
      throw Util::Error(fstring("unknown file format: ", filename));
    }

  Util::log(fstring("saved image file ", filename));
}

static const char vcid_imgfile_cc[] = "$Header$";
#endif // !IMGFILE_CC_DEFINED
