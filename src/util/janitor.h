///////////////////////////////////////////////////////////////////////
//
// janitor.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Mar 23 17:35:40 2000
// written: Wed Mar 19 12:45:39 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JANITOR_H_DEFINED
#define JANITOR_H_DEFINED

namespace Util
{

/// Generic class for scope-based cleanup.
template <class C, class T>
class Janitor
{
public:
  typedef T (C::* Getter)() const;
  typedef void (C::* Setter)(T);

  Janitor(C& obj, Getter g, Setter s) :
    itsObj(obj), itsGetter(g), itsSetter(s),
    itsOrigVal( (itsObj.*itsGetter)() ) {}

  ~Janitor() { (itsObj.*itsSetter)(itsOrigVal); }
private:
  C& itsObj;
  Getter itsGetter;
  Setter itsSetter;
  T itsOrigVal;
};

} // end namespace Util

static const char vcid_janitor_h[] = "$Header$";
#endif // !JANITOR_H_DEFINED
