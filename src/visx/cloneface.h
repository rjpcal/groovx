///////////////////////////////////////////////////////////////////////
//
// cloneface.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Apr 29 09:19:25 1999
// written: Wed Mar 19 12:45:33 2003
// $Id$
//
// CloneFace is a modified version of Face that allows additional
// parameters to be modified, such as the Bezier control points for
// the face outline, and the shape of the eye outline. The default
// behavior of a CloneFace will be identical to that of a Face.
//
///////////////////////////////////////////////////////////////////////

#ifndef CLONEFACE_H_DEFINED
#define CLONEFACE_H_DEFINED

#include "visx/face.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c CloneFace is a subclass of \c Face that adds the ability to
 * control additional paramters for Brunswik face rendering. This is
 * achieved by overriding Face's \c getCtrlPnts(), \c getEyeAspect(),
 * and \c getVertOffset() functions to return CloneFace data
 * members. (In the base class \c Face, these functions just return
 * constant data.)
 *
 **/
///////////////////////////////////////////////////////////////////////

class CloneFace : public Face
{
protected:
  /// Default constructor.
  CloneFace();

public:
  /// Default creation function.
  static CloneFace* make();

  /// Virtual destructor.
  virtual ~CloneFace();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  /// Returns an array of Bezier control points for face outline.
  virtual const double* getCtrlPnts() const;

  /// Returns the aspect ratio of eye outline.
  virtual double getEyeAspect() const;

  /** Returns the amount of vertical offset applied to all internal
      face features. */
  virtual double getVertOffset() const;

  static const FieldMap& classFields();

private:
  /// control aspect ratio of eye outline
  double itsEyeAspect;

  /// amount of vertical offset of all features
  double itsVertOffset;

  double itsCtrlPnts[24];       // Bezier control points for face outline
};

static const char vcid_cloneface_h[] = "$Header$";
#endif // !CLONEFACE_H_DEFINED
