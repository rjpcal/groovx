///////////////////////////////////////////////////////////////////////
//
// subject.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Dec-98
// written: Tue Aug 14 21:38:06 2001
// $Id$
//
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

class Subject : public IO::IoObject {
protected:
  Subject(const char* name=0, const char* dir=0);
  virtual ~Subject();

public:
  static Subject* make();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  const fstring& getName() const { return itsName; }
  void setName(const fstring& name);
  const fstring& getDirectory() const { return itsDirectory; }
  void setDirectory(const fstring& dir);

private:
  Subject(const Subject&);
  Subject& operator=(const Subject&);

  fstring itsName;
  fstring itsDirectory;
};

static const char vcid_subject_h[] = "$Header$";
#endif // !SUBJECT_H_DEFINED
