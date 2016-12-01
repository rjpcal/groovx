/** @file rutz/bzip2stream.cc handle bzip2-encoding through a c++ iostreams interface */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jun 14 15:02:40 2006
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

#include "rutz/bzip2stream.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"

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
  fstring bzip2_ext(".bz2");

  if (filename.ends_with(bzip2_ext))
    {
      throw error(sfmt("couldn't open file '%s' for writing: "
                       "bzip2 libraries must be installed",
                       filename.c_str()), SRC_POS);
    }
  else
    {
      unique_ptr<std::ostream> result =
        std::make_unique<std::ofstream>(filename.c_str(), flags);
      if (result->fail())
        throw error(sfmt("couldn't open file '%s' for writing",
                         filename.c_str()), SRC_POS);

      return result;
    }
}

unique_ptr<std::istream> rutz::ibzip2open(const fstring& filename,
                                          std::ios::openmode flags)
{
  fstring bzip2_ext(".bz2");

  if (filename.ends_with(bzip2_ext))
    {
      throw error(sfmt("couldn't open file '%s' for reading: "
                       "bzip2 libraries must be installed",
                       filename.c_str()), SRC_POS);
    }
  else
    {
      unique_ptr<std::istream> result =
        std::make_unique<std::ifstream>(filename.c_str(), flags);
      if (result->fail())
        throw error(sfmt("couldn't open file '%s' for reading",
                         filename.c_str()), SRC_POS);

      return result;
    }
}

#else

#include <bzlib.h>

namespace
{
  class bzip2streambuf : public std::streambuf
  {
  private:
    bool m_opened;
    unsigned int m_mode;
    FILE* m_file;
    BZFILE* m_bzfile;

    bzip2streambuf(const bzip2streambuf&);
    bzip2streambuf& operator=(const bzip2streambuf&);

    static const int s_buf_size = 4092;
    static const int s_pback_size = 4;
    char m_buf[s_buf_size];

    int flushoutput();

  public:
    bzip2streambuf(const char* name, unsigned int om);
    ~bzip2streambuf() { close(); }

    void close();

    virtual int underflow() override;

    virtual int overflow(int c) override;

    virtual int sync() override;
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

  bzip2streambuf::bzip2streambuf(const char* name, unsigned int om)
    :
    m_opened(false),
    m_mode(0),
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

            setg(m_buf+s_pback_size,
                 m_buf+s_pback_size,
                 m_buf+s_pback_size);
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

            setp(m_buf, m_buf+(s_buf_size-1));
          }

        if (m_bzfile != nullptr)
          {
            m_opened = true;
            m_mode = om;
          }
      }

    if (!m_opened)
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
    if (m_opened)
      {
        sync();
        m_opened = false;

        int bzerror = BZ_OK;

        if (m_mode & std::ios::in)
          BZ2_bzReadClose(&bzerror, m_bzfile);
        else
          BZ2_bzWriteClose(&bzerror, m_bzfile, 0, 0, 0);

        fclose(m_file);
      }
  }

  int bzip2streambuf::underflow() // with help from Josuttis, p. 678
  {
    GVX_TRACE("bzip2streambuf::underflow");
    // is read position before end of buffer?
    if (gptr() < egptr())
      return *gptr();

    off_t numPutback = 0;
    if (s_pback_size > 0)
      {
        // process size of putback area
        // -use number of characters read
        // -but at most four
        numPutback = gptr() - eback();
        if (numPutback > 4)
          numPutback = 4;

        GVX_ASSERT(numPutback >= 0);

        // copy up to four characters previously read into the putback
        // buffer (area of first four characters)
        std::memcpy (m_buf+(4-numPutback), gptr()-numPutback,
                     size_t(numPutback));
      }

    // read new characters
    int bzerror = BZ_OK;
    const int num =
      BZ2_bzRead(&bzerror, m_bzfile,
                 m_buf+s_pback_size, s_buf_size-s_pback_size);

    if (num <= 0) // error (0) or end-of-file (-1)
      return EOF;

    // BZ_STREAM_END isn't really an error as long as we still read
    // more than zero bytes... in that case, we don't want to return
    // EOF yet

    if (bzerror != BZ_OK && bzerror != BZ_STREAM_END)
      return EOF;

    // reset buffer pointers
    setg (m_buf+s_pback_size-numPutback,
          m_buf+s_pback_size,
          m_buf+s_pback_size+num);

    // return next character Hrmph. We have to cast to unsigned char to
    // avoid problems with eof. Problem is, -1 is a valid char value to
    // return. However, without a cast, char(-1) (0xff) gets converted
    // to int(-1), which is 0xffffffff, which is EOF! What we want is
    // int(0x000000ff), which we have to get by int(unsigned char(-1)).
    return static_cast<unsigned char>(*gptr());
  }

  int bzip2streambuf::overflow(int c)
  {
    GVX_TRACE("bzip2streambuf::overflow");
    if (!(m_mode & std::ios::out) || !m_opened) return EOF;

    if (c != EOF)
      {
        // insert the character into the buffer
        *pptr() = char(c);
        pbump(1);
      }

    if (flushoutput() == EOF)
      {
        return -1; // ERROR
      }

    return c;
  }

  int bzip2streambuf::sync()
  {
    if (flushoutput() == EOF)
      {
        return -1; // ERROR
      }
    return 0;
  }

  int bzip2streambuf::flushoutput()
  {
    if (!(m_mode & std::ios::out) || !m_opened) return EOF;

    GVX_ASSERT(pptr()-pbase() <= std::numeric_limits<int>::max());
    const int num = int(pptr()-pbase());
    int bzerror = BZ_OK;
    BZ2_bzWrite(&bzerror, m_bzfile, pbase(), num);

    if (bzerror != BZ_OK)
      return EOF; // ERROR

    pbump(-num);
    return num;
  }
}

unique_ptr<std::ostream> rutz::obzip2open(const fstring& filename,
                                          std::ios::openmode flags)
{
  fstring bzip2_ext(".bz2");

  if (filename.ends_with(bzip2_ext))
    {
      return std::make_unique<bzip2stream>
        (filename.c_str(), std::ios::out|flags);
    }
  else
    {
      unique_ptr<std::ostream> result =
        std::make_unique<std::ofstream>(filename.c_str(), flags);
      if (result->fail())
        throw error(sfmt("couldn't open file '%s' for writing",
                         filename.c_str()), SRC_POS);

      return result;
    }
}

unique_ptr<std::istream> rutz::ibzip2open(const fstring& filename,
                                          std::ios::openmode flags)
{
  fstring bzip2_ext(".bz2");

  if (filename.ends_with(bzip2_ext))
    {
      return std::make_unique<bzip2stream>
        (filename.c_str(), std::ios::in|flags);
    }
  else
    {
      unique_ptr<std::istream> result =
        std::make_unique<std::ifstream>(filename.c_str(), flags);
      if (result->fail())
        throw error(sfmt("couldn't open file '%s' for reading",
                         filename.c_str()), SRC_POS);

      return result;
    }
}

#endif // defined(HAVE_BZLIB_H)

unique_ptr<std::ostream> rutz::obzip2open(const char* filename,
                                       std::ios::openmode flags)
{
  return obzip2open(fstring(filename), flags);
}

unique_ptr<std::istream> rutz::ibzip2open(const char* filename,
                                          std::ios::openmode flags)
{
  return ibzip2open(fstring(filename), flags);
}
