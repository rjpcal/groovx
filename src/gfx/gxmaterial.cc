///////////////////////////////////////////////////////////////////////
//
// gxmaterial.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jul  5 16:07:42 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GXMATERIAL_CC_DEFINED
#define GXMATERIAL_CC_DEFINED

#include "gfx/gxmaterial.h"

#include "gfx/glcanvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"

const FieldMap& GxMaterial::classFields()
{
DOTRACE("GxMaterial::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("specularColor", Field::ValueType(), &GxMaterial::specularColor,
          "0.0 0.0 0.0 0.0", "0.0 0.0 0.0 0.0", "1.0 1.0 1.0 1.0",
          "0.01 0.01 0.01 0.01", Field::NEW_GROUP | Field::MULTI),
    Field("diffuseColor", Field::ValueType(), &GxMaterial::diffuseColor,
          "0.0 0.0 0.0 0.0", "0.0 0.0 0.0 0.0", "1.0 1.0 1.0 1.0",
          "0.01 0.01 0.01 0.01", Field::MULTI),
    Field("ambientColor", Field::ValueType(), &GxMaterial::ambientColor,
          "0.0 0.0 0.0 0.0", "0.0 0.0 0.0 0.0", "1.0 1.0 1.0 1.0",
          "0.01 0.01 0.01 0.01", Field::MULTI),
    Field("shininess", &GxMaterial::shininess, 50.0, 0.0, 128.0, 1.0)
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

GxMaterial* GxMaterial::make()
{
DOTRACE("GxMaterial::make");
 return new GxMaterial;
}

GxMaterial::GxMaterial() :
  FieldContainer(&sigNodeChanged),
  specularColor(1.0, 1.0, 1.0, 1.0),
  diffuseColor(1.0, 1.0, 1.0, 1.0),
  ambientColor(0.0, 0.0, 0.0, 1.0),
  shininess(50.0)
{
DOTRACE("GxMaterial::GxMaterial");
  setFieldMap(GxMaterial::classFields());
}

GxMaterial::~GxMaterial() throw()
{
DOTRACE("GxMaterial::~GxMaterial");
}

void GxMaterial::readFrom(IO::Reader* reader)
{
DOTRACE("GxMaterial::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxMaterial::writeTo(IO::Writer* writer) const
{
DOTRACE("GxMaterial::writeTo");
  writeFieldsTo(writer, classFields());
}

/////////////
// actions //
/////////////

void GxMaterial::getBoundingCube(Gfx::Bbox&) const
{}

void GxMaterial::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxMaterial::draw");

  GLCanvas* glcanvas = dynamic_cast<GLCanvas*>(&canvas);

  if (glcanvas == 0)
    throw Util::Error("can't use GxLighting with non-OpenGL canvas");

  glcanvas->material(&specularColor,
                     &diffuseColor,
                     &ambientColor,
                     &shininess);
}

static const char vcid_gxmaterial_cc[] = "$Header$";
#endif // !GXMATERIAL_CC_DEFINED
