///////////////////////////////////////////////////////////////////////
//
// subject.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Dec-98
// written: Mon Jun 25 06:42:23 2001
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

class Subject : public virtual IO::IoObject {
protected:
  Subject(const char* name="", const char* dir="");
  virtual ~Subject();

public:
  static Subject* make();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  const fixed_string& getName() const { return itsName; }
  void setName(const fixed_string& name);
  const fixed_string& getDirectory() const { return itsDirectory; }
  void setDirectory(const fixed_string& dir);

private:
  Subject(const Subject&);
  Subject& operator=(const Subject&);

  fixed_string itsName;
  fixed_string itsDirectory;
};

static const char vcid_subject_h[] = "$Header$";
#endif // !SUBJECT_H_DEFINED
