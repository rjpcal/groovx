///////////////////////////////////////////////////////////////////////
//
// gtext.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 11:54:48 1999
// written: Wed Nov 13 22:08:25 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXT_CC_DEFINED
#define GTEXT_CC_DEFINED

#include "visx/gtext.h"

#include "gfx/canvas.h"
#include "gfx/gxaligner.h"
#include "gfx/gxcache.h"
#include "gfx/gxfont.h"
#include "gfx/gxscaler.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include <GL/gl.h>

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  const IO::VersionId GTEXT_SERIAL_VERSION_ID = 2;
}

///////////////////////////////////////////////////////////////////////
//
// Gtext member definitions
//
///////////////////////////////////////////////////////////////////////

Gtext* Gtext::make()
{
DOTRACE("Gtext::make");
  return new Gtext;
}

#include "gfx/gxrasterfont.h"

Gtext::Gtext(const char* text) :
  GrObj(),
  itsFont(GxFont::make("helvetica 34")),
//   itsFont(GxFont::make("vector")),
//   itsFont(GxFont::make("-adobe-helvetica-medium-r-normal--34-240-100-100-p-176-iso8859-1")),
//   itsFont(new GxRasterFont("-adobe-helvetica-medium-r-normal--34-240-100-100-p-176-iso8859-1")),
  itsText(text ? text : ""),
  itsStrokeWidth(2)
{
DOTRACE("Gtext::Gtext(const char*)");

  setAlignmentMode(GxAligner::CENTER_ON_CENTER);
  setScalingMode(GxScaler::MAINTAIN_ASPECT_SCALING);
  setRenderMode(GxCache::GLCOMPILE);
  setHeight(1.0);
}

Gtext::~Gtext()
{
DOTRACE("Gtext::~Gtext");
}

IO::VersionId Gtext::serialVersionId() const
{
DOTRACE("Gtext::serialVersionId");
  return GTEXT_SERIAL_VERSION_ID;
}

void Gtext::readFrom(IO::Reader* reader)
{
DOTRACE("Gtext::readFrom");

  reader->ensureReadVersionId("Gtext", 2, "Try grsh0.8a4");

  reader->readValue("text", itsText);
  reader->readValue("strokeWidth", itsStrokeWidth);

  reader->readBaseClass("GrObj", IO::makeProxy<GrObj>(this));
}

void Gtext::writeTo(IO::Writer* writer) const
{
DOTRACE("Gtext::writeTo");

  writer->ensureWriteVersionId("Gtext", GTEXT_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a4");

  writer->writeValue("text", itsText);
  writer->writeValue("strokeWidth", itsStrokeWidth);

  writer->writeBaseClass("GrObj", IO::makeConstProxy<GrObj>(this));
}

const FieldMap& Gtext::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("text", &Gtext::itsText,
          fstring("0"), fstring("0"), fstring("100"), fstring("1"),
          Field::NEW_GROUP | Field::STRING),
    Field("strokeWidth",
          make_mypair(&Gtext::getStrokeWidth, &Gtext::setStrokeWidth),
          1, 1, 20, 1),
    Field("font", make_mypair(&Gtext::getFont, &Gtext::setFont),
          fstring(), fstring(), fstring(), fstring(),
          Field::STRING | Field::TRANSIENT)
  };

  static FieldMap GTEXT_FIELDS(FIELD_ARRAY, &GrObj::classFields());

  return GTEXT_FIELDS;
}

void Gtext::setText(const fstring& text)
{
DOTRACE("Gtext::setText");
  itsText = text;
  this->sigNodeChanged.emit();
}

const fstring& Gtext::getText() const
{
DOTRACE("Gtext::getText");
  return itsText;
}

void Gtext::setStrokeWidth(int width)
{
DOTRACE("Gtext::setStrokeWidth");
  if (width > 0)
    {
      itsStrokeWidth = width;
      setPixelBorder(itsStrokeWidth+4);
      this->sigNodeChanged.emit();
    }
}

int Gtext::getStrokeWidth() const
{
DOTRACE("Gtext::getStrokeWidth");
  return itsStrokeWidth;
}

Gfx::Rect<double> Gtext::grGetBoundingBox(Gfx::Canvas& canvas) const
{
DOTRACE("Gtext::grGetBoundingBox");

  return itsFont->sizeOf(itsText.c_str(), canvas);
}

void Gtext::setFont(fstring name)
{
DOTRACE("Gtext::setFont");
  itsFont.reset(GxFont::make(name.c_str()));
  this->sigNodeChanged.emit();
}

fstring Gtext::getFont() const
{
DOTRACE("Gtext::getFont");
  return itsFont->fontName();
}

void Gtext::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("Gtext::grRender");

  Gfx::MatrixSaver msaver(canvas);

  Gfx::AttribSaver asaver(canvas);

  canvas.setLineWidth(itsStrokeWidth);
  glRasterPos2d( 0.0, 0.0 );
  glListBase( itsFont->listBase() );
  glCallLists( itsText.length(), GL_BYTE, itsText.c_str() );
}

static const char vcid_gtext_cc[] = "$Header$";
#endif // !GTEXT_CC_DEFINED
