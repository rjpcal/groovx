/** @file gfx/gxtext.cc GxShapeKit subclass for rendering text */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Jul  1 11:54:48 1999 (as gtext.cc)
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

#include "gxtext.h"

#include "gfx/canvas.h"
#include "gfx/gxaligner.h"
#include "gfx/gxcache.h"
#include "gfx/gxfactory.h"
#include "gfx/gxfont.h"
#include "gfx/gxscaler.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

namespace
{
  const io::version_id GTEXT_SVID = 3;
}

///////////////////////////////////////////////////////////////////////
//
// GxText member definitions
//
///////////////////////////////////////////////////////////////////////

GxText* GxText::make()
{
GVX_TRACE("GxText::make");
  return new GxText;
}

#include "gfx/gxrasterfont.h"

GxText::GxText(const char* text) :
  GxShapeKit(),
  itsFont(GxFactory::makeFont("helvetica:34")),
//   itsFont(GxFactory::makeFont("vector")),
//   itsFont(GxFactory::makeFont("-adobe-helvetica-medium-r-normal--34-240-100-100-p-176-iso8859-1")),
//   itsFont(new GxRasterFont("-adobe-helvetica-medium-r-normal--34-240-100-100-p-176-iso8859-1")),
  itsText(text ? text : ""),
  itsStrokeWidth(2)
{
GVX_TRACE("GxText::GxText(const char*)");

  setAlignmentMode(GxAligner::CENTER_ON_CENTER);
  setScalingMode(GxScaler::MAINTAIN_ASPECT_SCALING);
  setRenderMode(GxCache::GLCOMPILE);
  setHeight(1.0);
}

GxText::~GxText() noexcept
{
GVX_TRACE("GxText::~GxText");
}

io::version_id GxText::class_version_id() const
{
GVX_TRACE("GxText::class_version_id");
  return GTEXT_SVID;
}

void GxText::read_from(io::reader& reader)
{
GVX_TRACE("GxText::read_from");

  reader.ensure_version_id("GxText", 3,
                           "Try cvs tag xml_conversion_20040526",
                           SRC_POS);

  reader.read_value("text", itsText);
  reader.read_value("strokeWidth", itsStrokeWidth);

  reader.read_base_class("GxShapeKit", io::make_proxy<GxShapeKit>(this));
}

void GxText::write_to(io::writer& writer) const
{
GVX_TRACE("GxText::write_to");

  writer.ensure_output_version_id("GxText", GTEXT_SVID, 3,
                              "Try groovx0.8a4", SRC_POS);

  writer.write_value("text", itsText);
  writer.write_value("strokeWidth", itsStrokeWidth);

  writer.write_base_class("GxShapeKit", io::make_const_proxy<GxShapeKit>(this));
}

const FieldMap& GxText::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("text", &GxText::itsText,
          fstring("0"), fstring("0"), fstring("100"), fstring("1"),
          Field::NEW_GROUP | Field::STRING),
    Field("strokeWidth",
          &GxText::getStrokeWidth, &GxText::setStrokeWidth,
          1, 1, 20, 1),
    Field("font", &GxText::getFont, &GxText::setFont,
          fstring(), fstring(), fstring(), fstring(),
          Field::STRING | Field::TRANSIENT)
  };

  static FieldMap GTEXT_FIELDS(FIELD_ARRAY, &GxShapeKit::classFields());

  return GTEXT_FIELDS;
}

void GxText::setText(const fstring& text)
{
GVX_TRACE("GxText::setText");
  itsText = text;
  this->sigNodeChanged.emit();
}

const fstring& GxText::getText() const
{
GVX_TRACE("GxText::getText");
  return itsText;
}

void GxText::setStrokeWidth(int width)
{
GVX_TRACE("GxText::setStrokeWidth");
  if (width > 0)
    {
      itsStrokeWidth = width;
      setPercentBorder(itsStrokeWidth+4);
      this->sigNodeChanged.emit();
    }
}

int GxText::getStrokeWidth() const
{
GVX_TRACE("GxText::getStrokeWidth");
  return itsStrokeWidth;
}

void GxText::grGetBoundingBox(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxText::grGetBoundingBox");

  itsFont->bboxOf(itsText.c_str(), bbox);
}

void GxText::setFont(fstring name)
{
GVX_TRACE("GxText::setFont");
  itsFont = GxFactory::makeFont(name.c_str());
  this->sigNodeChanged.emit();
}

fstring GxText::getFont() const
{
GVX_TRACE("GxText::getFont");
  return itsFont->fontName();
}

void GxText::grRender(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxText::grRender");

  Gfx::MatrixSaver msaver(canvas);

  Gfx::AttribSaver asaver(canvas);

  canvas.setLineWidth(itsStrokeWidth);
  itsFont->drawText(itsText.c_str(), canvas);
}
