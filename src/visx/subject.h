///////////////////////////////////////////////////////////////////////
// subject.h
// Rob Peters
// created: Dec-98
// written: Sat Mar 13 15:13:10 1999
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECT_H_DEFINED
#define SUBJECT_H_DEFINED

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

///////////////////////////////////////////////////////////////////////
// Subject class
///////////////////////////////////////////////////////////////////////

class Subject : public virtual IO {
public:
  Subject(const char *name, const char* dir);
  Subject(istream &is);
  virtual ~Subject();

  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const;
  virtual IOResult deserialize(istream &is, IOFlag flag = NO_FLAGS);

  const char* getName() const { return itsName; }
  void setName(const char* name);
  const char* getDirectory() const { return itsDirectory; }
  void setDirectory(const char* dir);
private:
  char *itsName;
  char *itsDirectory;
};

static const char vcid_subject_h[] = "$Id$";
#endif // !SUBJECT_H_DEFINED
