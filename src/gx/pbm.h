///////////////////////////////////////////////////////////////////////
//
// pbm.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 16:41:06 1999
// written: Sun Feb 20 22:02:25 2000
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

#ifndef BMAPDATA_H_DEFINED
#include "bmapdata.h"
#endif

class istream;

/** This exception class will be thrown by \c Pbm if there is an error
    during parsing of the bitmap data. */
class PbmError : public ErrorWithMsg {
public:
  /// Construct with an appropriate error message \a msg.
  PbmError(const string& msg="");
};

class Pbm {
public:
  /** Construct from the specified parameters. This has the same
      effect as calling \c setBytes() with the same arguments. */
  Pbm(const vector<unsigned char>& bytes,
		int width, int height, int bits_per_pixel);

  /** Construct by copying from of \a data. This has the same effect
      as calling \c setBytes() with the same argument. */
  Pbm(const BmapData& data);

  /// Construct by reading PBM format data from the \c istream \a is.
  Pbm(istream& is);

  /// Construct by reading PBM format data from the file \a filename.
  Pbm(const char* filename);

  /// Virtual destructor.
  virtual ~Pbm();

  /** This function allows an external caller to take ownership of the
      bitmap data help by the \c Pbm object. The internal bitmap data
      are swapped into \a bytes, and the other reference parameters
      are set appropriately. The \c Pbm's internal state is then
      reinitialized, to contain an empty bitmap. */
  void grabBytes(vector<unsigned char>& bytes,
 					  int& width, int& height, int& bits_per_pixel);					  

  /** The data are copied from \a bytes into the \c Pbm object. */
  void setBytes(const vector<unsigned char>& bytes,
					 int width, int height, int bits_per_pixel);

  /** Bitmap data are copied from \a data into the \c Pbm object. */
  void setBytes(const BmapData& data);

  /** The \c Pbm object's internal representation is swapped with that
      of \a data. */
  void swapInto(BmapData& data);

  /// Write PBM format data to the \c ostream \a os.
  void write(ostream& os) const;

  /// Write PBM format data to the file \a filename.
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
