///////////////////////////////////////////////////////////////////////
//
// gxsphere.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jul  5 14:18:59 2002
// written: Wed Mar 19 12:45:58 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSPHERE_H_DEFINED
#define GXSPHERE_H_DEFINED

#include "gfx/gxnode.h"

#include "io/fields.h"

//  ###################################################################
//  ===================================================================

/// GxSphere is a GxNode subclass for drawing spheres.

class GxSphere : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxSphere();

public:
  /// Default creator.
  static GxSphere* make();

  /// Virtual destructor.
  virtual ~GxSphere();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ////////////
  // fields //
  ////////////

private:
  double itsRadius;
  int itsSlices;
  int itsStacks;
  bool itsFilled;

public:
  /// Get GxSphere's FieldMap.
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Get the bounding cube for the sphere.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  /// Draw the sphere.
  virtual void draw(Gfx::Canvas&) const;

private:
  GxSphere(const GxSphere&);
  GxSphere& operator=(const GxSphere&);
};

static const char vcid_gxsphere_h[] = "$Header$";
#endif // !GXSPHERE_H_DEFINED
