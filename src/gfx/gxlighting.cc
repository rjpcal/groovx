///////////////////////////////////////////////////////////////////////
//
// gxlighting.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jul  5 15:17:06 2002
// written: Wed Mar 19 12:46:01 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXLIGHTING_CC_DEFINED
#define GXLIGHTING_CC_DEFINED

#include "gfx/gxlighting.h"

#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"

const FieldMap& GxLighting::classFields()
{
DOTRACE("GxLighting::classFields");
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
DOTRACE("GxLighting::make");
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
DOTRACE("GxLighting::GxLighting");
  setFieldMap(GxLighting::classFields());
}

GxLighting::~GxLighting()
{
DOTRACE("GxLighting::~GxLighting");
}

void GxLighting::readFrom(IO::Reader* reader)
{
DOTRACE("GxLighting::readFrom");
  readFieldsFrom(reader, classFields());
}

void GxLighting::writeTo(IO::Writer* writer) const
{
DOTRACE("GxLighting::writeTo");
  writeFieldsTo(writer, classFields());
}

/////////////
// actions //
/////////////

void GxLighting::getBoundingCube(Gfx::Bbox&) const
{}

#include <GL/gl.h>

void GxLighting::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxLighting::draw");

  const GLfloat specular[] =
    { specularColor.r(), specularColor.g(), specularColor.b(), specularColor.a() };
  const GLfloat diffuse[] =
    { diffuseColor.r(), diffuseColor.g(), diffuseColor.b(), diffuseColor.a() };
  const GLfloat ambient[] =
    { ambientColor.r(), ambientColor.g(), ambientColor.b(), ambientColor.a() };

  const GLfloat w = (attenuation == 0.0) ? 0.0 : 1.0;
  const GLfloat mul = (attenuation == 0.0) ? 1.0 : (1.0/attenuation);

  const GLfloat fposition[] =
    { mul*position.x(), mul*position.y(), mul*position.z(), w };

  const GLfloat fdirection[] =
    { spotDirection.x(), spotDirection.y(), spotDirection.z(), 0.0 };

  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_POSITION, fposition);
  glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spotExponent);
  glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotCutoff);
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, fdirection);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
}

static const char vcid_gxlighting_cc[] = "$Header$";
#endif // !GXLIGHTING_CC_DEFINED
