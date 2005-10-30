/** @file rutz/sfmt.cc make a rutz::fstring using printf-style
    formatting */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jul  5 11:02:46 2005
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_RUTZ_SFMT_CC_UTC20050705180246_DEFINED
#define GROOVX_RUTZ_SFMT_CC_UTC20050705180246_DEFINED

#include "rutz/sfmt.h"

#include "rutz/error.h"

#include "rutz/debug.h"

using rutz::fstring;

fstring rutz::vsfmt(const char* fmt, va_list ap)
{
  // if we have a null pointer or an empty string, then just return an
  // empty fstring
  if (fmt == 0 || fmt[0] == '\0')
    {
      return fstring();
    }

  size_t bufsize = 512;
  while (1)
    {
      // relying on a gcc extension to create stack arrays whose size
      // is not a compile-time constant -- could also use alloca or
      // std::vector here
      char buf[bufsize];

      const int nchars = vsnprintf(buf, bufsize, fmt, ap);

      if (nchars < 0)
        {
          // a negative return code from vsnprintf() means there was
          // an actual formatting error (i.e., not just a too-short
          // buffer)
          const fstring msg("vsnprintf failed with format string '",
                            fmt, "' and bufsize ", bufsize);
          throw rutz::error(msg, SRC_POS);
        }
      else if (static_cast<size_t>(nchars) >= bufsize)
        {
          // buffer was too small, so let's double the bufsize and try
          // again:
          const size_t new_bufsize = bufsize * 2;
          // make sure that we haven't overflow the unsigned int
          // size_t:
          if (new_bufsize < bufsize)
            {
              const fstring msg("buffer size overflow "
                                "with format string '",
                                fmt, "' and bufsize ", bufsize);
              throw rutz::error(msg, SRC_POS);
            }

          bufsize = new_bufsize;
          continue;
        }
      else
        {
          // OK, the vsnprintf() succeeded:
          return fstring(rutz::char_range(&buf[0], nchars));
        }
    }
  // should never get here:
  GVX_ASSERT(0);
  /* can't happen: */ return fstring();
}

fstring rutz::sfmt(const char* fmt, ...)
{
  va_list a;
  va_start(a, fmt);
  const fstring result = rutz::vsfmt(fmt, a);
  va_end(a);
  return result;
}

static const char vcid_groovx_rutz_sfmt_cc_utc20050705180246[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_SFMT_CC_UTC20050705180246DEFINED
