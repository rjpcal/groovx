///////////////////////////////////////////////////////////////////////
//
// tclitempkgbase.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed May 31 18:32:51 2000
// written: Wed May 31 19:00:34 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKGBASE_CC_DEFINED
#define TCLITEMPKGBASE_CC_DEFINED

#include "tcl/tclitempkgbase.h"

#include "util/strings.h"

template <class T>
Getter<T>::~Getter() {}

template <class T>
Setter<T>::~Setter() {}

template <class T>
Attrib<T>::~Attrib() {}

template class Getter<int>;
template class Getter<bool>;
template class Getter<double>;
template class Getter<const char*>;
template class Getter<const fixed_string&>;

template class Setter<int>;
template class Setter<bool>;
template class Setter<double>;
template class Setter<const char*>;
template class Setter<const fixed_string&>;

template class Attrib<int>;
template class Attrib<bool>;
template class Attrib<double>;
template class Attrib<const char*>;
template class Attrib<const fixed_string&>;

Tcl::TclItemPkgBase::TclItemPkgBase(Tcl_Interp* interp,
												const char* name, const char* version) :
  TclPkg(interp, name, version)
{}

Tcl::TclItemPkgBase::~TclItemPkgBase() {}

static const char vcid_tclitempkgbase_cc[] = "$Header$";
#endif // !TCLITEMPKGBASE_CC_DEFINED
