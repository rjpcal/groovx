///////////////////////////////////////////////////////////////////////
//
// gxline.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Apr 17 15:53:53 2002
// written: Mon Jan 13 11:01:39 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXLINE_H_DEFINED
#define GXLINE_H_DEFINED

#include "gfx/gbvec.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * The GxLine class offers a simple way to render a line onscreen.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GxLine : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxLine();

public:
  /// Default creator.
  static GxLine* make();

  /// Virtual destructor.
  virtual ~GxLine();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ////////////
  // fields //
  ////////////

  /// The start point.
  GbVec3<double> start;

  /// The stop point.
  GbVec3<double> stop;

private:
  /// If itsWidth is less than zero, then we use the current default width.
  int itsWidth;

public:
  /// Get GxLine's FieldMap.
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Extend the bounding cube for this line.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  /// Draw the line.
  virtual void draw(Gfx::Canvas&) const;

private:
  GxLine(const GxLine&);
  GxLine& operator=(const GxLine&);
};

static const char vcid_gxline_h[] = "$Header$";
#endif // !GXLINE_H_DEFINED
