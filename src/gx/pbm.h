///////////////////////////////////////////////////////////////////////
//
// pbm.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 16:41:06 1999
// written: Wed Mar 29 14:07:36 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PBM_H_DEFINED
#define PBM_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

class istream;
class ostream;
class BmapData;

/** This exception class will be thrown by \c Pbm if there is an error
    during parsing of the bitmap data. */
class PbmError : public ErrorWithMsg {
public:
  /// Default constructor.
  PbmError();
  /// Construct with an appropriate error message \a msg.
  PbmError(const char* msg);
};

class Pbm {
public:
  /** Construct by copying from of \a data. This has the same effect
      as calling \c setBytes() with the same argument. */
  Pbm(const BmapData& data);

  /// Construct by reading PBM format data from the \c istream \a is.
  Pbm(istream& is);

  /// Construct by reading PBM format data from the file \a filename.
  Pbm(const char* filename);

  /// Virtual destructor.
  virtual ~Pbm();

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
  void readStream(istream& is);

  void parseMode1(istream& is);
  void parseMode2(istream& is);
  void parseMode3(istream& is);
  void parseMode4(istream& is);
  void parseMode5(istream& is);
  void parseMode6(istream& is);

  Pbm(const Pbm&);
  Pbm& operator=(const Pbm&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_pbm_h[] = "$Header$";
#endif // !PBM_H_DEFINED
