///////////////////////////////////////////////////////////////////////
//
// iolegacy.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 27 08:40:04 2000
// written: Sun Nov  3 13:41:52 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOLEGACY_CC_DEFINED
#define IOLEGACY_CC_DEFINED

#include "io/iolegacy.h"

#include "util/objmgr.h"
#include "util/ref.h"
#include "util/strings.h"
#include "util/value.h"

#include <cctype>
#include <iostream>

#include "util/trace.h"
#include "util/debug.h"

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

  void throwIfError(const char* type)
  {
    if (itsInStream.fail())
      {
        dbgPrint(3, "throwIfError for"); dbgEvalNL(3, type);
        throw IO::ReadError(type);
      }
  }

  void throwIfError(const fstring& type)
  {
    throwIfError(type.c_str());
  }

  IO::LegacyReader* itsOwner;
  STD_IO::istream& itsInStream;
  int itsLegacyVersionId;

  void readTypename(const fstring& correct_name)
  {
    fstring name;
    itsInStream >> name;

    if (! name.equals(correct_name))
      {
        // If we got here, then none of the substrings matched so we must
        // raise an exception.
        throw IO::ReadError(fstring("couldn't read typename for ",
                                    correct_name.c_str()));
      }
  }

  int getLegacyVersionId()
  {
    while ( isspace(itsInStream.peek()) )
      { itsInStream.get(); }

    int version = -1;

    if (itsInStream.peek() == '@')
      {
        int c = itsInStream.get();
        Assert(c == '@');

        itsInStream >> version;
        dbgEvalNL(3, version);
        throwIfError("versionId");
      }
    else
      {
        throw IO::ReadError("missing legacy versionId");
      }

    return version;
  }

  void grabLeftBrace()
  {
    char brace;
    itsInStream >> brace;
    if (brace != '{')
      {
        dbgPrintNL(3, "grabLeftBrace failed");
        throw IO::ReadError("missing left-brace");
      }
  }

  void grabRightBrace()
  {
    char brace;
    itsInStream >> brace;
    if (brace != '}')
      {
        dbgPrintNL(3, "grabRightBrace failed");
        throw IO::ReadError("missing right-brace");
      }
  }

  void inflateObject(const fstring& name, Ref<IO::IoObject> obj)
  {
    dbgEvalNL(3, name);

    itsLegacyVersionId = getLegacyVersionId();
    if (itsLegacyVersionId != -1)
      {
        grabLeftBrace();
        obj->readFrom(itsOwner);
        grabRightBrace();
      }

    throwIfError(name);
  }
};

///////////////////////////////////////////////////////////////////////
//
// LegacyReader member definitions
//
///////////////////////////////////////////////////////////////////////

IO::LegacyReader::LegacyReader(STD_IO::istream& is) :
  itsImpl(new Impl(this, is))
{
DOTRACE("IO::LegacyReader::LegacyReader");
}

IO::LegacyReader::~LegacyReader()
{
DOTRACE("IO::LegacyReader::~LegacyReader");
  delete itsImpl;
}

IO::VersionId IO::LegacyReader::readSerialVersionId()
{
DOTRACE("IO::LegacyReader::readSerialVersionId");
  dbgEvalNL(3, itsImpl->itsLegacyVersionId);
  return itsImpl->itsLegacyVersionId;
}

char IO::LegacyReader::readChar(const fstring& name)
{
DOTRACE("IO::LegacyReader::readChar");
  dbgEval(3, name);
  char val;
  itsImpl->itsInStream >> val;   dbgEvalNL(3, val);
  itsImpl->throwIfError(name);
  return val;
}

int IO::LegacyReader::readInt(const fstring& name)
{
DOTRACE("IO::LegacyReader::readInt");
  dbgEval(3, name);
  int val;
  itsImpl->itsInStream >> val;   dbgEvalNL(3, val);
  itsImpl->throwIfError(name);
  return val;
}

bool IO::LegacyReader::readBool(const fstring& name)
{
DOTRACE("IO::LegacyReader::readBool");
  dbgEval(3, name);
  int val;
  itsImpl->itsInStream >> val;   dbgEvalNL(3, val);
  itsImpl->throwIfError(name);
  return bool(val);
}

double IO::LegacyReader::readDouble(const fstring& name)
{
DOTRACE("IO::LegacyReader::readDouble");
  dbgEval(3, name);
  double val;
  itsImpl->itsInStream >> val;   dbgEvalNL(3, val);
  itsImpl->throwIfError(name);
  return val;
}

fstring IO::LegacyReader::readStringImpl(const fstring& name)
{
DOTRACE("IO::LegacyReader::readStringImpl");
  dbgEvalNL(3, name);

  int numchars = 0;
  itsImpl->itsInStream >> numchars;

  itsImpl->throwIfError(name);

  if (numchars < 0)
    {
      throw IO::ReadError("LegacyReader::readStringImpl "
                          "saw negative character count");
    }

  int c = itsImpl->itsInStream.get();
  if (c != ' ')
    {
      throw IO::ReadError("LegacyReader::readStringImpl "
                          "did not have whitespace after character count");
    }

//   if (itsImpl->itsInStream.peek() == '\n') { itsImpl->itsInStream.get(); }

  fstring new_string;
  new_string.readsome(itsImpl->itsInStream, (unsigned int) numchars);

  itsImpl->throwIfError(name);

  dbgEvalNL(3, new_string);

  return new_string;
}

void IO::LegacyReader::readValueObj(const fstring& name, Value& value)
{
DOTRACE("IO::LegacyReader::readValueObj");
  dbgEvalNL(3, name);
  value.scanFrom(itsImpl->itsInStream);
  itsImpl->throwIfError(name);
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
  dbgEval(3, name);
  fstring type;
  itsImpl->itsInStream >> type; dbgEval(3, type);

  if (type == "NULL")
    {
      return SoftRef<IO::IoObject>();
    }

  Ref<IO::IoObject> obj(Util::ObjMgr::newTypedObj<IO::IoObject>(type));
  dbgEvalNL(3, obj->ioTypename());

  itsImpl->inflateObject(name, obj);

  return obj;
}

void IO::LegacyReader::readOwnedObject(const fstring& name,
                                       Ref<IO::IoObject> obj)
{
DOTRACE("IO::LegacyReader::readOwnedObject");

  itsImpl->readTypename(obj->ioTypename());
  itsImpl->inflateObject(name, obj);
}

void IO::LegacyReader::readBaseClass(const fstring& baseClassName,
                                     Ref<IO::IoObject> basePart)
{
DOTRACE("IO::LegacyReader::readBaseClass");

  itsImpl->readTypename(basePart->ioTypename());
  itsImpl->inflateObject(baseClassName, basePart);
}

Ref<IO::IoObject> IO::LegacyReader::readRoot(IO::IoObject* givenRoot)
{
DOTRACE("IO::LegacyReader::readRoot");
  if (givenRoot == 0)
    {
      return readObject("rootObject");
    }

  dbgEvalNL(3, givenRoot->ioTypename());

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

  void throwIfError(const char* type)
  {
    if (itsOutStream.fail())
      {
        dbgPrint(3, "throwIfError for"); dbgEvalNL(3, type);
        throw IO::WriteError(type);
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
        throwIfError(obj_name);
        return;
      }

    Assert(obj.isValid());

    stream() << obj->ioTypename() << itsFSep;
    throwIfError(obj->ioTypename().c_str());

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
          obj->writeTo(itsOwner);
        }
        requestWhitespace();
        stream() << "}";
      }

    if (itsIndentLevel > 0)
      requestWhitespace();
    else
      requestNewline();

    throwIfError(obj_name);
  }
};

///////////////////////////////////////////////////////////////////////
//
// LegacyWriter member definitions
//
///////////////////////////////////////////////////////////////////////


IO::LegacyWriter::LegacyWriter(STD_IO::ostream& os, bool write_bases) :
  itsImpl(new Impl(this, os, write_bases))
{
DOTRACE("IO::LegacyWriter::LegacyWriter");
}

IO::LegacyWriter::~LegacyWriter()
{
DOTRACE("IO::LegacyWriter::~LegacyWriter");
  itsImpl->flushWhitespace();
  delete itsImpl;
}

void IO::LegacyWriter::usePrettyPrint(bool yes)
{
DOTRACE("IO::LegacyWriter::usePrettyPrint");
  itsImpl->usePrettyPrint(yes);
}

void IO::LegacyWriter::writeChar(const char* name, char val)
{
DOTRACE("IO::LegacyWriter::writeChar");
  itsImpl->stream() << val << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeInt(const char* name, int val)
{
DOTRACE("IO::LegacyWriter::writeInt");
  itsImpl->stream() << val << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeBool(const char* name, bool val)
{
DOTRACE("IO::LegacyWriter::writeBool");
  itsImpl->stream() << val << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeDouble(const char* name, double val)
{
DOTRACE("IO::LegacyWriter::writeDouble");
  itsImpl->stream() << val << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeCstring(const char* name, const char* val)
{
DOTRACE("IO::LegacyWriter::writeCstring");

  itsImpl->stream() << strlen(val) << " "
                    << val << itsImpl->itsFSep;

  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeValueObj(const char* name,
                                     const Value& value)
{
DOTRACE("IO::LegacyWriter::writeValueObj");
  value.printTo(itsImpl->stream());
  itsImpl->stream() << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeObject(const char* name,
                                   SoftRef<const IO::IoObject> obj)
{
DOTRACE("IO::LegacyWriter::writeObject");

  itsImpl->flattenObject(name, obj);
}

void IO::LegacyWriter::writeOwnedObject(const char* name,
                                        Ref<const IO::IoObject> obj)
{
DOTRACE("IO::LegacyWriter::writeOwnedObject");

  itsImpl->flattenObject(name, obj);
}

void IO::LegacyWriter::writeBaseClass(const char* baseClassName,
                                      Ref<const IO::IoObject> basePart)
{
DOTRACE("IO::LegacyWriter::writeBaseClass");
  if (itsImpl->itsWriteBases)
    {
      itsImpl->flattenObject(baseClassName, basePart);
    }
  else
    {
      itsImpl->flattenObject(baseClassName, basePart, true);
    }
}

void IO::LegacyWriter::writeRoot(const IO::IoObject* root)
{
DOTRACE("IO::LegacyWriter::writeRoot");

  itsImpl->flattenObject
    ("rootObject", SoftRef<IO::IoObject>(const_cast<IO::IoObject*>(root),
                                         Util::STRONG,
                                         Util::PRIVATE));
}

static const char vcid_iolegacy_cc[] = "$Header$";
#endif // !IOLEGACY_CC_DEFINED
