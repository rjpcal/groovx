///////////////////////////////////////////////////////////////////////
// subject.h
// Rob Peters
// created: Dec-98
// written: Thu Oct 21 18:13:44 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECT_H_DEFINED
#define SUBJECT_H_DEFINED

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

  const char* getName() const { return itsName; }
  void setName(const char* name);
  const char* getDirectory() const { return itsDirectory; }
  void setDirectory(const char* dir);
private:
  char* itsName;
  char* itsDirectory;
};

static const char vcid_subject_h[] = "$Header$";
#endif // !SUBJECT_H_DEFINED
