/** @file visx/cloneface.cc graphic object for modified Brunswik faces
    with some additional parameters */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Apr 29 09:19:26 1999
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

#ifndef GROOVX_VISX_CLONEFACE_CC_UTC20050626084017_DEFINED
#define GROOVX_VISX_CLONEFACE_CC_UTC20050626084017_DEFINED

#include "visx/cloneface.h"

#include "io/ioproxy.h"
#include "io/readutils.h"
#include "io/writeutils.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  const int CLONEFACE_SVID = 0;
}

///////////////////////////////////////////////////////////////////////
//
// CloneFace member functions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& CloneFace::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("eyeAspect", &CloneFace::itsEyeAspect, 0.0, 0.0, 1.0, 0.05,
          Field::NEW_GROUP),
    Field("vertOffset", &CloneFace::itsVertOffset, 0.0, -0.1, 0.1, 0.01)
  };

  static FieldMap CLONEFACE_FIELDS(FIELD_ARRAY, &Face::classFields());

  return CLONEFACE_FIELDS;
}

CloneFace* CloneFace::make()
{
GVX_TRACE("CloneFace::make");
  return new CloneFace;
}

CloneFace::CloneFace () :
  Face(), itsEyeAspect(0.0), itsVertOffset(0.0)
{
GVX_TRACE("CloneFace::CloneFace()");

  setFieldMap(CloneFace::classFields());

  // Copy Face's control points into CloneFace's itsCtrlPnts member,
  // so that the default behavior of CloneFace is to mimic Face.
  const double* const face_pnts = Face::getCtrlPnts();
  for (int i = 0; i < 24; ++i)
    {
      itsCtrlPnts[i] = face_pnts[i];
    }
}

CloneFace::~CloneFace () noexcept
{
GVX_TRACE("CloneFace::~CloneFace");
}

io::version_id CloneFace::class_version_id() const
{
GVX_TRACE("CloneFace::class_version_id");
  return CLONEFACE_SVID;
}

void CloneFace::read_from(io::reader& reader)
{
GVX_TRACE("CloneFace::read_from");

  io::read_utils::read_value_seq<double>(reader, "ctrlPnts",
                                         itsCtrlPnts, 24);

  readFieldsFrom(reader, classFields());

  reader.read_base_class("Face", io::make_proxy<Face>(this));
}

void CloneFace::write_to(io::writer& writer) const
{
GVX_TRACE("CloneFace::write_to");

  io::write_utils::write_value_seq(writer, "ctrlPnts",
                                   itsCtrlPnts, itsCtrlPnts+24, true);

  writeFieldsTo(writer, classFields(), CLONEFACE_SVID);

  writer.write_base_class("Face", io::make_const_proxy<Face>(this));
}

const double* CloneFace::getCtrlPnts() const
{
GVX_TRACE("CloneFace::getCtrlPnts");
  return itsCtrlPnts;
}

double CloneFace::getEyeAspect() const
{
GVX_TRACE("CloneFace::getEyeAspect");
  return itsEyeAspect;
}

double CloneFace::getVertOffset() const
{
GVX_TRACE("CloneFace::getVertOffset");
  return itsVertOffset;
}

#endif // !GROOVX_VISX_CLONEFACE_CC_UTC20050626084017_DEFINED
