///////////////////////////////////////////////////////////////////////
//
// iolegacy.cc
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Sep 27 08:40:04 2000
// commit: $Id$
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

#ifndef IOLEGACY_CC_DEFINED
#define IOLEGACY_CC_DEFINED

#include "io/iolegacy.h"

#include "util/error.h"
#include "util/objmgr.h"
#include "util/ref.h"
#include "util/strings.h"
#include "util/value.h"

#include <cctype>
#include <cstring>
#include <iostream>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using Util::Ref;
using Util::SoftRef;

///////////////////////////////////////////////////////////////////////
//
// LegacyReader::Impl definition
//
///////////////////////////////////////////////////////////////////////

class IO::LegacyReader::Impl
{
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(IO::LegacyReader* owner, STD_IO::istream& is) :
    itsOwner(owner),
    itsInStream(is),
    itsLegacyVersionId(0)
  {}

  void throwIfError(const char* type, const rutz::file_pos& pos)
  {
    if (itsInStream.fail())
      {
        dbg_print(3, "throwIfError for"); dbg_eval_nl(3, type);
        throw rutz::error(type, pos);
      }
  }

  void throwIfError(const fstring& type, const rutz::file_pos& pos)
  {
    throwIfError(type.c_str(), pos);
  }

  IO::LegacyReader* itsOwner;
  STD_IO::istream& itsInStream;
  int itsLegacyVersionId;

  void readTypename(const fstring& correct_name)
  {
    DOTRACE("IO::LegacyReader::Impl::readTypename(fstring)");

    fstring name;
    itsInStream >> name;

    if (! name.equals(correct_name))
      {
        // If we got here, then none of the substrings matched so we must
        // raise an exception.
        throw rutz::error(fstring("couldn't read typename for ",
                                  correct_name.c_str()), SRC_POS);
      }
  }

  // An override for when we have two alternative names
  void readTypename(const fstring& correct_name, const fstring& alt_name)
  {
    DOTRACE("IO::LegacyReader::Impl::readTypename(fstring, fstring)");

    fstring name;
    itsInStream >> name;

    if ( !name.equals(correct_name) && !name.equals(alt_name) )
      {
        // If we got here, then none of the substrings matched so we must
        // raise an exception.
        throw rutz::error(fstring("couldn't read typename for ",
                                  correct_name.c_str()), SRC_POS);
      }
  }

  int getLegacyVersionId()
  {
    DOTRACE("IO::LegacyReader::Impl::getLegacyVersionId");
    while ( isspace(itsInStream.peek()) )
      { itsInStream.get(); }

    int version = -1;

    if (itsInStream.peek() == '@')
      {
        int c = itsInStream.get();
        ASSERT(c == '@');

        itsInStream >> version;
        dbg_eval_nl(3, version);
        throwIfError("versionId", SRC_POS);
      }
    else
      {
        throw rutz::error("missing legacy versionId", SRC_POS);
      }

    return version;
  }

  void grabLeftBrace()
  {
    char brace;
    itsInStream >> brace;
    if (brace != '{')
      {
        dbg_print_nl(3, "grabLeftBrace failed");
        throw rutz::error("missing left-brace", SRC_POS);
      }
  }

  void grabRightBrace()
  {
    char brace;
    itsInStream >> brace;
    if (brace != '}')
      {
        dbg_print_nl(3, "grabRightBrace failed");
        throw rutz::error("missing right-brace", SRC_POS);
      }
  }

  void inflateObject(const fstring& name, Ref<IO::IoObject> obj)
  {
    DOTRACE("IO::LegacyReader::Impl::inflateObject");

    dbg_eval_nl(3, name);

    itsLegacyVersionId = getLegacyVersionId();
    if (itsLegacyVersionId != -1)
      {
        grabLeftBrace();
        obj->readFrom(*itsOwner);
        grabRightBrace();
      }

    throwIfError(name, SRC_POS);
  }
};

///////////////////////////////////////////////////////////////////////
//
// LegacyReader member definitions
//
///////////////////////////////////////////////////////////////////////

IO::LegacyReader::LegacyReader(STD_IO::istream& is) :
  rep(new Impl(this, is))
{
DOTRACE("IO::LegacyReader::LegacyReader");
}

IO::LegacyReader::~LegacyReader() throw()
{
DOTRACE("IO::LegacyReader::~LegacyReader");
  delete rep;
}

IO::VersionId IO::LegacyReader::readSerialVersionId()
{
DOTRACE("IO::LegacyReader::readSerialVersionId");
  dbg_eval_nl(3, rep->itsLegacyVersionId);
  return rep->itsLegacyVersionId;
}

char IO::LegacyReader::readChar(const fstring& name)
{
DOTRACE("IO::LegacyReader::readChar");
  dbg_eval(3, name);
  char val;
  rep->itsInStream >> val;   dbg_eval_nl(3, val);
  rep->throwIfError(name, SRC_POS);
  return val;
}

int IO::LegacyReader::readInt(const fstring& name)
{
DOTRACE("IO::LegacyReader::readInt");
  dbg_eval(3, name);
  int val;
  rep->itsInStream >> val;   dbg_eval_nl(3, val);
  rep->throwIfError(name, SRC_POS);
  return val;
}

bool IO::LegacyReader::readBool(const fstring& name)
{
DOTRACE("IO::LegacyReader::readBool");
  dbg_eval(3, name);
  int val;
  rep->itsInStream >> val;   dbg_eval_nl(3, val);
  rep->throwIfError(name, SRC_POS);
  return bool(val);
}

double IO::LegacyReader::readDouble(const fstring& name)
{
DOTRACE("IO::LegacyReader::readDouble");
  dbg_eval(3, name);
  double val;
  rep->itsInStream >> val;   dbg_eval_nl(3, val);
  rep->throwIfError(name, SRC_POS);
  return val;
}

fstring IO::LegacyReader::readStringImpl(const fstring& name)
{
DOTRACE("IO::LegacyReader::readStringImpl");
  dbg_eval_nl(3, name);

  int numchars = 0;
  rep->itsInStream >> numchars;

  rep->throwIfError(name, SRC_POS);

  if (numchars < 0)
    {
      throw rutz::error("LegacyReader::readStringImpl "
                        "saw negative character count", SRC_POS);
    }

  int c = rep->itsInStream.get();
  if (c != ' ')
    {
      throw rutz::error("LegacyReader::readStringImpl "
                        "did not have whitespace after character count", SRC_POS);
    }

  fstring new_string;
  new_string.readsome(rep->itsInStream, (unsigned int) numchars);

  rep->throwIfError(name, SRC_POS);

  dbg_eval_nl(3, new_string);

  return new_string;
}

void IO::LegacyReader::readValueObj(const fstring& name, Value& value)
{
DOTRACE("IO::LegacyReader::readValueObj");
  dbg_eval_nl(3, name);
  value.scanFrom(rep->itsInStream);
  rep->throwIfError(name, SRC_POS);
}

Ref<IO::IoObject>
IO::LegacyReader::readObject(const fstring& name)
{
DOTRACE("IO::LegacyReader::readObject");
  return Ref<IO::IoObject>(readMaybeObject(name));
}

SoftRef<IO::IoObject>
IO::LegacyReader::readMaybeObject(const fstring& name)
{
DOTRACE("IO::LegacyReader::readMaybeObject");
  dbg_eval(3, name);
  fstring type;
  rep->itsInStream >> type; dbg_eval(3, type);

  if (type == "NULL")
    {
      return SoftRef<IO::IoObject>();
    }

  Ref<IO::IoObject> obj(Util::ObjMgr::newTypedObj<IO::IoObject>(type));
  dbg_eval_nl(3, obj->objTypename());

  rep->inflateObject(name, obj);

  return obj;
}

void IO::LegacyReader::readOwnedObject(const fstring& name,
                                       Ref<IO::IoObject> obj)
{
DOTRACE("IO::LegacyReader::readOwnedObject");

  rep->readTypename(obj->objTypename());
  rep->inflateObject(name, obj);
}

void IO::LegacyReader::readBaseClass(const fstring& baseClassName,
                                     Ref<IO::IoObject> basePart)
{
DOTRACE("IO::LegacyReader::readBaseClass");

  // For backward-compatibility, we allow the typename to match either the
  // real typename of the base part, or the descriptive name given to the
  // base class.
  rep->readTypename(basePart->objTypename(), baseClassName);
  rep->inflateObject(baseClassName, basePart);
}

Ref<IO::IoObject> IO::LegacyReader::readRoot(IO::IoObject* givenRoot)
{
DOTRACE("IO::LegacyReader::readRoot");
  if (givenRoot == 0)
    {
      return readObject("rootObject");
    }

  dbg_eval_nl(3, givenRoot->objTypename());

  Ref<IO::IoObject> root(givenRoot);
  readOwnedObject("rootObject", root);

  return root;
}

///////////////////////////////////////////////////////////////////////
//
// LegacyWriter::Impl definition
//
///////////////////////////////////////////////////////////////////////

class IO::LegacyWriter::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(IO::LegacyWriter* owner, STD_IO::ostream& os, bool write_bases) :
    itsOwner(owner),
    itsOutStream(os),
    itsWriteBases(write_bases),
    itsFSep(' '),
    itsIndentLevel(0),
    itsNeedsNewline(false),
    itsNeedsWhitespace(false),
    itsIsBeginning(true),
    itsUsePrettyPrint(true)
  {}

  void throwIfError(const char* type, const rutz::file_pos& pos)
  {
    if (itsOutStream.fail())
      {
        dbg_print(3, "throwIfError for"); dbg_eval_nl(3, type);
        throw rutz::error(type, pos);
      }
  }

  IO::LegacyWriter* itsOwner;
private:
  STD_IO::ostream& itsOutStream;
public:
  const bool itsWriteBases;
  const char itsFSep;              // field separator
  int itsIndentLevel;
  bool itsNeedsNewline;
  bool itsNeedsWhitespace;
  bool itsIsBeginning;
  bool itsUsePrettyPrint;

  STD_IO::ostream& stream()
    {
      flushWhitespace();
      itsIsBeginning = false;
      return itsOutStream;
    }

  void flushWhitespace()
    {
      updateNewline();
      updateWhitespace();
    }

private:
  class Indenter
  {
  private:
    Impl* itsOwner;

    Indenter(const Indenter&);
    Indenter& operator=(const Indenter&);

  public:
    Indenter(Impl* impl) : itsOwner(impl) { ++(itsOwner->itsIndentLevel); }
    ~Indenter() { --(itsOwner->itsIndentLevel); }
  };

  void doNewlineAndTabs()
    {
      itsOutStream << '\n';
      for (int i = 0; i < itsIndentLevel; ++i)
        itsOutStream << '\t';
    }

  void doWhitespace()
    {
      if (itsUsePrettyPrint)
        doNewlineAndTabs();
      else
        itsOutStream << ' ';
    }

  void updateNewline()
    {
      if (itsNeedsNewline)
        {
          doNewlineAndTabs();
          noNewlineNeeded();
          noWhitespaceNeeded();
        }
    }

  void updateWhitespace()
    {
      if (itsNeedsWhitespace)
        {
          doWhitespace();
          noWhitespaceNeeded();
        }
    }

public:
  void usePrettyPrint(bool yes) { itsUsePrettyPrint = yes; }

  void requestNewline() { if (!itsIsBeginning) itsNeedsNewline = true; }
  void requestWhitespace() { if (!itsIsBeginning) itsNeedsWhitespace = true; }
  void noNewlineNeeded() { itsNeedsNewline = false; }
  void noWhitespaceNeeded() { itsNeedsWhitespace = false; }

  void flattenObject(const char* obj_name,
                     SoftRef<const IO::IoObject> obj,
                     bool stub_out = false)
  {
    if (itsIndentLevel > 0)
      requestWhitespace();
    else
      requestNewline();

    if ( !(obj.isValid()) )
      {
        stream() << "NULL" << itsFSep;
        throwIfError(obj_name, SRC_POS);
        return;
      }

    ASSERT(obj.isValid());

    stream() << obj->objTypename() << itsFSep;
    throwIfError(obj->objTypename().c_str(), SRC_POS);

    stream() << '@';

    if (stub_out)
      {
        stream() << "-1 ";
      }
    else
      {
        stream() << obj->serialVersionId() << " {";
        {
          Indenter indent(this);
          requestWhitespace();
          obj->writeTo(*itsOwner);
        }
        requestWhitespace();
        stream() << "}";
      }

    if (itsIndentLevel > 0)
      requestWhitespace();
    else
      requestNewline();

    throwIfError(obj_name, SRC_POS);
  }
};

///////////////////////////////////////////////////////////////////////
//
// LegacyWriter member definitions
//
///////////////////////////////////////////////////////////////////////


IO::LegacyWriter::LegacyWriter(STD_IO::ostream& os, bool write_bases) :
  rep(new Impl(this, os, write_bases))
{
DOTRACE("IO::LegacyWriter::LegacyWriter");
}

IO::LegacyWriter::~LegacyWriter() throw()
{
DOTRACE("IO::LegacyWriter::~LegacyWriter");
  rep->flushWhitespace();
  delete rep;
}

void IO::LegacyWriter::usePrettyPrint(bool yes)
{
DOTRACE("IO::LegacyWriter::usePrettyPrint");
  rep->usePrettyPrint(yes);
}

void IO::LegacyWriter::writeChar(const char* name, char val)
{
DOTRACE("IO::LegacyWriter::writeChar");
  rep->stream() << val << rep->itsFSep;
  rep->throwIfError(name, SRC_POS);
}

void IO::LegacyWriter::writeInt(const char* name, int val)
{
DOTRACE("IO::LegacyWriter::writeInt");
  rep->stream() << val << rep->itsFSep;
  rep->throwIfError(name, SRC_POS);
}

void IO::LegacyWriter::writeBool(const char* name, bool val)
{
DOTRACE("IO::LegacyWriter::writeBool");
  rep->stream() << val << rep->itsFSep;
  rep->throwIfError(name, SRC_POS);
}

void IO::LegacyWriter::writeDouble(const char* name, double val)
{
DOTRACE("IO::LegacyWriter::writeDouble");
  rep->stream() << val << rep->itsFSep;
  rep->throwIfError(name, SRC_POS);
}

void IO::LegacyWriter::writeCstring(const char* name, const char* val)
{
DOTRACE("IO::LegacyWriter::writeCstring");

  rep->stream() << strlen(val) << " " << val << rep->itsFSep;

  rep->throwIfError(name, SRC_POS);
}

void IO::LegacyWriter::writeValueObj(const char* name,
                                     const Value& value)
{
DOTRACE("IO::LegacyWriter::writeValueObj");
  value.printTo(rep->stream());
  rep->stream() << rep->itsFSep;
  rep->throwIfError(name, SRC_POS);
}

void IO::LegacyWriter::writeRawData(const char* name,
                                    const unsigned char* data,
                                    unsigned int length)
{
DOTRACE("IO::LegacyWriter::writeRawData");
  defaultWriteRawData(name, data, length);
}

void IO::LegacyWriter::writeObject(const char* name,
                                   SoftRef<const IO::IoObject> obj)
{
DOTRACE("IO::LegacyWriter::writeObject");

  rep->flattenObject(name, obj);
}

void IO::LegacyWriter::writeOwnedObject(const char* name,
                                        Ref<const IO::IoObject> obj)
{
DOTRACE("IO::LegacyWriter::writeOwnedObject");

  rep->flattenObject(name, obj);
}

void IO::LegacyWriter::writeBaseClass(const char* baseClassName,
                                      Ref<const IO::IoObject> basePart)
{
DOTRACE("IO::LegacyWriter::writeBaseClass");
  if (rep->itsWriteBases)
    {
      rep->flattenObject(baseClassName, basePart);
    }
  else
    {
      rep->flattenObject(baseClassName, basePart, true);
    }
}

void IO::LegacyWriter::writeRoot(const IO::IoObject* root)
{
DOTRACE("IO::LegacyWriter::writeRoot");

  rep->flattenObject
    ("rootObject", SoftRef<IO::IoObject>(const_cast<IO::IoObject*>(root),
                                         Util::STRONG,
                                         Util::PRIVATE));
}

static const char vcid_iolegacy_cc[] = "$Header$";
#endif // !IOLEGACY_CC_DEFINED
