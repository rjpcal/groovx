///////////////////////////////////////////////////////////////////////
//
// maskhatch.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Sep 23 15:49:58 1999
// written: Wed Sep 25 19:02:45 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKHATCH_H_DEFINED
#define MASKHATCH_H_DEFINED

#include "visx/grobj.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c MaskHatch is a subclass of \c GrObj for rendering a visual mask
 * consisting of hatched lines. The pattern will contain vertical,
 * horizontal, and diagonal lines at 45-degrees left, and 45-degrees
 * right.
 *
 **/
///////////////////////////////////////////////////////////////////////

class MaskHatch : public GrObj
{
protected:
  /// Default constructor.
  MaskHatch();

public:
  /// Default creator.
  static MaskHatch* make();

  /// Virtual destructor.
  virtual ~MaskHatch();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  static const FieldMap& classFields();

  ////////////////
  // properties //
  ////////////////

private:
  /// The number of lines that will be rendered in each direction.
  int itsNumLines;

  /// The pixel-width of each line.
  int itsLineWidth;

  // To connect to Signal's.
  void update();

protected:
  virtual Gfx::Rect<double> grGetBoundingBox(Gfx::Canvas& canvas) const;

  virtual void grRender(Gfx::Canvas& canvas) const;
};

static const char vcid_maskhatch_h[] = "$Header$";
#endif // !MASKHATCH_H_DEFINED
