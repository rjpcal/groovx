///////////////////////////////////////////////////////////////////////
//
// tlistutils.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 03:04:32 1999
// written: Wed Sep 12 16:47:29 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTUTILS_CC_DEFINED
#define TLISTUTILS_CC_DEFINED

#include "visx/tlistutils.h"

#include "visx/grobj.h"
#include "visx/trial.h"

#include "io/writer.h"

#include "util/iter.h"
#include "util/objdb.h"
#include "util/ref.h"

#include <fstream.h>
#include <iomanip.h>

#include "util/trace.h"

void TlistUtils::writeResponses(const char* filename)
{
DOTRACE("TlistUtils::writeResponses");

  STD_IO::ofstream ofs(filename);
  const int wid = 8;

  // We prepend a '%' to the header line so that MATLAB can ignore
  // this line as a comment
  ofs << '%' << setw(wid-1) << "Trial" << setw(wid) << "N"
      << setw(wid) << "Average" << setw(wid) << "msec\n";

  ofs.setf(ios::fixed);
  ofs.precision(2);

  for (ObjDb::CastingIterator<Trial> itr(ObjDb::theDb().objects());
       itr.isValid();
       ++itr)
    {
      ofs << setw(wid) << itr->id();
      ofs << setw(wid) << itr->numResponses();
      ofs << setw(wid) << itr->avgResponse();
      ofs << setw(wid) << itr->avgRespTime() << endl;
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

      ofs << num_zeros << "  " << num_ones << endl;
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
    if (obj.isValid() && dynamic_cast<const GrObj*>(obj.get()) != 0)
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
    writeObject(name, obj);
  }

  virtual void writeBaseClass(const char*,
                              Util::Ref<const IO::IoObject> basePart)
  {
    basePart->writeTo(this);
  }

  virtual void writeRoot(const IO::IoObject* root)
  {
    root->writeTo(this);
  }
};

void TlistUtils::writeMatlab(const char* filename)
{
DOTRACE("TlistUtils::writeMatlab");

  STD_IO::ofstream ofs(filename);

  ofs.setf(ios::fixed);
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
