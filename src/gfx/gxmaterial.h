///////////////////////////////////////////////////////////////////////
//
// gxmaterial.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul  5 16:06:59 2002
// written: Wed Sep 25 18:52:50 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXMATERIAL_H_DEFINED
#define GXMATERIAL_H_DEFINED

#include "gfx/gbcolor.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

//  ###################################################################
//  ===================================================================

/// GxMaterial is a GxNode subclass for specifying material properties.

class GxMaterial : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxMaterial();

public:
  /// Default creator.
  static GxMaterial* make();

  /// Virtual destructor.
  virtual ~GxMaterial();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ////////////
  // fields //
  ////////////

private:
  GbColor specularColor;
  GbColor diffuseColor;
  GbColor ambientColor;
  double shininess;

public:
  /// Get GxMaterials's FieldMap.
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
  GxMaterial(const GxMaterial&);
  GxMaterial& operator=(const GxMaterial&);
};

static const char vcid_gxmaterial_h[] = "$Header$";
#endif // !GXMATERIAL_H_DEFINED
