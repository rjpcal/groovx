///////////////////////////////////////////////////////////////////////
//
// pbm.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 16:41:07 1999
// written: Fri Sep 24 19:12:23 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PBM_CC_DEFINED
#define PBM_CC_DEFINED

#include "pbm.h"

#include <fstream.h>
#include <strstream.h>
#include <cctype>

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

PbmError::PbmError(const string& str) :
  ErrorWithMsg(str)
{
DOTRACE("PbmError::PbmError");
}

void Pbm::init() {
DOTRACE("Pbm::init");
  itsMode = 0;
  itsImageWidth = 0;
  itsImageHeight = 0;
  itsMaxGrey = 0;
  itsBitsPerPixel = 0;
  itsNumBytes = 0;
  itsBytes.resize(1);
}

Pbm::Pbm(const vector<unsigned char>& bytes,
			int width, int height, int bits_per_pixel)
{
  setBytes(bytes, width, height, bits_per_pixel);
}

Pbm::Pbm(istream& is) {
DOTRACE("Pbm::Pbm(istream&)");
  init();
  readStream(is);
}

Pbm::Pbm(const char* filename) {
DOTRACE("Pbm::Pbm(const char*)");
  init();

  ifstream ifs(filename, ios::in|ios::binary);
  if (ifs.fail()) {
	 throw PbmError(string("couldn't open file: ") + filename);
  }
  readStream(ifs);
}

Pbm::~Pbm () {
DOTRACE("Pbm::~Pbm ");
}

void Pbm::grabBytes(vector<unsigned char>& bytes,
						  int& width, int& height, int& bits_per_pixel) {
DOTRACE("Pbm::grabBytes");
  itsBytes.swap(bytes);
  itsBytes.resize(1);
  width = itsImageWidth;
  height = itsImageHeight;
  bits_per_pixel = itsBitsPerPixel;
}

void Pbm::setBytes(const vector<unsigned char>& bytes,
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
	 throw PbmError("invalid bits_per_pixel value");
	 break;
  }
}

void Pbm::write(const char* filename) const {
DOTRACE("Pbm::write");
  ofstream ofs(filename);
  write(ofs);
}

void Pbm::write(ostream& os) const {
DOTRACE("Pbm::write");
  os << 'P' << itsMode << ' ' 
	  << itsImageWidth << ' ' << itsImageHeight << '\n';
  os.write(&itsBytes[0], itsBytes.size());
}

void Pbm::readStream(istream& is) {
DOTRACE("Pbm::readStream");
  if (is.fail()) {
	 throw PbmError("input stream failed before reading pbm file");
  }

  int c = is.get();
  if (c != 'P') {
	 throw PbmError("bad magic number while reading pbm file");
  }

  is >> itsMode;
  DebugEvalNL(itsMode);

  if (itsMode < 1 || itsMode > 6) {
	 throw PbmError("invalid mode seen while reading pbm file");
  }

  while ( isspace(is.peek()) ) {
	 is.get();
  }
  if ( is.peek() == '#' ) {
	 is.ignore(1000, '\n');
  }

  is >> itsImageWidth;
  is >> itsImageHeight;

  DebugEval(itsImageWidth);
  DebugEvalNL(itsImageHeight);

  if (itsMode != 1 && itsMode != 4) {
	 is >> itsMaxGrey;
  }
  else {
	 itsMaxGrey = 1;
  }
  
  DebugEvalNL(itsMaxGrey);

  // read one more character of whitespace from the stream after MaxGrey
  c = is.get();
  if ( !isspace(c) ) {
	 throw PbmError("missing whitespace while reading pbm file");
  }

  switch (itsMode) {
  case 1:
  case 4: itsBitsPerPixel = 1; break;
  case 2:
  case 5: itsBitsPerPixel = 8; break;
  case 3:
  case 6: itsBitsPerPixel = 24; break;
  default: Assert(false); break;
  }
  
  int bytes_per_row = ( (itsImageWidth*itsBitsPerPixel - 1)/8 + 1 );
  itsNumBytes = bytes_per_row * itsImageHeight;
  DebugEvalNL(itsNumBytes);

  itsBytes.resize(itsNumBytes);

  switch (itsMode) {
  case 1: parseMode1(is); break;
  case 2: parseMode2(is); break;
  case 3: parseMode3(is); break;
  case 4: parseMode4(is); break;
  case 5: parseMode5(is); break;
  case 6: parseMode6(is); break;
  default: Assert(false); break;
  }
}

void Pbm::parseMode1(istream& is) {
DOTRACE("Pbm::parseMode1");
  int position = 0;
  int val = 0;
  while (is.peek() != EOF && position < itsNumBytes) {
	 DebugEval(position);

	 is >> val;
	 if (val == 0) {
		itsBytes[position] = 0;
	 }
	 else {
		itsBytes[position] = 255;
	 }
	 
	 ++position;
  } 
}

void Pbm::parseMode2(istream& is) {
DOTRACE("Pbm::parseMode2");
  double conversion = 255.0/double(itsMaxGrey);
  
  int position = 0;
  int val = 0;
  while (is.peek() != EOF && position < itsNumBytes) {
	 DebugEval(position);

	 is >> val;
	 itsBytes[position] = static_cast<unsigned char>(val * conversion);
	 
	 ++position;
  } 
}

void Pbm::parseMode3(istream& is) {
DOTRACE("Pbm::parseMode3");
  parseMode2(is);
}

void Pbm::parseMode4(istream& is) {
DOTRACE("Pbm::parseMode4");
  is.read(&(itsBytes[0]), itsNumBytes);
}

void Pbm::parseMode5(istream& is) {
DOTRACE("Pbm::parseMode5");
  is.read(&(itsBytes[0]), itsNumBytes);  
}

void Pbm::parseMode6(istream& is) {
DOTRACE("Pbm::parseMode6");
  is.read(&(itsBytes[0]), itsNumBytes);
}

static const char vcid_pbm_cc[] = "$Header$";
#endif // !PBM_CC_DEFINED
