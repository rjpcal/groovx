///////////////////////////////////////////////////////////////////////
//
// cloneface.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Apr 29 09:19:26 1999
// written: Wed Mar 19 12:45:33 2003
// $Id$
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

CloneFace::~CloneFace ()
{
DOTRACE("CloneFace::~CloneFace");
}

void CloneFace::readFrom(IO::Reader* reader)
{
DOTRACE("CloneFace::readFrom");

  IO::ReadUtils::readValueSeq<double>(reader, "ctrlPnts",
                                               itsCtrlPnts, 24);

  readFieldsFrom(reader, classFields());

  reader->readBaseClass("Face", IO::makeProxy<Face>(this));
}

void CloneFace::writeTo(IO::Writer* writer) const
{
DOTRACE("CloneFace::writeTo");

  IO::WriteUtils::writeValueSeq(writer, "ctrlPnts",
                                itsCtrlPnts, itsCtrlPnts+24, true);

  writeFieldsTo(writer, classFields());

  writer->writeBaseClass("Face", IO::makeConstProxy<Face>(this));
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

static const char vcid_cloneface_cc[] = "$Header$";
#endif // !CLONEFACE_CC_DEFINED
