///////////////////////////////////////////////////////////////////////
//
// tclitempkgbase.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed May 31 18:32:51 2000
// written: Wed May 31 18:35:44 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKGBASE_CC_DEFINED
#define TCLITEMPKGBASE_CC_DEFINED

#include "tcl/tclitempkgbase.h"

template <class T>
Getter<T>::~Getter() {}

template <class T>
Setter<T>::~Setter() {}

template <class T>
Attrib<T>::~Attrib() {}

Tcl::TclItemPkgBase::TclItemPkgBase(Tcl_Interp* interp,
												const char* name, const char* version) :
  TclPkg(interp, name, version)
{}

virtual ~TclItemPkgBase();

static const char vcid_tclitempkgbase_cc[] = "$Header$";
#endif // !TCLITEMPKGBASE_CC_DEFINED
