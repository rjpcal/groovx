///////////////////////////////////////////////////////////////////////
//
// stringifycmd.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 21:43:28 1999
// written: Wed Jul 18 14:13:35 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGIFYCMD_CC_DEFINED
#define STRINGIFYCMD_CC_DEFINED

#include "tcl/stringifycmd.h"

#include "io/asciistreamreader.h"
#include "io/asciistreamwriter.h"
#include "io/io.h"
#include "io/iolegacy.h"

#include "tcl/tclerror.h"
#include "tcl/tclvalue.h"

#include "util/arrays.h"
#include "util/gzstreambuf.h"
#include "util/ref.h"
#include "util/strings.h"

#include <cstring>
#include <fstream.h>
#include <strstream.h>
#include <typeinfo>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
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
  Tcl::ObjPtr stringWrite(Util::Ref<IO::IoObject> obj)
  {
    ostrstream ost;

    try {
      streamWrite<Writer>(obj, ost);
      ost << '\0';
    }
    catch (IO::IoError& err) {
      err.appendMsg(" with buffer contents ==\n");

      ost << '\0';
      err.appendMsg(ost.str());

      ost.rdbuf()->freeze(0); // avoids leaking the buffer memory

      throw err;
    }
    catch (...) {
      ost.rdbuf()->freeze(0); // avoids leaking the buffer memory
      throw;
    }

    Tcl::ObjPtr result(ost.str());

    ost.rdbuf()->freeze(0); // avoids leaking the buffer memory

    return result;
  }
}

Tcl::ObjPtr Tcl::stringify(Util::Ref<IO::IoObject> obj)
{
  return stringWrite<IO::LegacyWriter>(obj);
}

Tcl::ObjPtr Tcl::write(Util::Ref<IO::IoObject> obj)
{
  return stringWrite<AsciiStreamWriter>(obj);
}

void Tcl::destringify(Util::Ref<IO::IoObject> obj, const char* buf)
{
  istrstream ist(buf);

  streamRead<IO::LegacyReader>(obj, ist);
}

void Tcl::read(Util::Ref<IO::IoObject> obj, const char* buf)
{
  istrstream ist(buf);

  streamRead<AsciiStreamReader>(obj, ist);
}

void Tcl::saveASW(Util::Ref<IO::IoObject> obj, fixed_string filename)
{
  STD_IO::ofstream ofs(filename.c_str());
  if ( ofs.fail() )
    {
      Tcl::TclError err("couldn't open file ");
      err.appendMsg("'", filename.c_str(), "'");
      err.appendMsg("for writing");
      throw err;
    }

  if ( filename.ends_with("gz") )
    {
      ofs.close();

      Util::gzstreambuf buf(filename.c_str(), STD_IO::ios::out);
      STD_IO::ostream os(&buf);

      streamWrite<AsciiStreamWriter>(obj, os);
    }
  else
    {
      streamWrite<AsciiStreamWriter>(obj, ofs);
    }
}

void Tcl::loadASR(Util::Ref<IO::IoObject> obj, fixed_string filename)
{
  STD_IO::ifstream ifs(filename.c_str());
  if ( ifs.fail() )
    {
      Tcl::TclError err("couldn't open file ");
      err.appendMsg("'", filename.c_str(), "'");
      err.appendMsg("for reading");
      throw err;
    }

  if ( filename.ends_with("gz") )
    {
      ifs.close();

      Util::gzstreambuf buf(filename.c_str(), STD_IO::ios::in);
      STD_IO::istream is(&buf);

      streamRead<AsciiStreamReader>(obj, is);
    }
  else
    {
      streamRead<AsciiStreamReader>(obj, ifs);
    }
}

static const char vcid_stringifycmd_cc[] = "$Header$";
#endif // !STRINGIFYCMD_CC_DEFINED
