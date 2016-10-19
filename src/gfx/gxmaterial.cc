/** @file gfx/gxmaterial.cc GxNode subclass for specifying material
    properties */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jul  5 16:07:42 2002
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#include "gfx/gxmaterial.h"

#include "gfx/glcanvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/error.h"

#include "rutz/trace.h"

namespace
{
  const int GXMATERIAL_SVID = 0;
}

const FieldMap& GxMaterial::classFields()
{
GVX_TRACE("GxMaterial::classFields");
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
GVX_TRACE("GxMaterial::make");
 return new GxMaterial;
}

GxMaterial::GxMaterial() :
  FieldContainer(&sigNodeChanged),
  specularColor(1.0, 1.0, 1.0, 1.0),
  diffuseColor(1.0, 1.0, 1.0, 1.0),
  ambientColor(0.0, 0.0, 0.0, 1.0),
  shininess(50.0)
{
GVX_TRACE("GxMaterial::GxMaterial");
  setFieldMap(GxMaterial::classFields());
}

GxMaterial::~GxMaterial() noexcept
{
GVX_TRACE("GxMaterial::~GxMaterial");
}

io::version_id GxMaterial::class_version_id() const
{
GVX_TRACE("GxMaterial::class_version_id");
  return GXMATERIAL_SVID;
}

void GxMaterial::read_from(io::reader& reader)
{
GVX_TRACE("GxMaterial::read_from");
  readFieldsFrom(reader, classFields());
}

void GxMaterial::write_to(io::writer& writer) const
{
GVX_TRACE("GxMaterial::write_to");
  writeFieldsTo(writer, classFields(), GXMATERIAL_SVID);
}

/////////////
// actions //
/////////////

void GxMaterial::getBoundingCube(Gfx::Bbox&) const
{}

void GxMaterial::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxMaterial::draw");

  GLCanvas* glcanvas = dynamic_cast<GLCanvas*>(&canvas);

  if (glcanvas == 0)
    throw rutz::error("can't use GxLighting with non-OpenGL canvas", SRC_POS);

  glcanvas->material(&specularColor,
                     &diffuseColor,
                     &ambientColor,
                     &shininess);
}
