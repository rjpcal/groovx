///////////////////////////////////////////////////////////////////////
//
// gxmaterial.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul  5 16:07:42 2002
// written: Tue Nov 19 12:55:40 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXMATERIAL_CC_DEFINED
#define GXMATERIAL_CC_DEFINED

#include "gfx/gxmaterial.h"

#include "gfx/canvas.h"

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

GxMaterial::~GxMaterial()
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

#include <GL/gl.h>

void GxMaterial::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxMaterial::draw");

  const GLfloat specular[] =
    { specularColor.r(), specularColor.g(), specularColor.b(), specularColor.a() };
  const GLfloat diffuse[] =
    { diffuseColor.r(), diffuseColor.g(), diffuseColor.b(), diffuseColor.a() };
  const GLfloat ambient[] =
    { ambientColor.r(), ambientColor.g(), ambientColor.b(), ambientColor.a() };

  glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);

  glEnable(GL_DEPTH_TEST);
}

static const char vcid_gxmaterial_cc[] = "$Header$";
#endif // !GXMATERIAL_CC_DEFINED
