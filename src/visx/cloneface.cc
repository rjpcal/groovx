///////////////////////////////////////////////////////////////////////
//
// cloneface.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Apr 29 09:19:26 1999
// written: Wed Aug 15 14:07:38 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CLONEFACE_CC_DEFINED
#define CLONEFACE_CC_DEFINED

#include "cloneface.h"

#include "io/ioproxy.h"
#include "io/readutils.h"
#include "io/writeutils.h"

#include <cstring>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

namespace
{
  const FieldInfo FINFOS[] =
  {
    FieldInfo("eyeAspect", FieldInfo::OldTag(), &CloneFace::eyeAspect, 0.0, 0.0, 1.0, 0.05, true),
    FieldInfo("vertOffset", FieldInfo::OldTag(), &CloneFace::vertOffset, 0.0, -0.1, 0.1, 0.01)
  };

  const unsigned int NUM_FINFOS = sizeof(FINFOS)/sizeof(FieldInfo);

  FieldMap CLONEFACE_FIELDS(FINFOS, FINFOS+NUM_FINFOS,
                            &Face::classFields());
}

///////////////////////////////////////////////////////////////////////
//
// CloneFace member functions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& CloneFace::classFields()
{
  return CLONEFACE_FIELDS;
}

CloneFace* CloneFace::make()
{
DOTRACE("CloneFace::make");
  return new CloneFace;
}

CloneFace::CloneFace () :
  Face(), eyeAspect(0.0), vertOffset(0.0)
{
DOTRACE("CloneFace::CloneFace()");

  setFieldMap(CLONEFACE_FIELDS);

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
  return eyeAspect();
}

double CloneFace::getVertOffset() const
{
DOTRACE("CloneFace::getVertOffset");
  return vertOffset();
}

static const char vcid_cloneface_cc[] = "$Header$";
#endif // !CLONEFACE_CC_DEFINED
