///////////////////////////////////////////////////////////////////////
//
// gxsphere.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul  5 14:18:59 2002
// written: Fri Jul  5 14:19:57 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSPHERE_H_DEFINED
#define GXSPHERE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GXNODE_H_DEFINED)
#include "gfx/gxnode.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

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
  /// Get GxLine's FieldMap.
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Get the bounding cube for the sphere.
  virtual void getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const;

  /// Draw the sphere.
  virtual void draw(Gfx::Canvas&) const;

private:
  GxSphere(const GxSphere&);
  GxSphere& operator=(const GxSphere&);
};

static const char vcid_gxsphere_h[] = "$Header$";
#endif // !GXSPHERE_H_DEFINED
