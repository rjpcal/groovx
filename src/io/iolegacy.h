///////////////////////////////////////////////////////////////////////
//
// iolegacy.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 26 18:47:31 2000
// written: Thu Sep 28 19:50:23 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOLEGACY_H_DEFINED
#define IOLEGACY_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(READER_H_DEFINED)
#include "io/reader.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(WRITER_H_DEFINED)
#include "io/writer.h"
#endif

#ifdef PRESTANDARD_IOSTREAMS
class istream;
class ostream;
#else
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#    include <iosfwd>
#    define IOSFWD_DEFINED
#  endif
#endif

///////////////////////////////////////////////////////////////////////
//
// Forward class declarations
//
///////////////////////////////////////////////////////////////////////

namespace IO {
  class LegacyReader;
  class LegacyWriter;

  class LRFlagJanitor;
  class LWFlagJanitor;

  enum LegacyStringMode { GETLINE_NEWLINE, GETLINE_TAB, CHAR_COUNT };
}


///////////////////////////////////////////////////////////////////////
//
// IO::LegacyReader class definition
//
///////////////////////////////////////////////////////////////////////

class IO::LegacyReader : public IO::Reader {
public:
  LegacyReader(STD_IO::istream& is, IO::IOFlag flag);
  virtual ~LegacyReader();

  IO::IOFlag flags() const;

  void throwIfError(const char* type);

  int eatWhitespace();

  int peek();

  int getChar();

  void setStringMode(LegacyStringMode mode);

  virtual IO::VersionId readSerialVersionId();

  virtual char readChar(const char* name);
  virtual int readInt(const char* name);
  virtual bool readBool(const char* name);
  virtual double readDouble(const char* name);
  virtual void readValueObj(const char* name, Value& value);

  virtual IO::IoObject* readObject(const char* name);
  virtual void readOwnedObject(const char* name, IO::IoObject* obj);
  virtual void readBaseClass(const char* baseClassName, IO::IoObject* basePart);

  virtual IO::IoObject* readRoot(IO::IoObject* root=0);

protected:
  virtual char* readCstring(const char* name);

private:
  friend class IO::LRFlagJanitor;
  void setFlags(IO::IOFlag new_flags);

  class Impl;
  friend class Impl;

  Impl* const itsImpl;
};


///////////////////////////////////////////////////////////////////////
//
// IO::LegacyWriter class definition
//
///////////////////////////////////////////////////////////////////////

class IO::LegacyWriter : public IO::Writer {
public:
  LegacyWriter(STD_IO::ostream& os, IO::IOFlag flag);
  virtual ~LegacyWriter();

  IO::IOFlag flags() const;

  void throwIfError(const char* type);

  void setFieldSeparator(char sep);
  void setFieldSeparator(const char* sep);
  void resetFieldSeparator();

  void insertChar(char c);

  void setStringMode(LegacyStringMode mode);

  virtual void writeChar(const char* name, char val);
  virtual void writeInt(const char* name, int val);
  virtual void writeBool(const char* name, bool val);
  virtual void writeDouble(const char* name, double val);
  virtual void writeValueObj(const char* name, const Value& value);

  virtual void writeObject(const char* name, const IO::IoObject* obj);
  virtual void writeOwnedObject(const char* name, const IO::IoObject* obj);
  virtual void writeBaseClass(const char* baseClassName, const IO::IoObject* basePart);

  virtual void writeRoot(const IO::IoObject* root);

protected:
  virtual void writeCstring(const char* name, const char* val);

private:
  friend class IO::LWFlagJanitor;
  void setFlags(IO::IOFlag new_flags);

  class Impl;
  friend class Impl;

  Impl* const itsImpl;
};


///////////////////////////////////////////////////////////////////////
//
// FlagJanitors class definitions
//
///////////////////////////////////////////////////////////////////////

class IO::LRFlagJanitor {
public:
  LRFlagJanitor(IO::LegacyReader& rdr, IO::IOFlag new_flags) :
	 itsReader(rdr),
	 itsOldFlags(rdr.flags())
  {
	 itsReader.setFlags(new_flags);
  }

  ~LRFlagJanitor()
  {
	 itsReader.setFlags(itsOldFlags);
  }

private:
  IO::LegacyReader& itsReader;
  IO::IOFlag itsOldFlags;
};

class IO::LWFlagJanitor {
public:
  LWFlagJanitor(IO::LegacyWriter& wrtr, IO::IOFlag new_flags) :
	 itsWriter(wrtr),
	 itsOldFlags(wrtr.flags())
  {
	 itsWriter.setFlags(new_flags);
  }

  ~LWFlagJanitor()
  {
	 itsWriter.setFlags(itsOldFlags);
  }

private:
  IO::LegacyWriter& itsWriter;
  IO::IOFlag itsOldFlags;
};

static const char vcid_iolegacy_h[] = "$Header$";
#endif // !IOLEGACY_H_DEFINED
