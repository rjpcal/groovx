///////////////////////////////////////////////////////////////////////
// subject.h
// Rob Peters
// created: Dec-98
// written: Mon Mar  6 18:39:01 2000
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECT_H_DEFINED
#define SUBJECT_H_DEFINED

#ifndef STRINGS_H_DEFINED
#include "util/strings.h"
#define STRING_DEFINED
#endif

#ifndef IO_H_DEFINED
#include "io.h"
#endif

///////////////////////////////////////////////////////////////////////
// Subject class
///////////////////////////////////////////////////////////////////////

class Subject : public virtual IO {
public:
  Subject(const char* name, const char* dir);
  Subject(istream &is, IOFlag flag);
  virtual ~Subject();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  const char* getName() const { return itsName.c_str(); }
  void setName(const char* name);
  const char* getDirectory() const { return itsDirectory.c_str(); }
  void setDirectory(const char* dir);

private:
  fixed_string itsName;
  fixed_string itsDirectory;
};

static const char vcid_subject_h[] = "$Header$";
#endif // !SUBJECT_H_DEFINED
