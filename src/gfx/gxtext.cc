///////////////////////////////////////////////////////////////////////
//
// gxtext.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Jul  1 11:54:48 1999 (as gtext.cc)
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

#ifndef GXTEXT_CC_DEFINED
#define GXTEXT_CC_DEFINED

#include "gxtext.h"

#include "gfx/canvas.h"
#include "gfx/gxaligner.h"
#include "gfx/gxcache.h"
#include "gfx/gxfont.h"
#include "gfx/gxscaler.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  const IO::VersionId GTEXT_SERIAL_VERSION_ID = 2;
}

///////////////////////////////////////////////////////////////////////
//
// GxText member definitions
//
///////////////////////////////////////////////////////////////////////

GxText* GxText::make()
{
DOTRACE("GxText::make");
  return new GxText;
}

#include "gfx/gxrasterfont.h"

GxText::GxText(const char* text) :
  GxShapeKit(),
  itsFont(GxFont::make("helvetica 34")),
//   itsFont(GxFont::make("vector")),
//   itsFont(GxFont::make("-adobe-helvetica-medium-r-normal--34-240-100-100-p-176-iso8859-1")),
//   itsFont(new GxRasterFont("-adobe-helvetica-medium-r-normal--34-240-100-100-p-176-iso8859-1")),
  itsText(text ? text : ""),
  itsStrokeWidth(2)
{
DOTRACE("GxText::GxText(const char*)");

  setAlignmentMode(GxAligner::CENTER_ON_CENTER);
  setScalingMode(GxScaler::MAINTAIN_ASPECT_SCALING);
  setRenderMode(GxCache::GLCOMPILE);
  setHeight(1.0);
}

GxText::~GxText() throw()
{
DOTRACE("GxText::~GxText");
}

IO::VersionId GxText::serialVersionId() const
{
DOTRACE("GxText::serialVersionId");
  return GTEXT_SERIAL_VERSION_ID;
}

void GxText::readFrom(IO::Reader& reader)
{
DOTRACE("GxText::readFrom");

  reader.ensureReadVersionId("GxText", 2, "Try grsh0.8a4");

  reader.readValue("text", itsText);
  reader.readValue("strokeWidth", itsStrokeWidth);

  // FIXME change to "GxShapeKit" with next version
  reader.readBaseClass("GrObj", IO::makeProxy<GxShapeKit>(this));
}

void GxText::writeTo(IO::Writer& writer) const
{
DOTRACE("GxText::writeTo");

  writer.ensureWriteVersionId("GxText", GTEXT_SERIAL_VERSION_ID, 2,
                              "Try grsh0.8a4");

  writer.writeValue("text", itsText);
  writer.writeValue("strokeWidth", itsStrokeWidth);

  // FIXME change to "GxShapeKit" with next version
  writer.writeBaseClass("GrObj", IO::makeConstProxy<GxShapeKit>(this));
}

const FieldMap& GxText::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("text", &GxText::itsText,
          fstring("0"), fstring("0"), fstring("100"), fstring("1"),
          Field::NEW_GROUP | Field::STRING),
    Field("strokeWidth",
          make_mypair(&GxText::getStrokeWidth, &GxText::setStrokeWidth),
          1, 1, 20, 1),
    Field("font", make_mypair(&GxText::getFont, &GxText::setFont),
          fstring(), fstring(), fstring(), fstring(),
          Field::STRING | Field::TRANSIENT)
  };

  static FieldMap GTEXT_FIELDS(FIELD_ARRAY, &GxShapeKit::classFields());

  return GTEXT_FIELDS;
}

void GxText::setText(const fstring& text)
{
DOTRACE("GxText::setText");
  itsText = text;
  this->sigNodeChanged.emit();
}

const fstring& GxText::getText() const
{
DOTRACE("GxText::getText");
  return itsText;
}

void GxText::setStrokeWidth(int width)
{
DOTRACE("GxText::setStrokeWidth");
  if (width > 0)
    {
      itsStrokeWidth = width;
      setPercentBorder(itsStrokeWidth+4);
      this->sigNodeChanged.emit();
    }
}

int GxText::getStrokeWidth() const
{
DOTRACE("GxText::getStrokeWidth");
  return itsStrokeWidth;
}

void GxText::grGetBoundingBox(Gfx::Bbox& bbox) const
{
DOTRACE("GxText::grGetBoundingBox");

  itsFont->bboxOf(itsText.c_str(), bbox);
}

void GxText::setFont(fstring name)
{
DOTRACE("GxText::setFont");
  itsFont = GxFont::make(name.c_str());
  this->sigNodeChanged.emit();
}

fstring GxText::getFont() const
{
DOTRACE("GxText::getFont");
  return itsFont->fontName();
}

void GxText::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("GxText::grRender");

  Gfx::MatrixSaver msaver(canvas);

  Gfx::AttribSaver asaver(canvas);

  canvas.setLineWidth(itsStrokeWidth);
  canvas.drawText(itsText, *itsFont);
}

static const char vcid_gxtext_cc[] = "$Header$";
#endif // !GXTEXT_CC_DEFINED
