///////////////////////////////////////////////////////////////////////
//
// pbm.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 16:41:07 1999
// written: Thu Aug  9 06:55:27 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PBM_CC_DEFINED
#define PBM_CC_DEFINED

#include "pbm.h"

#include "bmapdata.h"

#include "util/arrays.h"
#include "util/gzstreambuf.h"
#include "util/pipe.h"
#include "util/pointers.h"
#include "util/strings.h"

#ifdef PRESTANDARD_IOSTREAMS
#  include <iostream.h>
#else
#  include <iostream>
#endif
#include <cctype>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const fstring GZ_EXT(".gz");
}

///////////////////////////////////////////////////////////////////////
//
// PbmError member definitions
//
///////////////////////////////////////////////////////////////////////

PbmError::~PbmError() {}

///////////////////////////////////////////////////////////////////////
//
// Pbm::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Pbm::Impl {
public:
  Impl() :
    itsMode(0), itsImageWidth(0), itsImageHeight(0), itsMaxGrey(1),
    itsBitsPerPixel(1), itsNumBytes(0), itsBytes(1)
    {}

  void setBytes(const dynamic_block<unsigned char>& bytes,
                int width, int height, int bits_per_pixel);

  int itsMode;
  int itsImageWidth;
  int itsImageHeight;
  int itsMaxGrey;
  int itsBitsPerPixel;

  int itsNumBytes;
  dynamic_block<unsigned char> itsBytes;
};

void Pbm::Impl::setBytes(const dynamic_block<unsigned char>& bytes,
                         int width, int height, int bits_per_pixel) {
  itsBytes = bytes;
  itsNumBytes = bytes.size();
  itsImageWidth = width;
  itsImageHeight = height;
  itsBitsPerPixel = bits_per_pixel;

  switch (itsBitsPerPixel) {
  case 1:
    itsMode = 4;
    break;
  case 8:
    itsMode = 5;
    break;
  case 24:
    itsMode = 6;
    break;
  default:
    {
      throw PbmError(fstring("invalid bits_per_pixel value: ",
                             itsBitsPerPixel));
    }
    break;
  }
}

///////////////////////////////////////////////////////////////////////
//
// Pbm member definitions
//
///////////////////////////////////////////////////////////////////////

Pbm::Pbm(const BmapData& data) :
  itsImpl( new Impl )
{
  setBytes(data);
}

Pbm::Pbm(STD_IO::istream& is) :
  itsImpl( new Impl )
{
DOTRACE("Pbm::Pbm(STD_IO::istream&)");
  readStream(is);
}

Pbm::Pbm(const char* filename) :
  itsImpl( new Impl )
{
DOTRACE("Pbm::Pbm(const char*)");

  shared_ptr<STD_IO::istream> is(Util::igzopen(filename));

  readStream(*is);
}

Pbm::~Pbm() {
DOTRACE("Pbm::~Pbm");
  delete itsImpl;
}

void Pbm::setBytes(const BmapData& data) {
DOTRACE("Pbm::setBytes");
  itsImpl->setBytes(data.bytesVec(), data.width(), data.height(),
                    data.bitsPerPixel());
}

void Pbm::swapInto(BmapData& data) {
DOTRACE("Pbm::swapInto");
  int dummy_alignment = 1;

  data.swap(itsImpl->itsBytes, itsImpl->itsImageWidth, itsImpl->itsImageHeight,
            itsImpl->itsBitsPerPixel, dummy_alignment);
}

void Pbm::write(const char* filename) const {
DOTRACE("Pbm::write");

  shared_ptr<STD_IO::ostream> os(Util::ogzopen(filename));

  write(*os);
}

void Pbm::write(STD_IO::ostream& os) const {
DOTRACE("Pbm::write");
  os << 'P' << itsImpl->itsMode << ' '
     << itsImpl->itsImageWidth << ' ' << itsImpl->itsImageHeight << '\n';
  os.write(reinterpret_cast<char*>(&itsImpl->itsBytes[0]),
           itsImpl->itsBytes.size());
}

void Pbm::readStream(STD_IO::istream& is) {
DOTRACE("Pbm::readStream");
  if (is.fail())
    {
      throw PbmError("input stream failed before reading pbm file");
    }

  int c = is.get();
  if (c != 'P')
    {
      throw PbmError(fstring("bad magic number while reading pbm file: ", c));
    }

  is >> itsImpl->itsMode;
  DebugEvalNL(itsImpl->itsMode);

  if (itsImpl->itsMode < 1 || itsImpl->itsMode > 6)
    {
      throw PbmError(fstring("invalid mode seen while reading pbm file: ",
                             itsImpl->itsMode));
    }

  while ( isspace(is.peek()) )
    {
      is.get();
    }
  if ( is.peek() == '#' )
    {
      is.ignore(1000, '\n');
    }

  is >> itsImpl->itsImageWidth;
  is >> itsImpl->itsImageHeight;

  DebugEval(itsImpl->itsImageWidth);
  DebugEvalNL(itsImpl->itsImageHeight);

  if (itsImpl->itsMode != 1 && itsImpl->itsMode != 4)
    {
      is >> itsImpl->itsMaxGrey;
    }
  else
    {
      itsImpl->itsMaxGrey = 1;
    }

  DebugEvalNL(itsImpl->itsMaxGrey);

  // read one more character of whitespace from the stream after MaxGrey
  c = is.get();
  if ( !isspace(c) )
    {
      throw PbmError("missing whitespace while reading pbm file");
    }

  switch (itsImpl->itsMode)
    {
    case 1:
    case 4: itsImpl->itsBitsPerPixel = 1; break;
    case 2:
    case 5: itsImpl->itsBitsPerPixel = 8; break;
    case 3:
    case 6: itsImpl->itsBitsPerPixel = 24; break;
    default: Assert(false); break;
    }

  int bytes_per_row = ( (itsImpl->itsImageWidth*itsImpl->itsBitsPerPixel - 1)/8 + 1 );
  itsImpl->itsNumBytes = bytes_per_row * itsImpl->itsImageHeight;
  DebugEvalNL(itsImpl->itsNumBytes);

  itsImpl->itsBytes.resize(itsImpl->itsNumBytes);

  switch (itsImpl->itsMode)
    {
    case 1: parseMode1(is); break;
    case 2: parseMode2(is); break;
    case 3: parseMode3(is); break;
    case 4: parseMode4(is); break;
    case 5: parseMode5(is); break;
    case 6: parseMode6(is); break;
    default: Assert(false); break;
    }
}

void Pbm::parseMode1(STD_IO::istream& is) {
DOTRACE("Pbm::parseMode1");
  int position = 0;
  int val = 0;
  while (is.peek() != EOF && position < itsImpl->itsNumBytes) {
    DebugEval(position);

    is >> val;
    if (val == 0) {
      itsImpl->itsBytes[position] = 0;
    }
    else {
      itsImpl->itsBytes[position] = 255;
    }

    ++position;
  }
}

void Pbm::parseMode2(STD_IO::istream& is) {
DOTRACE("Pbm::parseMode2");
  double conversion = 255.0/double(itsImpl->itsMaxGrey);

  int position = 0;
  int val = 0;
  while (is.peek() != EOF && position < itsImpl->itsNumBytes) {
    DebugEval(position);

    is >> val;
    itsImpl->itsBytes[position] = static_cast<unsigned char>(val * conversion);

    ++position;
  }
}

void Pbm::parseMode3(STD_IO::istream& is) {
DOTRACE("Pbm::parseMode3");
  parseMode2(is);
}

void Pbm::parseMode4(STD_IO::istream& is) {
DOTRACE("Pbm::parseMode4");
  is.read(reinterpret_cast<char*>(&(itsImpl->itsBytes[0])),
          itsImpl->itsNumBytes);
}

void Pbm::parseMode5(STD_IO::istream& is) {
DOTRACE("Pbm::parseMode5");
  is.read(reinterpret_cast<char*>(&(itsImpl->itsBytes[0])),
          itsImpl->itsNumBytes);
}

void Pbm::parseMode6(STD_IO::istream& is) {
DOTRACE("Pbm::parseMode6");
  is.read(reinterpret_cast<char*>(&(itsImpl->itsBytes[0])),
          itsImpl->itsNumBytes);
}

static const char vcid_pbm_cc[] = "$Header$";
#endif // !PBM_CC_DEFINED
