///////////////////////////////////////////////////////////////////////
//
// ioutil.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 21:43:28 1999
// written: Thu Sep 13 11:32:13 2001
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

#include "util/error.h"
#include "util/gzstreambuf.h"
#include "util/pointers.h"
#include "util/ref.h"
#include "util/strings.h"

#include <iostream.h>
#include <strstream.h>

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
    ostrstream ost;

    try
      {
        streamWrite<Writer>(obj, ost);
        ost << '\0';
      }
    catch (Util::Error& err)
      {
        err.msg().append(" with buffer contents ==\n");

        ost << '\0';
        err.msg().append(ost.str());

        ost.rdbuf()->freeze(0); // avoids leaking the buffer memory

        throw;
      }
    catch (...)
      {
        ost.rdbuf()->freeze(0); // avoids leaking the buffer memory
        throw;
      }

    fstring result(ost.str());

    ost.rdbuf()->freeze(0); // avoids leaking the buffer memory

    return result;
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
  istrstream ist(buf);

  streamRead<IO::LegacyReader>(obj, ist);
}

void IO::read(Util::Ref<IO::IoObject> obj, const char* buf)
{
  istrstream ist(buf);

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

static const char vcid_ioutil_cc[] = "$Header$";
#endif // !IOUTIL_CC_DEFINED
