///////////////////////////////////////////////////////////////////////
//
// gxline.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Apr 17 15:53:53 2002
// written: Wed Apr 17 16:01:24 2002
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
  int itsWidth;

public:
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Translate, scale, and rotate.
  virtual void draw(Gfx::Canvas&) const;

  /** Redo most recent draw(). (This is useful if the behavior of
      draw() changes from one call to the next.) */
  virtual void undraw(Gfx::Canvas&) const;

private:
  GxLine(const GxLine&);
  GxLine& operator=(const GxLine&);
};

static const char vcid_gxline_h[] = "$Header$";
#endif // !GXLINE_H_DEFINED
