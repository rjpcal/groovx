/** @file rutz/gzstreambuf.cc handle gzip-encoding through a c++
    iostreams interface */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Jul 20 13:13:22 2001
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

#include "rutz/gzstreambuf.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"
#include "rutz/stream_buffer.h"

#include <fstream>
#include <limits>
#include <memory>

#include "rutz/trace.h"

using rutz::fstring;
using std::unique_ptr;

namespace rutz
{
  class gzstreambuf;
}

/// A std::streambuf implementation that handles gzip-encoded data.
class rutz::gzstreambuf : public rutz::stream_buffer
{
private:
  gzFile m_gzfile;

  gzstreambuf(const gzstreambuf&);
  gzstreambuf& operator=(const gzstreambuf&);

  static const int s_buf_size = 4092;
  static const int s_pback_size = 4;
  char m_buf[s_buf_size];

  int flushoutput();

public:
  gzstreambuf(const char* name, std::ios::openmode om, bool throw_exception=false);
  ~gzstreambuf() { close(); }

  bool is_open() { return m_gzfile != nullptr; }

  void close();

  virtual ssize_t do_read(char* mem, size_t n) override;
  virtual ssize_t do_write(const char* mem, size_t n) override;
};

rutz::gzstreambuf::gzstreambuf(const char* name, std::ios::openmode om,
                               bool throw_exception)
  :
  rutz::stream_buffer(om),
  m_gzfile(nullptr)
{
  // no append nor read/write mode
  if ( (om & std::ios::ate) || (om & std::ios::app)
       || ((om & std::ios::in) && (om & std::ios::out)) )
    {
      /* do nothing -- opening fails */;
    }
  else
    {
      char fmode[10];
      char* fmodeptr = fmode;

      if (om & std::ios::in)
        {
          *fmodeptr++ = 'r';
          setg(m_buf+s_pback_size,
               m_buf+s_pback_size,
               m_buf+s_pback_size);
        }
      else if (om & std::ios::out)
        {
          *fmodeptr++ = 'w';
          setp(m_buf, m_buf+(s_buf_size-1));
        }

      *fmodeptr++ = 'b';
      *fmodeptr = '\0';

      m_gzfile = gzopen(name,fmode);
    }

  if (throw_exception && !m_gzfile)
    {
      if (om & std::ios::in)
        {
          throw rutz::error(rutz::sfmt("couldn't open file '%s' "
                                       "for reading", name), SRC_POS);
        }
      else if (om & std::ios::out)
        {
          throw rutz::error(rutz::sfmt("couldn't open file '%s' "
                                       "for writing", name), SRC_POS);
        }
    }
}

void rutz::gzstreambuf::close()
{
  if (m_gzfile)
    {
      sync();
      gzclose(m_gzfile);
      m_gzfile = nullptr;
    }
}

ssize_t rutz::gzstreambuf::do_read(char* mem, size_t n)
{
GVX_TRACE("rutz::gzstreambuf::do_read");
  if (!m_gzfile) return EOF;
  GVX_ASSERT(n < std::numeric_limits<unsigned int>::max());
  return gzread(m_gzfile, mem, (unsigned int)(n));
}

ssize_t rutz::gzstreambuf::do_write(const char* mem, size_t n)
{
GVX_TRACE("rutz::gzstreambuf::do_write");
  if (!m_gzfile) return EOF;
  GVX_ASSERT(n < std::numeric_limits<unsigned int>::max());
  return gzwrite(m_gzfile, mem, (unsigned int)(n));
}

namespace
{
  class gzstream : public std::iostream
  {
  private:
    rutz::gzstreambuf m_buf;
  public:
    gzstream(const char* filename_cstr,
             std::ios::openmode mode,
             bool throw_exception)
      :
      std::iostream(0),
      m_buf(filename_cstr, mode, throw_exception)
    {
      rdbuf(&m_buf);
    }
  };
}

unique_ptr<std::ostream> rutz::ogzopen(const fstring& filename,
                                       std::ios::openmode flags)
{
  return std::make_unique<gzstream>(filename.c_str(), std::ios::out|flags, true);
}

unique_ptr<std::istream> rutz::igzopen(const fstring& filename,
                                       std::ios::openmode flags)
{
  return std::make_unique<gzstream>(filename.c_str(), std::ios::in|flags, true);
}

// sample test code

//  #include "src/util/gzstreambuf.h"

//  int main() {
//    {
//      rutz::gzstreambuf buf("test.gz", std::ios::out);

//      std::ostream os(&buf);

//      os << "Hello, World!\n";
//    }

//    {
//      rutz::gzstreambuf buf2("test.gz", std::ios::in);

//      std::istream is(&buf2);

//      int c;
//      while ( (c=is.get()) != EOF )
//        {
//          std::cout << char(c);
//        }
//    }

//    return 0;
//  }
