///////////////////////////////////////////////////////////////////////
//
// tlistutils.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Dec  4 03:04:32 1999
// written: Wed Mar 19 12:46:27 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTUTILS_CC_DEFINED
#define TLISTUTILS_CC_DEFINED

#include "visx/tlistutils.h"

#include "gfx/gxshapekit.h"

#include "io/writer.h"

#include "util/iter.h"
#include "util/objdb.h"
#include "util/ref.h"

#include "visx/trial.h"

#include <fstream>
#include <iomanip>

#include "util/trace.h"

void TlistUtils::writeResponses(const char* filename)
{
DOTRACE("TlistUtils::writeResponses");

  STD_IO::ofstream ofs(filename);
  const int wid = 8;

  // We prepend a '%' to the header line so that MATLAB can ignore
  // this line as a comment
  ofs << '%' << std::setw(wid-1) << "Trial" << std::setw(wid) << "N"
      << std::setw(wid) << "Average" << std::setw(wid) << "msec\n";

  ofs.setf(std::ios::fixed);
  ofs.precision(2);

  for (ObjDb::CastingIterator<Trial> itr(ObjDb::theDb().objects());
       itr.isValid();
       ++itr)
    {
      ofs << std::setw(wid) << itr->id();
      ofs << std::setw(wid) << itr->numResponses();
      ofs << std::setw(wid) << itr->avgResponse();
      ofs << std::setw(wid) << itr->avgRespTime() << std::endl;
    }

  if (ofs.fail())
    throw Util::Error("error while writing to file");
}

void TlistUtils::writeIncidenceMatrix(const char* filename)
{
DOTRACE("TlistUtils::writeIncidenceMatrix");

  STD_IO::ofstream ofs(filename);

  for (ObjDb::CastingIterator<Trial> itr(ObjDb::theDb().objects());
       itr.isValid();
       ++itr)
    {
      // Use this to make sure we don't round down when we should round up.
      double fudge = 0.0001;

      int num_zeros =
        int( (1.0 - itr->avgResponse()) * itr->numResponses() + fudge );

      int num_ones = itr->numResponses() - num_zeros;

      ofs << num_zeros << "  " << num_ones << std::endl;
    }
}

class MatlabTrialWriter : public IO::Writer
{
private:
  STD_IO::ostream& itsOs;
public:
  MatlabTrialWriter(STD_IO::ostream& os) : itsOs(os) {}

  virtual void writeChar(const char*, char) {}
  virtual void writeInt(const char*, int) {}
  virtual void writeBool(const char*, bool) {}
  virtual void writeDouble(const char*, double) {}
  virtual void writeValueObj(const char*, const Value&) {}
  virtual void writeCstring(const char*, const char*) {}

  virtual void writeObject(const char*,
                           Util::SoftRef<const IO::IoObject> obj)
  {
    DOTRACE("MatlabTrialWriter::writeObject");
    if (!obj.isValid())
      return;
    if (dynamic_cast<const GxShapeKit*>(obj.get()) != 0)
      {
        itsOs << obj->id() << ' ';
      }
    else
      {
        obj->writeTo(this);
      }
  }

  virtual void writeOwnedObject(const char* name,
                                Util::Ref<const IO::IoObject> obj)
  {
    DOTRACE("MatlabTrialWriter::writeOwnedObject");
    writeObject(name, obj);
  }

  virtual void writeBaseClass(const char*,
                              Util::Ref<const IO::IoObject> basePart)
  {
    DOTRACE("MatlabTrialWriter::writeBaseClass");
    basePart->writeTo(this);
  }

  virtual void writeRoot(const IO::IoObject* root)
  {
    DOTRACE("MatlabTrialWriter::writeRoot");
    root->writeTo(this);
  }
};

void TlistUtils::writeMatlab(const char* filename)
{
DOTRACE("TlistUtils::writeMatlab");

  STD_IO::ofstream ofs(filename);

  ofs.setf(std::ios::fixed);
  ofs.precision(2);

  MatlabTrialWriter writer(ofs);

  for (ObjDb::CastingIterator<Trial> itr(ObjDb::theDb().objects());
       itr.isValid();
       ++itr)
    {
      writer.writeRoot(*itr);
      ofs << itr->avgResponse() << '\n';
    }
}

static const char vcid_tlistutils_cc[] = "$Header$";
#endif // !TLISTUTILS_CC_DEFINED
