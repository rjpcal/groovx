/** @file pkgs/whitebox/basesixfourtest.cc tcl interface package for
    testing base64 encoding/decoding */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Oct 13 13:51:48 2004
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

#include "pkgs/whitebox/basesixfourtest.h"

#include "rutz/base64.h"
#include "rutz/bytearray.h"
#include "rutz/fstring.h"
#include "rutz/rand.h"
#include "rutz/unittest.h"

#include "tcl/pkg.h"

#include <cstring> // for memcmp()

#include "rutz/trace.h"

namespace
{
  rutz::fstring makeString(const rutz::byte_array& src)
  {
    return rutz::fstring
      ((rutz::char_range(reinterpret_cast<const char*>(&src.vec[0]),
                         src.vec.size())));
  }

  void testBase64EncodeDecode1()
  {
    // Test encoding/decoding of a string with no extra chars (so the
    // base64 encoding has no trailing "=")

    const char* decoded1 =
      "The Quick Brown Fox Jumped Over The Lazy Dog?";

    const char* encoded1 =
      "VGhlIFF1aWNrIEJyb3duIEZveCBKdW1wZWQgT3ZlciBUaGUgTGF6eSBEb2c/";


    const std::string buf = rutz::base64_encode_string(decoded1, 0);
    TEST_REQUIRE_EQ(rutz::fstring(buf.c_str()), encoded1);
    const rutz::byte_array buf2 = rutz::base64_decode(buf);
    TEST_REQUIRE_EQ(makeString(buf2), decoded1);
  }

  void testBase64EncodeDecode2()
  {
    // Test encoding/decoding of a string with one extra char (so the
    // base64 encoding has a trailing "==")

    const char* decoded2 =
      "The Quick Brown Fox Jumped Over The Lazy Dog?!";

    const char* encoded2 =
      "VGhlIFF1aWNrIEJyb3duIEZveCBKdW1wZWQgT3ZlciBUaGUgTGF6eSBEb2c/IQ==";

    const std::string buf = rutz::base64_encode_string(decoded2, 0);
    TEST_REQUIRE_EQ(rutz::fstring(buf.c_str()), encoded2);
    const rutz::byte_array buf2 = rutz::base64_decode(buf);
    TEST_REQUIRE_EQ(makeString(buf2), decoded2);
  }

  void testBase64EncodeDecode3()
  {
    // Test encoding/decoding of a string with two extra chars (so the
    // base64 encoding has a trailing "=")

    const char* decoded3 =
      "The Quick Brown Fox Jumped Over The Lazy Dog?!?";

    const char* encoded3 =
      "VGhlIFF1aWNrIEJyb3duIEZveCBKdW1wZWQgT3ZlciBUaGUgTGF6eSBEb2c/IT8=";

    const std::string buf = rutz::base64_encode_string(decoded3, 0);
    TEST_REQUIRE_EQ(rutz::fstring(buf.c_str()), encoded3);
    const rutz::byte_array buf2 = rutz::base64_decode(buf);
    TEST_REQUIRE_EQ(makeString(buf2), decoded3);
  }

  void testBase64EncodeDecode4()
  {
    // Test encoding/decoding of a set of random binary data

    const unsigned char decoded4[512] =
      {
        0x50, 0xaa, 0x00, 0xf7, 0x9e, 0x9e, 0xf1, 0x3c, 0xdf, 0xfa, 0x03, 0xe4, 0x58, 0x61, 0x86, 0x37,
        0x4f, 0x00, 0x5b, 0x3d, 0x19, 0x88, 0x66, 0x01, 0x24, 0xf2, 0x2e, 0xba, 0x3d, 0x17, 0x4b, 0x5e,
        0xbb, 0xaa, 0x99, 0xd4, 0x49, 0xee, 0xca, 0x8b, 0x9b, 0x5c, 0x8c, 0x5f, 0x48, 0x40, 0xe2, 0xc7,
        0x14, 0x83, 0x1e, 0xe5, 0xdd, 0xa8, 0xdd, 0x67, 0x4e, 0x3d, 0xf6, 0xc8, 0xba, 0xec, 0xe3, 0x08,
        0xbe, 0x4a, 0x27, 0x8a, 0xcd, 0xf6, 0x27, 0xf3, 0xf7, 0x96, 0xb1, 0x0b, 0xd0, 0xb9, 0xe7, 0x5d,
        0x6b, 0x2d, 0x35, 0xad, 0xc4, 0x29, 0xfe, 0x30, 0x95, 0x98, 0x26, 0xbc, 0x5f, 0x2c, 0x4d, 0x36,
        0x24, 0x62, 0xae, 0x5f, 0x18, 0x14, 0x88, 0xb1, 0xf4, 0x96, 0x25, 0x74, 0xad, 0x94, 0x2d, 0xb8,
        0xaa, 0x56, 0x8e, 0x7c, 0x19, 0x15, 0x25, 0xa2, 0x1c, 0x8b, 0x8b, 0x29, 0xab, 0x45, 0x0e, 0x20,
        0xab, 0x1c, 0xdc, 0xa3, 0xba, 0xb7, 0x62, 0x27, 0x7b, 0x8d, 0x5f, 0xb4, 0x83, 0xeb, 0x51, 0x72,
        0xaf, 0xf3, 0x4c, 0xb6, 0x82, 0x55, 0x85, 0xed, 0xc2, 0xbf, 0x3d, 0x70, 0x66, 0xa2, 0x09, 0xe1,
        0xdb, 0x1c, 0xed, 0xe8, 0x16, 0xf5, 0x08, 0x1c, 0x96, 0xc8, 0xbc, 0x29, 0x8d, 0xfe, 0x4f, 0x5d,
        0xd0, 0xaf, 0x54, 0xc7, 0xe9, 0x4f, 0x88, 0x35, 0x30, 0x1c, 0x68, 0xdb, 0xa0, 0x4a, 0x74, 0xfd,
        0x08, 0xcd, 0x20, 0xae, 0x6d, 0x7f, 0x71, 0x8c, 0x21, 0x00, 0x0d, 0x66, 0xd9, 0x7e, 0x67, 0xe4,
        0x5e, 0xf9, 0x81, 0x01, 0xdb, 0xce, 0x7f, 0xbe, 0x85, 0xbb, 0x09, 0xc7, 0x93, 0x67, 0x36, 0xbf,
        0x95, 0x75, 0xa7, 0x65, 0xc2, 0x88, 0x28, 0xae, 0x81, 0xb5, 0xb6, 0x63, 0x82, 0x5c, 0x7d, 0x4d,
        0x52, 0x45, 0x25, 0x17, 0xb7, 0x36, 0xcd, 0x08, 0xf6, 0x06, 0x52, 0x12, 0x94, 0x87, 0x2e, 0x37,
        0xd4, 0xc9, 0x95, 0x2c, 0x45, 0x81, 0x24, 0x6e, 0xe1, 0xaa, 0x6b, 0x9b, 0xf6, 0xfb, 0xb1, 0x4e,
        0xcc, 0xc8, 0x31, 0x8b, 0xdd, 0x4d, 0x69, 0xc5, 0xdb, 0x62, 0xa4, 0xd0, 0x58, 0x72, 0xe8, 0xe2,
        0x6b, 0x28, 0x3c, 0x50, 0xe8, 0xa9, 0xf8, 0xcf, 0x3d, 0x9e, 0xeb, 0x1f, 0xb7, 0xd9, 0xba, 0x8f,
        0x9d, 0x1c, 0xbf, 0x9d, 0x11, 0x40, 0x3c, 0x49, 0xac, 0x4c, 0x24, 0x95, 0x6a, 0xad, 0xfc, 0x41,
        0xec, 0x3b, 0x33, 0x77, 0x94, 0xea, 0xe8, 0x65, 0x60, 0x27, 0xa7, 0x43, 0x71, 0x57, 0xcc, 0xeb,
        0x4f, 0x0b, 0xe6, 0x78, 0x0a, 0x61, 0x01, 0xa7, 0x2f, 0x96, 0x09, 0xed, 0xd9, 0x24, 0x23, 0xe6,
        0x52, 0xd2, 0xe5, 0xfb, 0x8b, 0x59, 0x1e, 0xd7, 0x28, 0x0b, 0x4d, 0xe0, 0x31, 0x32, 0xe7, 0x68,
        0x11, 0x61, 0x38, 0xdb, 0xc0, 0x19, 0x2b, 0x8d, 0x40, 0x88, 0x6c, 0x88, 0x3c, 0x8c, 0x9f, 0x8e,
        0xac, 0x93, 0xd9, 0xd4, 0x9f, 0x10, 0x85, 0x53, 0xea, 0x2d, 0xc2, 0x23, 0xce, 0xb9, 0x58, 0xbe,
        0x7c, 0x37, 0xe1, 0x64, 0xc7, 0x6e, 0x23, 0x37, 0xe8, 0xe1, 0x06, 0x7e, 0x82, 0x81, 0xcc, 0xbc,
        0x99, 0x5d, 0x75, 0xf7, 0xd5, 0xf5, 0xfa, 0xa5, 0x5c, 0x5b, 0xd1, 0x23, 0x53, 0x11, 0x99, 0xa2,
        0xe5, 0x67, 0x29, 0xde, 0x05, 0x80, 0xe3, 0x36, 0xbb, 0x99, 0xf9, 0xf2, 0xbc, 0x53, 0x10, 0x81,
        0x32, 0x2f, 0x69, 0x34, 0x66, 0xc2, 0xc9, 0x63, 0x74, 0x0e, 0x7c, 0x0e, 0x4a, 0xdd, 0x6e, 0xc1,
        0xb7, 0x09, 0x78, 0x7a, 0x4d, 0xe1, 0x00, 0xb8, 0x71, 0x8c, 0x42, 0x41, 0x67, 0x6f, 0xd1, 0x1f,
        0x6a, 0x13, 0xe9, 0xe5, 0x78, 0x8b, 0x63, 0x89, 0x8c, 0x34, 0x43, 0xbc, 0xf2, 0xa6, 0x01, 0x92,
        0xa3, 0xad, 0xbf, 0x5b, 0x99, 0x5f, 0x83, 0x5a, 0x8e, 0xcc, 0xe6, 0xb7, 0xd1, 0x38, 0xff, 0x81,
      };

    const char* encoded4 =
      "UKoA956e8Tzf+gPkWGGGN08AWz0ZiGYBJPIuuj0XS167qpnUSe7Ki5tcjF9IQOLHFIMe5d2o3WdO"
      "PfbIuuzjCL5KJ4rN9ifz95axC9C5511rLTWtxCn+MJWYJrxfLE02JGKuXxgUiLH0liV0rZQtuKpW"
      "jnwZFSWiHIuLKatFDiCrHNyjurdiJ3uNX7SD61Fyr/NMtoJVhe3Cvz1wZqIJ4dsc7egW9Qgclsi8"
      "KY3+T13Qr1TH6U+INTAcaNugSnT9CM0grm1/cYwhAA1m2X5n5F75gQHbzn++hbsJx5NnNr+Vdadl"
      "wogoroG1tmOCXH1NUkUlF7c2zQj2BlISlIcuN9TJlSxFgSRu4aprm/b7sU7MyDGL3U1pxdtipNBY"
      "cujiayg8UOip+M89nusft9m6j50cv50RQDxJrEwklWqt/EHsOzN3lOroZWAnp0NxV8zrTwvmeAph"
      "Aacvlgnt2SQj5lLS5fuLWR7XKAtN4DEy52gRYTjbwBkrjUCIbIg8jJ+OrJPZ1J8QhVPqLcIjzrlY"
      "vnw34WTHbiM36OEGfoKBzLyZXXX31fX6pVxb0SNTEZmi5Wcp3gWA4za7mfnyvFMQgTIvaTRmwslj"
      "dA58DkrdbsG3CXh6TeEAuHGMQkFnb9EfahPp5XiLY4mMNEO88qYBkqOtv1uZX4Najszmt9E4/4E=";

    const std::string buf = rutz::base64_encode(&decoded4[0], 512, 0);
    TEST_REQUIRE_EQ(rutz::fstring(buf.c_str()), encoded4);
    const rutz::byte_array buf2 = rutz::base64_decode(buf);
    TEST_REQUIRE_EQ(memcmp(&buf2.vec[0], &decoded4[0], 512), 0);
  }

  void testBase64EncodeDecode5()
  {
    // In this test we just generate some random data, and make sure
    // that we get the same thing back after an encoding-decoding
    // roundtrip.

    rutz::urand generator(rutz::default_rand_seed);

    const unsigned int SZ = (unsigned int)(65530 + generator.idraw(5));

    rutz::byte_array decoded5;
    decoded5.vec.resize(SZ);

    for (unsigned int i = 0; i < SZ; ++i)
      {
        decoded5.vec[i] = (unsigned char)(generator.idraw(256));
      }

    const std::string buf = rutz::base64_encode(&decoded5.vec[0], SZ, 0);
    TEST_REQUIRE_EQ(buf.length(), 4*((SZ+2)/3));
    const rutz::byte_array buf2 = rutz::base64_decode(buf);
    TEST_REQUIRE_EQ(buf2.vec.size(), SZ);
    TEST_REQUIRE_EQ(memcmp(&buf2.vec[0], &decoded5.vec[0], SZ), 0);
  }
}

extern "C"
int Basesixfourtest_Init(Tcl_Interp* interp)
{
GVX_TRACE("Basesixfourtest_Init");

  return tcl::pkg::init
    (interp, "Basesixfourtest", "4.0",
     [](tcl::pkg* pkg) {

      DEF_TEST(pkg, testBase64EncodeDecode1);
      DEF_TEST(pkg, testBase64EncodeDecode2);
      DEF_TEST(pkg, testBase64EncodeDecode3);
      DEF_TEST(pkg, testBase64EncodeDecode4);
      DEF_TEST(pkg, testBase64EncodeDecode5);
    });
}
