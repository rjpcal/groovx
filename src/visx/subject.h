///////////////////////////////////////////////////////////////////////
// subject.h
// Rob Peters
// created: Dec-98
// written: Fri Mar 12 11:25:16 1999
static const char vcid[] = "$Id$";
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

  const char* getName() const { return itsName; }
  void setName(const char* name);
  const char* getDirectory() const { return itsDirectory; }
  void setDirectory(const char* dir);
private:
  char *itsName;
  char *itsDirectory;
};

#endif // !SUBJECT_H_DEFINED
