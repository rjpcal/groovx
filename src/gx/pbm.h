///////////////////////////////////////////////////////////////////////
//
// pbm.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 16:41:06 1999
// written: Fri Sep 24 18:41:23 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PBM_H_DEFINED
#define PBM_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

class istream;

class PbmError : public ErrorWithMsg {
public:
  PbmError(const string& str="");
};

class Pbm {
public:
  Pbm(vector<unsigned char>& bytes,
		int width, int height, int bits_per_pixel);
  Pbm(istream& is);
  Pbm(const char* filename);
  virtual ~Pbm();

  // This function transfers ownership of itsBytes to the caller of
  // the function.
  void grabBytes(vector<unsigned char>& bytes,
 					  int& width, int& height, int& bits_per_pixel);					  

  void setBytes(const vector<unsigned char>& bytes,
					 int width, int height, int bits_per_pixel);

  void write(ostream& os) const;
  void write(const char* filename) const;

private:
  void init();

  void readStream(istream& is);

  void parseMode1(istream& is);
  void parseMode2(istream& is);
  void parseMode3(istream& is);
  void parseMode4(istream& is);
  void parseMode5(istream& is);
  void parseMode6(istream& is);

  int itsMode;
  int itsImageWidth;
  int itsImageHeight;
  int itsMaxGrey;
  int itsBitsPerPixel;

  int itsNumBytes;
  vector<unsigned char> itsBytes;
};

static const char vcid_pbm_h[] = "$Header$";
#endif // !PBM_H_DEFINED
