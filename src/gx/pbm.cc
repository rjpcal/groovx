///////////////////////////////////////////////////////////////////////
//
// pbm.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 16:41:07 1999
// written: Thu Jun 24 14:48:51 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PBM_CC_DEFINED
#define PBM_CC_DEFINED

#include "pbm.h"

#include <fstream.h>
#include <strstream.h>
#include <cctype>

#define LOCAL_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

PbmError::PbmError(const string& str) :
  ErrorWithMsg(str)
{
DOTRACE("PbmError::PbmError");
}

Pbm::Pbm(istream& is) {
DOTRACE("Pbm::Pbm(istream&)");
  init(is);
}

Pbm::Pbm(const char* filename) {
DOTRACE("Pbm::Pbm(const char*)");
  ifstream ifs(filename, ios::in|ios::binary);
  if (ifs.fail()) {
	 throw PbmError(string("couldn't open file: ") + filename);
  }
  init(ifs);
}

Pbm::~Pbm () {
DOTRACE("Pbm::~Pbm ");
  DebugEvalNL(itsBytes);
  delete [] itsBytes;
}

void Pbm::grabBytes(unsigned char*& bytes, 
						  int& width, int& height, int& bits_per_pixel) {
DOTRACE("Pbm::grabBytes");
  bytes = itsBytes;
  width = itsImageWidth;
  height = itsImageHeight;
  bits_per_pixel = itsBitsPerPixel;

  // Ownership of the area pointed to by itsBytes has been transferred
  // to the caller of the functions, so we destroy any reference to
  // that area.
  itsBytes = 0;
}

void Pbm::init(istream& is) {
DOTRACE("Pbm::init");
  if (is.fail()) {
	 throw PbmError("input stream failed before reading pbm file");
  }

  itsBytes = 0;

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

  itsNumBytes = ((itsImageWidth*itsBitsPerPixel)/8 + 1) * itsImageHeight;
  DebugEvalNL(itsNumBytes);

  DebugEvalNL((void*) &itsBytes);

  DebugEvalNL((void*) itsBytes);
  delete [] itsBytes;
  itsBytes = new unsigned char[itsNumBytes];
  DebugEvalNL((void*) itsBytes);

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
  is.read(itsBytes, itsNumBytes);
}

void Pbm::parseMode5(istream& is) {
DOTRACE("Pbm::parseMode5");
  is.read(itsBytes, itsNumBytes);  
}

void Pbm::parseMode6(istream& is) {
DOTRACE("Pbm::parseMode6");
  is.read(itsBytes, itsNumBytes);
}

static const char vcid_pbm_cc[] = "$Header$";
#endif // !PBM_CC_DEFINED
