///////////////////////////////////////////////////////////////////////
//
// fixpt.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Mon Sep 10 17:17:37 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_H_DEFINED
#define FIXPT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "visx/grobj.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c FixPt is a subclass of \c GrObj for drawing a simple
 * psychophysics fixation cross. \c FixPt has attributes to control
 * the length of the bars, and the pixel-width of the bars.
 *
 **/
///////////////////////////////////////////////////////////////////////

class FixPt : public GrObj
{
protected:
  /// Construct with initial values for the cross length and pixel-width.
  FixPt (double len=0.1, int wid=1);

public:
  /// Default creator.
  static FixPt* make();

  /// Virtual destructor.
  virtual ~FixPt ();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  static const FieldMap& classFields();

protected:
  virtual Gfx::Rect<double> grGetBoundingBox(Gfx::Canvas& canvas) const;

  virtual void grRender(Gfx::Canvas& canvas) const;

private:
  ////////////////
  // properties //
  ////////////////

  /// Length of crosshairs in GL coordinates.
  double itsLength;

  /// Width of crosshairs in pixels.
  int itsWidth;
};

static const char vcid_fixpt_h[] = "$Header$";
#endif // !FIXPT_H_DEFINED
