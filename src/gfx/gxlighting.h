///////////////////////////////////////////////////////////////////////
//
// gxlighting.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul  5 15:15:40 2002
// written: Wed Sep 25 18:52:37 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXLIGHTING_H_DEFINED
#define GXLIGHTING_H_DEFINED

#include "gfx/gbcolor.h"
#include "gfx/gbvec.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

//  ###################################################################
//  ===================================================================

/// GxLighting is a GxNode subclass for manipulating scene lighting.

class GxLighting : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxLighting();

public:
  /// Default creator.
  static GxLighting* make();

  /// Virtual destructor.
  virtual ~GxLighting();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ////////////
  // fields //
  ////////////

private:
  GbColor specularColor;
  GbColor diffuseColor;
  GbColor ambientColor;
  GbVec3<double> position;
  GbVec3<double> spotDirection;
  double attenuation;
  double spotExponent;
  double spotCutoff;

public:
  /// Get GxLightings's FieldMap.
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// A no-op, since lighting doesn't actually draw anything itself.
  virtual void getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const;

  /// Draw the sphere.
  virtual void draw(Gfx::Canvas&) const;

private:
  GxLighting(const GxLighting&);
  GxLighting& operator=(const GxLighting&);
};

static const char vcid_gxlighting_h[] = "$Header$";
#endif // !GXLIGHTING_H_DEFINED
