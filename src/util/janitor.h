///////////////////////////////////////////////////////////////////////
//
// janitor.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Mar 23 17:35:40 2000
// written: Thu Mar 23 17:36:34 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JANITOR_H_DEFINED
#define JANITOR_H_DEFINED

namespace Util {

template <class C, class T>
class Janitor {
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
