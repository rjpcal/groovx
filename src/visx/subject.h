///////////////////////////////////////////////////////////////////////
//
// subject.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  1 08:00:00 1998
// written: Wed Sep 25 19:03:25 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECT_H_DEFINED
#define SUBJECT_H_DEFINED

#include "io/io.h"

#include "util/strings.h"

///////////////////////////////////////////////////////////////////////
// Subject class
///////////////////////////////////////////////////////////////////////

class Subject : public IO::IoObject
{
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
