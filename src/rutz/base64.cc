/** @file rutz/base64.cc functions for base64 encoding/decoding */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Oct  8 13:44:09 2004
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

#include "base64.h"

#include "rutz/bytearray.h"
#include "rutz/error.h"
#include "rutz/mappedfile.h"

#include <cctype> // for isspace()
#include <cstdint> // for uint8_t

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  const char base64_chars[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

  const int base64_to_num2[256] =
    {
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,//   0.. 15
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,//  16.. 31
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,//  32.. 47
      52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,//  48.. 63
      -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,//  64.. 79
      15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,//  80.. 95
      -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,//  96..111
      41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,// 112..127
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 128..143
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 144..159
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 160..175
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 176..191
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 192..207
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 208..223
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 224..239
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,// 240..255
    };
}

std::string rutz::base64_encode(const unsigned char* src,
                                size_t src_len,
                                unsigned int line_width)
{
GVX_TRACE("rutz::base64_encode");

  std::string result;

  if (src_len == 0) return result;

  size_t reserve_size = ((src_len+2)/3) * 4;
  if (line_width > 0)
    reserve_size += 2*(src_len/line_width + 2);
  result.reserve(reserve_size);
  int i = 0;
  unsigned int c = 0;
  unsigned char dec3[3]; // three characters of decoded string (i.e. src input)
  int enc4[4]; // four values of encoded string

  if (line_width > 0)
    {
      result += '\n';
      result += '\t';
    }

  const unsigned char* const stop = src + src_len;

  for (const unsigned char* p = src; p < stop; ++p)
    {
      dec3[i++] = *p;

      if (i == 3)
        {
          //         123456781234567812345678
          // dec3[0] 00000000
          // dec3[1]         11111111
          // dec3[2]                 22222222
          // enc4[0] 000000
          // enc4[1]       111111
          // enc4[2]             222222
          // enc4[3]                   333333

          enc4[0] = (dec3[0] & 0xfc) >> 2;
          enc4[1] = ((dec3[0] & 0x03) << 4) + ((dec3[1] & 0xf0) >> 4);
          enc4[2] = ((dec3[1] & 0x0f) << 2) + ((dec3[2] & 0xc0) >> 6);
          enc4[3] = dec3[2] & 0x3f;

          for (i = 0; i < 4; ++i)
            result += base64_chars[enc4[i]];

          i = 0;

          c += 4;

          if (line_width > 0 && c > line_width)
            {
              c = 0;
              result += '\n';
              result += '\t';
            }
        }
    }

  GVX_ASSERT(i >= 0 && i <= 2);

  if (i == 1)
    {
      enc4[0] = (dec3[0] & 0xfc) >> 2;
      enc4[1] = (dec3[0] & 0x03) << 4;

      result += base64_chars[enc4[0]];
      result += base64_chars[enc4[1]];
      result += '=';
      result += '=';
    }
  else if (i == 2)
    {
      enc4[0] = (dec3[0] & 0xfc) >> 2;
      enc4[1] = ((dec3[0] & 0x03) << 4) + ((dec3[1] & 0xf0) >> 4);
      enc4[2] = (dec3[1] & 0x0f) << 2;

      result += base64_chars[enc4[0]];
      result += base64_chars[enc4[1]];
      result += base64_chars[enc4[2]];
      result += '=';
    }

  return result;
}

std::string rutz::base64_encode_string(const char* str,
                                       unsigned int line_width)
{
  return rutz::base64_encode(reinterpret_cast<const unsigned char*>(str),
                             strlen(str),
                             line_width);
}

std::string rutz::base64_encode_file(const char* filename,
                                     unsigned int line_width)
{
  rutz::mapped_infile m(filename);
  return rutz::base64_encode(static_cast<const unsigned char*>(m.memory()),
                             size_t(m.length()),
                             line_width);
}

rutz::byte_array rutz::base64_decode(const char* src, size_t in_len)
{
GVX_TRACE("rutz::base64_decode");
  rutz::byte_array dst;
  dst.vec.reserve((in_len / 4) * 3);

  int i = 0;
  unsigned char enc4[4];
  unsigned char dec3[3];

  const char* const stop = src+in_len;

  for (const char* p = src; p < stop; ++p)
    {
      int n = base64_to_num2[static_cast<unsigned char>(*p)];
      if (n >= 0)
        {
          enc4[i++] = uint8_t(n);

          if (i == 4)
            {
              dec3[0] = uint8_t((enc4[0] << 2) + ((enc4[1] & 0x30) >> 4));
              dec3[1] = uint8_t(((enc4[1] & 0xf) << 4) + ((enc4[2] & 0x3c) >> 2));
              dec3[2] = uint8_t(((enc4[2] & 0x3) << 6) + enc4[3]);

              for (i = 0; i < 3; ++i)
                dst.vec.push_back(dec3[i]);

              i = 0;
            }
        }

      else if (*p == '=')
        break;

      else if (isspace(*p))
        continue;

      else
        throw rutz::error("invalid character within base64 string",
                          SRC_POS);
    }

  GVX_ASSERT(i != 1); // can't happen; this would be an extra 6 bits
                  // encoded, implying zero extra bits decoded

  if (i == 2)
    {
      // extra 12 bits (2 chars) encoded
      // extra 8 bits (1 char) decoded

      dec3[0] = uint8_t((enc4[0] << 2) + ((enc4[1] & 0x30) >> 4));

      dst.vec.push_back(dec3[0]);
    }

  else if (i == 3)
    {
      // extra 18 bits (3 chars) encoded
      // extra 16 bits (2 chars) decoded
      dec3[0] = uint8_t((enc4[0] << 2) + ((enc4[1] & 0x30) >> 4));
      dec3[1] = uint8_t(((enc4[1] & 0xf) << 4) + ((enc4[2] & 0x3c) >> 2));

      dst.vec.push_back(dec3[0]);
      dst.vec.push_back(dec3[1]);
    }

  return dst;
}

rutz::byte_array rutz::base64_decode(const rutz::byte_array& src)
{
  return base64_decode(reinterpret_cast<const char*>(&src.vec[0]),
                       src.vec.size());
}

rutz::byte_array rutz::base64_decode(const std::string& src)
{
  return base64_decode(src.c_str(), src.length());
}
