///////////////////////////////////////////////////////////////////////
//
// cloneface.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Apr 29 09:19:25 1999
// written: Thu May 10 12:04:41 2001
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

#include "face.h"

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

class CloneFace : public Face {
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

  /// control aspect ratio of eye outline
  TField<double> eyeAspect;

  /// amount of vertical offset of all features
  TField<double> vertOffset;

  static const FieldMap& classFields();

private:
  double itsCtrlPnts[24];		  // Bezier control points for face outline
};

static const char vcid_cloneface_h[] = "$Header$";
#endif // !CLONEFACE_H_DEFINED
