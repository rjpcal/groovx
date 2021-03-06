/** @file gfx/gxlighting.cc GxNode subclass for manipulating scene
    lighting */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Jul  5 15:17:06 2002
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "gfx/gxlighting.h"

#include "gfx/glcanvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/error.h"

#include "rutz/trace.h"

namespace
{
  const int GXLIGHTING_SVID = 0;
}

const FieldMap& GxLighting::classFields()
{
GVX_TRACE("GxLighting::classFields");
  static const Field FIELD_ARRAY[] =
  {
    Field("specularColor", Field::ValueType(), &GxLighting::specularColor,
          "0.0 0.0 0.0 0.0", "0.0 0.0 0.0 0.0", "1.0 1.0 1.0 1.0",
          "0.01 0.01 0.01 0.01", Field::NEW_GROUP | Field::MULTI),
    Field("diffuseColor", Field::ValueType(), &GxLighting::diffuseColor,
          "0.0 0.0 0.0 0.0", "0.0 0.0 0.0 0.0", "1.0 1.0 1.0 1.0",
          "0.01 0.01 0.01 0.01", Field::MULTI),
    Field("ambientColor", Field::ValueType(), &GxLighting::ambientColor,
          "0.0 0.0 0.0 0.0", "0.0 0.0 0.0 0.0", "1.0 1.0 1.0 1.0",
          "0.01 0.01 0.01 0.01", Field::MULTI),
    Field("position", Field::ValueType(), &GxLighting::position,
          "1.0 1.0 1.0", "-1.0 -1.0 -1.0", "1.0 1.0 1.0",
          "0.01 0.01 0.01", Field::NEW_GROUP | Field::MULTI),
    Field("spotDirection", Field::ValueType(), &GxLighting::spotDirection,
          "1.0 1.0 1.0", "-1.0 -1.0 -1.0", "1.0 1.0 1.0",
          "0.01 0.01 0.01", Field::MULTI),
    Field("attenuation", &GxLighting::attenuation, 0.0, 0.0, 1.0, 0.01),
    Field("spotExponent", &GxLighting::spotExponent, 0.0, 0.0, 128.0, 1.0),
    Field("spotCutoff", &GxLighting::spotCutoff, 180.0, 0.0, 180.0, 1.0),
  };

  static FieldMap FIELD_MAP(FIELD_ARRAY);

  return FIELD_MAP;
}

GxLighting* GxLighting::make()
{
GVX_TRACE("GxLighting::make");
 return new GxLighting;
}

GxLighting::GxLighting() :
  FieldContainer(&sigNodeChanged),
  specularColor(1.0, 1.0, 1.0, 1.0),
  diffuseColor(1.0, 1.0, 1.0, 1.0),
  ambientColor(0.0, 0.0, 0.0, 1.0),
  position(1.0, 1.0, 1.0),
  spotDirection(-1.0, -1.0, -1.0),
  attenuation(0.0),
  spotExponent(0.0),
  spotCutoff(180.0)
{
GVX_TRACE("GxLighting::GxLighting");
  setFieldMap(GxLighting::classFields());
}

GxLighting::~GxLighting() noexcept
{
GVX_TRACE("GxLighting::~GxLighting");
}

io::version_id GxLighting::class_version_id() const
{
GVX_TRACE("GxLighting::class_version_id");
  return GXLIGHTING_SVID;
}

void GxLighting::read_from(io::reader& reader)
{
GVX_TRACE("GxLighting::read_from");
  readFieldsFrom(reader, classFields());
}

void GxLighting::write_to(io::writer& writer) const
{
GVX_TRACE("GxLighting::write_to");
  writeFieldsTo(writer, classFields(), GXLIGHTING_SVID);
}

/////////////
// actions //
/////////////

void GxLighting::getBoundingCube(Gfx::Bbox&) const
{}

void GxLighting::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxLighting::draw");

  GLCanvas* glcanvas = dynamic_cast<GLCanvas*>(&canvas);

  if (glcanvas == nullptr)
    throw rutz::error("can't use GxLighting with non-OpenGL canvas", SRC_POS);

  glcanvas->light(0,
                  &specularColor,
                  &diffuseColor,
                  &ambientColor,
                  &position,
                  &spotDirection,
                  attenuation,
                  spotExponent,
                  spotCutoff);
}
