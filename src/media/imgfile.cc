/** @file media/imgfile.cc load/save images in any (supported) format */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Apr 25 09:06:51 2002
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

#include "imgfile.h"

#include "media/bmapdata.h"
#include "media/jpegparser.h"
#include "media/pngparser.h"
#include "media/pnmparser.h"

#include "nub/log.h"

#include "rutz/error.h"
#include "rutz/pipe.h"
#include "rutz/sfmt.h"

#include <cctype>
#include <iterator>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

namespace
{
  fstring tolower(const char* inp)
  {
    std::vector<char> result;

    while (*inp != '\0')
      {
        result.push_back(char(std::tolower(*inp++)));
      }

    result.push_back('\0');

    return fstring(&result[0]);
  }

  enum class image_file_type
    {
      UNKNOWN,
      JPEG,
      PNM,
      PNG,
      GIF
    };

  image_file_type get_image_file_type(const char* filename)
  {
    // Conver to lowercase so we can do case-insensitive tests of the
    // filename extension.
    fstring lowername(tolower(filename));

    if (lowername.ends_with(".pbm")
        || lowername.ends_with(".pgm")
        || lowername.ends_with(".ppm")
        || lowername.ends_with(".pnm")
        || lowername.ends_with(".pbm.gz")
        || lowername.ends_with(".pgm.gz")
        || lowername.ends_with(".ppm.gz")
        || lowername.ends_with(".pnm.gz")
        || lowername.ends_with(".pbm.bz2")
        || lowername.ends_with(".pgm.bz2")
        || lowername.ends_with(".ppm.bz2")
        || lowername.ends_with(".pnm.bz2"))
      {
        return image_file_type::PNM;
      }
    else if (lowername.ends_with(".png"))
      {
        return image_file_type::PNG;
      }
    else if (lowername.ends_with(".jpg")
             || lowername.ends_with(".jpeg"))
      {
        return image_file_type::JPEG;
      }
    else if (lowername.ends_with(".gif"))
      {
        return image_file_type::GIF;
      }
    // else...
    return image_file_type::UNKNOWN;
  }

  rutz::fstring exec_search(const char* progname)
  {
    if (progname[0] == '/') return progname;

    const char* path = getenv("PATH");
    if (!path) path = "/bin:/usr/bin";
    const char* cur = path;
    while (true)
      {
        const char* end = strchr(cur, ':');
        if (!end) end = cur + strlen(cur);
        rutz::fstring p = rutz::sfmt("%.*s/%s", int(end-cur), cur, progname);
        if (access(p.c_str(), R_OK|X_OK) == 0)
          return p;
        if (*end == '\0') break;
        cur = end + 1;
      }
    throw rutz::error(rutz::sfmt("couldn't find %s in PATH=%s", progname, path), SRC_POS);
  }

  // A fallback function to try to read an image by filtering it
  // through a pipe that will convert it to PNM format.
  media::bmap_data pipe_load(const char* progname, const char* filename)
  {
  GVX_TRACE("<imgfile.cc>::pipe_load");

    rutz::fstring fullprogname = exec_search(progname);

    if (access(fullprogname.c_str(), R_OK|X_OK) != 0)
      throw rutz::error(rutz::sfmt("couldn't access program '%s'", progname), SRC_POS);
    if (access(filename, R_OK)      != 0)
      throw rutz::error(rutz::sfmt("couldn't read file '%s'", filename), SRC_POS);

    std::vector<char> nm_copy;
    std::copy(filename, filename+strlen(filename)+1,
              std::back_inserter(nm_copy));
    char* const argv[] = { (char*) fullprogname.c_str(), &nm_copy[0], (char*) 0 };

    rutz::exec_pipe p("r", argv);

    media::bmap_data result = media::load_pnm(p.stream());

    if (p.exit_status() != 0)
      throw rutz::error("child process exited abnormally", SRC_POS);

    return result;
  }

  class io_redirector
  {
    int target_fd;
    int orig_target_fd;
    int file_fd;
  public:
    io_redirector(int target_fd_, const char* filename, int mode)
      :
      target_fd(target_fd_)
    {
      file_fd = open(filename, mode, 0666);
      if (file_fd < 0)
        throw rutz::error(rutz::sfmt("couldn't open file '%s'", filename), SRC_POS);
      orig_target_fd = dup(target_fd);
      if (orig_target_fd < 0)
        {
          close(file_fd);
          throw rutz::error(rutz::sfmt("couldn't dup(%d)", target_fd), SRC_POS);
        }
      if (dup2(file_fd, target_fd) < 0)
        {
          close(orig_target_fd);
          close(file_fd);
          throw rutz::error(rutz::sfmt("couldn't dup2(%d,%d)", file_fd, target_fd), SRC_POS);
        }
    }

    ~io_redirector() { dismiss(); }

    io_redirector(const io_redirector&) = delete;
    io_redirector& operator=(const io_redirector&) = delete;

    void dismiss()
    {
      if (file_fd > 0)
        {
          close(file_fd); file_fd = -1;
          dup2(orig_target_fd, target_fd); target_fd = -1;
          close(orig_target_fd); orig_target_fd = -1;
        }
    }
  };

  void pipe_save(const char* progname, const char* filename,
                 const media::bmap_data& bmap)
  {
    rutz::fstring fullpath = exec_search(progname);

    char* const argv[] = { (char*) fullpath.c_str(), (char*) 0 };

    io_redirector rr(STDOUT_FILENO, filename, O_WRONLY | O_CREAT);
    rutz::exec_pipe p("w", argv);
    rr.dismiss();

    media::save_pnm(p.stream(), bmap);
    p.close();

    if (p.exit_status() != 0)
      throw rutz::error(rutz::sfmt("error while running %s", progname), SRC_POS);
  }
}

media::bmap_data media::load_image(const char* filename)
{
GVX_TRACE("media::load_image");

  media::bmap_data result;
  switch (get_image_file_type(filename))
    {
    case image_file_type::PNM:  result = media::load_pnm(filename); break;
    case image_file_type::PNG:  result = media::load_png(filename); break;
#ifdef HAVE_LIBJPEG
    case image_file_type::JPEG: result = media::load_jpeg(filename); break;
#else
    case image_file_type::JPEG: result = pipe_load("jpegtopnm", filename); break;
#endif
    case image_file_type::GIF:  result = pipe_load("giftopnm", filename); break;
    case image_file_type::UNKNOWN: // fall through
    default:
      // A fallback to try to read just about any image type, given
      // that the program "anytopnm" is installed on the host
      // machine. In that case, we can process the image file with
      // anytopnm, and pipe the output via a stream into a pnm parser.
      result = pipe_load("anytopnm", filename);
      break;
    }

  nub::log(rutz::sfmt("loaded image file %s", filename));

  return result;
}

void media::save_image(const char* filename,
                       const media::bmap_data& data)
{
  switch (get_image_file_type(filename))
    {
    case image_file_type::PNM:  media::save_pnm(filename, data); break;
    case image_file_type::PNG:  media::save_png(filename, data); break;
    case image_file_type::JPEG: pipe_save("pnmtojpeg", filename, data); break;
    case image_file_type::GIF:  pipe_save("pamtogif", filename, data); break;
    case image_file_type::UNKNOWN: // fall through
    default:
      throw rutz::error(rutz::sfmt("unknown file format: %s", filename),
                        SRC_POS);
    }

  nub::log(rutz::sfmt("saved image file %s", filename));
}
