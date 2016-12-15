/** @file rutz/bzip2stream.cc handle bzip2-encoding through a c++ iostreams interface */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Jun 14 15:02:40 2006
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

#include "rutz/bzip2stream.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"
#include "rutz/stream_buffer.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <memory>

#include "rutz/trace.h"

using rutz::sfmt;
using rutz::error;
using rutz::fstring;
using std::unique_ptr;

#ifndef HAVE_LIBBZ2

unique_ptr<std::ostream> rutz::obzip2open(const fstring& filename,
                                          std::ios::openmode flags)
{
  throw error(sfmt("couldn't open file '%s' for bzip2-writing: "
                   "bzip2 libraries must be installed",
                   filename.c_str()), SRC_POS);
}

unique_ptr<std::istream> rutz::ibzip2open(const fstring& filename,
                                          std::ios::openmode flags)
{
  throw error(sfmt("couldn't open file '%s' for bzip2-reading: "
                   "bzip2 libraries must be installed",
                   filename.c_str()), SRC_POS);
}

#else

#include <bzlib.h>

namespace
{
  class bzip2streambuf : public rutz::stream_buffer
  {
  private:
    FILE* m_file;
    BZFILE* m_bzfile;

  public:
    bzip2streambuf(const char* name, std::ios::openmode om);
    ~bzip2streambuf() { close(); }

    bzip2streambuf(const bzip2streambuf&) = delete;
    bzip2streambuf& operator=(const bzip2streambuf&) = delete;

    void close();

    virtual ssize_t do_read(char* mem, size_t n) override;
    virtual ssize_t do_write(const char* mem, size_t n) override;
  };

  class bzip2stream : public std::iostream
  {
  private:
    bzip2streambuf m_buf;
  public:
    bzip2stream(const char* filename_cstr,
                std::ios::openmode mode)
      :
      std::iostream(0),
      m_buf(filename_cstr, mode)
    {
      rdbuf(&m_buf);
    }
  };

  bzip2streambuf::bzip2streambuf(const char* name, std::ios::openmode om)
    :
    rutz::stream_buffer(om),
    m_file(nullptr),
    m_bzfile(nullptr)
  {
    // no append nor read/write mode
    if ( (om & std::ios::ate) || (om & std::ios::app)
         || ((om & std::ios::in) && (om & std::ios::out)) )
      {
        /* do nothing -- opening fails */;
      }
    else
      {
        if (om & std::ios::in)
          {
            m_file = fopen(name, "rb");

            if (m_file == nullptr)
              throw error(sfmt("couldn't open file '%s' for reading",
                               name), SRC_POS);

            int bzerror = BZ_OK;
            m_bzfile = BZ2_bzReadOpen(&bzerror, m_file,
                                      /*verbosity*/ 0,
                                      /*small*/ 0,
                                      /*unused_memory*/ 0,
                                      /*nunused*/ 0);

            if (m_bzfile == nullptr)
              {
                fclose(m_file);

                throw error(sfmt("couldn't open file '%s' for "
                                 "bzip2 decompression", name), SRC_POS);
              }
          }
        else if (om & std::ios::out)
          {
            m_file = fopen(name, "wb");

            if (m_file == nullptr)
              throw error(sfmt("couldn't open file '%s' for writing",
                               name), SRC_POS);

            int bzerror = BZ_OK;
            m_bzfile = BZ2_bzWriteOpen(&bzerror, m_file,
                                       /*blockSize100k*/ 9,
                                       /*verbosity*/ 0,
                                       /*workFactor*/ 30);

            if (m_bzfile == nullptr)
              {
                fclose(m_file);

                throw error(sfmt("couldn't open file '%s' for "
                                 "bzip2 compression", name), SRC_POS);
              }
          }
      }

    if (!m_bzfile)
      {
        if (om & std::ios::in)
          {
            throw error(sfmt("couldn't open file '%s' for reading",
                             name), SRC_POS);
          }
        else if (om & std::ios::out)
          {
            throw error(sfmt("couldn't open file '%s' for writing",
                             name), SRC_POS);
          }
      }
  }

  void bzip2streambuf::close()
  {
    if (m_bzfile)
      {
        sync();

        int bzerror = BZ_OK;
        if (this->openmode() & std::ios::in)
          BZ2_bzReadClose(&bzerror, m_bzfile);
        else
          BZ2_bzWriteClose(&bzerror, m_bzfile, 0, 0, 0);
        m_bzfile = nullptr;

        fclose(m_file);
        m_file = nullptr;
      }
  }

  ssize_t bzip2streambuf::do_read(char* mem, size_t n)
  {
    if (!m_bzfile) return EOF;

    int bzerror = BZ_OK;
    GVX_ASSERT(n < std::numeric_limits<int>::max());
    ssize_t result = BZ2_bzRead(&bzerror, m_bzfile, mem, int(n));

    // return EOF if error; but note BZ_STREAM_END isn't really an
    // error as long as we still read more than zero bytes... in that
    // case, we don't want to return EOF yet

    if (bzerror != BZ_OK && bzerror != BZ_STREAM_END)
      return EOF;

    return result;
  }

  ssize_t bzip2streambuf::do_write(const char* mem, size_t n)
  {
    if (!m_bzfile) return EOF;

    int bzerror = BZ_OK;
    GVX_ASSERT(n < std::numeric_limits<int>::max());
    BZ2_bzWrite(&bzerror, m_bzfile, const_cast<char*>(mem), int(n));

    return (bzerror == BZ_OK) ? ssize_t(n) : EOF;
  }
}

unique_ptr<std::ostream> rutz::obzip2open(const fstring& filename,
                                          std::ios::openmode flags)
{
  return std::make_unique<bzip2stream>(filename.c_str(), std::ios::out|flags);
}

unique_ptr<std::istream> rutz::ibzip2open(const fstring& filename,
                                          std::ios::openmode flags)
{
  return std::make_unique<bzip2stream>(filename.c_str(), std::ios::in|flags);
}

#endif // defined(HAVE_BZLIB_H)
