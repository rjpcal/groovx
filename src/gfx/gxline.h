///////////////////////////////////////////////////////////////////////
//
// gxline.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Apr 17 15:53:53 2002
// written: Fri Jul  5 13:41:14 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXLINE_H_DEFINED
#define GXLINE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GBVEC_H_DEFINED)
#include "gfx/gbvec.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GXNODE_H_DEFINED)
#include "gfx/gxnode.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

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
  virtual void getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const;

  /// Draw the line.
  virtual void draw(Gfx::Canvas&) const;

  /// Undraw the line.
  virtual void undraw(Gfx::Canvas&) const;

private:
  GxLine(const GxLine&);
  GxLine& operator=(const GxLine&);
};

static const char vcid_gxline_h[] = "$Header$";
#endif // !GXLINE_H_DEFINED
