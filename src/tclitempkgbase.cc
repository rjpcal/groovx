///////////////////////////////////////////////////////////////////////
//
// tclitempkgbase.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed May 31 18:32:51 2000
// written: Thu Jul 12 13:15:59 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKGBASE_CC_DEFINED
#define TCLITEMPKGBASE_CC_DEFINED

#include "tcl/tclitempkgbase.h"

#include "tcl/tclcmd.h"
#include "tcl/tclvalue.h"

#include "util/strings.h"

#include "util/debug.h"
#include "util/trace.h"

template <class T>
Getter<T>::~Getter() { DebugEvalNL((void*)this); }

template <class T>
Setter<T>::~Setter() {}

Action::~Action() {}

template class Getter<int>;
template class Getter<unsigned int>;
template class Getter<unsigned long>;
template class Getter<bool>;
template class Getter<double>;
template class Getter<const char*>;
template class Getter<fixed_string>;
template class Getter<const fixed_string&>;
template class Getter<Tcl::TclValue>;

template class Setter<int>;
template class Setter<unsigned int>;
template class Setter<unsigned long>;
template class Setter<bool>;
template class Setter<double>;
template class Setter<const char*>;
template class Setter<const fixed_string&>;
template class Setter<Tcl::TclValue>;

Tcl::ItemFetcher::~ItemFetcher() {}

static const char vcid_tclitempkgbase_cc[] = "$Header$";
#endif // !TCLITEMPKGBASE_CC_DEFINED
