///////////////////////////////////////////////////////////////////////
// subject.h
// Rob Peters
// created: Dec-98
// written: Thu Mar 30 09:50:01 2000
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECT_H_DEFINED
#define SUBJECT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STRINGS_H_DEFINED)
#include "util/strings.h"
#define STRING_DEFINED
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

///////////////////////////////////////////////////////////////////////
// Subject class
///////////////////////////////////////////////////////////////////////

class Subject : public virtual IO::IoObject {
public:
  Subject(const char* name, const char* dir);
  Subject(istream &is, IO::IOFlag flag);
  virtual ~Subject();

  virtual void serialize(ostream &os, IO::IOFlag flag) const;
  virtual void deserialize(istream &is, IO::IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  const char* getName() const { return itsName.c_str(); }
  void setName(const char* name);
  const char* getDirectory() const { return itsDirectory.c_str(); }
  void setDirectory(const char* dir);

private:
  Subject(const Subject&);
  Subject& operator=(const Subject&);

  fixed_string itsName;
  fixed_string itsDirectory;
};

static const char vcid_subject_h[] = "$Header$";
#endif // !SUBJECT_H_DEFINED
