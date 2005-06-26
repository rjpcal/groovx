///////////////////////////////////////////////////////////////////////
//
// cloneface.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Apr 29 09:19:26 1999
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

#ifndef CLONEFACE_CC_DEFINED
#define CLONEFACE_CC_DEFINED

#include "visx/cloneface.h"

#include "io/ioproxy.h"
#include "io/readutils.h"
#include "io/writeutils.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

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
DOTRACE("CloneFace::make");
  return new CloneFace;
}

CloneFace::CloneFace () :
  Face(), itsEyeAspect(0.0), itsVertOffset(0.0)
{
DOTRACE("CloneFace::CloneFace()");

  setFieldMap(CloneFace::classFields());

  // Copy Face's control points into CloneFace's itsCtrlPnts member,
  // so that the default behavior of CloneFace is to mimic Face.
  const double* const face_pnts = Face::getCtrlPnts();
  for (int i = 0; i < 24; ++i)
    {
      itsCtrlPnts[i] = face_pnts[i];
    }
}

CloneFace::~CloneFace () throw()
{
DOTRACE("CloneFace::~CloneFace");
}

IO::VersionId CloneFace::serialVersionId() const
{
DOTRACE("CloneFace::serialVersionId");
  return CLONEFACE_SVID;
}

void CloneFace::readFrom(IO::Reader& reader)
{
DOTRACE("CloneFace::readFrom");

  IO::ReadUtils::readValueSeq<double>(reader, "ctrlPnts",
                                               itsCtrlPnts, 24);

  readFieldsFrom(reader, classFields());

  reader.readBaseClass("Face", IO::makeProxy<Face>(this));
}

void CloneFace::writeTo(IO::Writer& writer) const
{
DOTRACE("CloneFace::writeTo");

  IO::WriteUtils::writeValueSeq(writer, "ctrlPnts",
                                itsCtrlPnts, itsCtrlPnts+24, true);

  writeFieldsTo(writer, classFields(), CLONEFACE_SVID);

  writer.writeBaseClass("Face", IO::makeConstProxy<Face>(this));
}

const double* CloneFace::getCtrlPnts() const
{
DOTRACE("CloneFace::getCtrlPnts");
  return itsCtrlPnts;
}

double CloneFace::getEyeAspect() const
{
DOTRACE("CloneFace::getEyeAspect");
  return itsEyeAspect;
}

double CloneFace::getVertOffset() const
{
DOTRACE("CloneFace::getVertOffset");
  return itsVertOffset;
}

static const char vcid_cloneface_cc[] = "$Id$ $URL$";
#endif // !CLONEFACE_CC_DEFINED
