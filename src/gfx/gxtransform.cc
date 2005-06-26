///////////////////////////////////////////////////////////////////////
//
// gxtransform.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov 20 15:15:50 2002
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef GROOVX_GFX_GXTRANSFORM_CC_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXTRANSFORM_CC_UTC20050626084023_DEFINED

#include "gxtransform.h"

#include "geom/box.h"
#include "geom/rect.h"
#include "geom/txform.h"
#include "geom/vec3.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
DBG_REGISTER

namespace
{
  const IO::VersionId GXTRANSFORM_SVID = 1;
}

///////////////////////////////////////////////////////////////////////
//
// GxTransformImpl structure
//
///////////////////////////////////////////////////////////////////////

struct GxTransformImpl
{
  GxTransformImpl(GxTransform* p) :
    owner(p),
    txformCache(geom::txform::identity())
  {}

  GxTransform* owner;

private:
  mutable geom::txform txformCache;

public:
  const geom::txform& getTxform() const
  {
    txformCache = geom::txform(owner->translation,
                               owner->scaling,
                               owner->rotationAxis,
                               owner->itsRotationAngle);

    return txformCache;
  }
};

///////////////////////////////////////////////////////////////////////
//
// GxTransform member functions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& GxTransform::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("translation", Field::ValueType(), &GxTransform::translation,
          "0. 0. 0.", "-10. -10. -10.", "10. 10. 10.", "0.1 0.1 0.1",
          Field::NEW_GROUP | Field::MULTI),
    Field("scaling", Field::ValueType(), &GxTransform::scaling,
          "1. 1. 1.", "0.1 0.1 0.1", "10. 10. 10.", "0.1 0.1 0.1",
          Field::MULTI),
    Field("rotationAxis", Field::ValueType(), &GxTransform::rotationAxis,
          "0. 0. 1.", "-1. -1. -1.", "1. 1. 1.", "0.1 0.1 0.1", Field::MULTI),
    Field("rotationAngle",
          &GxTransform::itsRotationAngle, 0., 0., 360., 1.),
    Field("mtxMode", &GxTransform::itsMtxMode, 0, 0, 1, 1,
          Field::TRANSIENT | Field::BOOLEAN),
    Field("jackSize", &GxTransform::itsJackSize, 0, 0, 100, 1, Field::TRANSIENT)
  };

  static FieldMap POS_FIELDS(FIELD_ARRAY);

  return POS_FIELDS;
}

GxTransform* GxTransform::make()
{
DOTRACE("GxTransform::make");
  return new GxTransform;
}

GxTransform::GxTransform() :
  FieldContainer(&sigNodeChanged),
  translation(0.0, 0.0, 0.0),
  scaling(1.0, 1.0, 1.0),
  rotationAxis(0.0, 0.0, 1.0),
  itsRotationAngle(0.0),
  itsMtxMode(0),
  itsJackSize(0),
  rep(new GxTransformImpl(this))
{
DOTRACE("GxTransform::GxTransform");

  dbg_eval_nl(3, rep);

  setFieldMap(GxTransform::classFields());
}

GxTransform::~GxTransform() throw()
{
DOTRACE("GxTransform::~GxTransform");
  delete rep;
}

IO::VersionId GxTransform::serialVersionId() const
{
DOTRACE("GxTransform::serialVersionId");
  return GXTRANSFORM_SVID;
}

void GxTransform::readFrom(IO::Reader& reader)
{
DOTRACE("GxTransform::readFrom");

  reader.ensureReadVersionId("GxTransform", 1,
                             "Try cvs tag xml_conversion_20040526",
                             SRC_POS);

  readFieldsFrom(reader, classFields());
}

void GxTransform::writeTo(IO::Writer& writer) const
{
DOTRACE("GxTransform::writeTo");

  writer.ensureWriteVersionId("GxTransform",
                              GXTRANSFORM_SVID, 1,
                               "Try groovx0.8a4", SRC_POS);

  writeFieldsTo(writer, classFields(), GXTRANSFORM_SVID);
}

///////////////
// accessors //
///////////////

const geom::txform& GxTransform::getTxform() const
{
DOTRACE("GxTransform::getTxform");

  return rep->getTxform();
}

/////////////
// actions //
/////////////

void GxTransform::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxTransform::getBoundingCube");

  bbox.transform(getTxform());
}

void GxTransform::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxTransform::draw");

  if (itsMtxMode == 0)
    {
      DOTRACE("GxTransform::draw::native-transform");
      canvas.translate(translation);
      canvas.scale(scaling);
      canvas.rotate(rotationAxis, itsRotationAngle);
    }
  else
    {
      DOTRACE("GxTransform::draw::custom-transform");
      canvas.transform(rep->getTxform());
    }

  if (itsJackSize > 0)
    {
      Gfx::LinesBlock block(canvas);

      const double d = itsJackSize / 10.0;

      canvas.vertex3(geom::vec3<double>(-d, 0.0, 0.0));
      canvas.vertex3(geom::vec3<double>( d, 0.0, 0.0));

      canvas.vertex3(geom::vec3<double>(0.0, -d, 0.0));
      canvas.vertex3(geom::vec3<double>(0.0,  d, 0.0));

      canvas.vertex3(geom::vec3<double>(0.0, 0.0, -d));
      canvas.vertex3(geom::vec3<double>(0.0, 0.0,  d));
    }
}

static const char vcid_groovx_gfx_gxtransform_cc_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXTRANSFORM_CC_UTC20050626084023_DEFINED
