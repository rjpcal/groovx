///////////////////////////////////////////////////////////////////////
//
// ioutil.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 11 21:43:28 1999
// written: Wed Mar 19 12:45:50 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOUTIL_CC_DEFINED
#define IOUTIL_CC_DEFINED

#include "io/ioutil.h"

#include "io/asciistreamreader.h"
#include "io/asciistreamwriter.h"
#include "io/io.h"
#include "io/iolegacy.h"

#include "util/cstrstream.h"
#include "util/error.h"
#include "util/gzstreambuf.h"
#include "util/pointers.h"
#include "util/ref.h"
#include "util/strings.h"

#include <iostream>
#include <sstream>

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  template <class Writer>
  void streamWrite(Util::Ref<IO::IoObject> obj, STD_IO::ostream& os)
  {
    Writer writer(os);
    writer.writeRoot(obj.get());
  }

  template <class Reader>
  void streamRead(Util::Ref<IO::IoObject> obj, STD_IO::istream& is)
  {
    Reader reader(is);
    reader.readRoot(obj.get());
  }

  template <class Writer>
  fstring stringWrite(Util::Ref<IO::IoObject> obj)
  {
    std::ostringstream ost;

    try
      {
        streamWrite<Writer>(obj, ost);
      }
    catch (Util::Error& err)
      {
        err.msg().append(" with buffer contents ==\n",
                         ost.str().c_str());

        throw;
      }

    return fstring(ost.str().c_str());
  }
}

fstring IO::stringify(Util::Ref<IO::IoObject> obj)
{
  return stringWrite<IO::LegacyWriter>(obj);
}

fstring IO::write(Util::Ref<IO::IoObject> obj)
{
  return stringWrite<AsciiStreamWriter>(obj);
}

void IO::destringify(Util::Ref<IO::IoObject> obj, const char* buf)
{
  Util::icstrstream ist(buf);

  streamRead<IO::LegacyReader>(obj, ist);
}

void IO::read(Util::Ref<IO::IoObject> obj, const char* buf)
{
  Util::icstrstream ist(buf);

  streamRead<AsciiStreamReader>(obj, ist);
}

void IO::saveASW(Util::Ref<IO::IoObject> obj, fstring filename)
{
  shared_ptr<STD_IO::ostream> os(Util::ogzopen(filename));

  streamWrite<AsciiStreamWriter>(obj, *os);
}

void IO::loadASR(Util::Ref<IO::IoObject> obj, fstring filename)
{
  shared_ptr<STD_IO::istream> is(Util::igzopen(filename));

  streamRead<AsciiStreamReader>(obj, *is);
}

Util::Ref<IO::IoObject> IO::retrieveASR(fstring filename)
{
  shared_ptr<STD_IO::istream> is(Util::igzopen(filename));

  AsciiStreamReader reader(*is);
  return reader.readRoot();
}

static const char vcid_ioutil_cc[] = "$Header$";
#endif // !IOUTIL_CC_DEFINED
