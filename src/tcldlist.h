///////////////////////////////////////////////////////////////////////
//
// tcldlist.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Dec-98
// written: Thu May 10 12:04:37 2001
// $Id$
//
// This package provides additional list manipulation functions using
// object-style (rather than string-style) Tcl commands
//
// The following Tcl functions are implemented:
//      dlist_choose    form a permutation of a list
//      dlist_ones      form a list with n 1's
//      dlist_zeros     form a list with n 0's
//      dlist_sum       return the numerical sum of a list's elements
//      dlist_repeat    form a new list by repeating a list's elements
//      dlist_select    choose a subset of a list's elements
//      dlist_not       form the element-wise logical not of a list
//      dlist_pickone   return a randomly chosen list element
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLDLIST_H_DEFINED
#define TCLDLIST_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp* interp);

extern "C" Tcl_PackageInitProc Tcldlist_Init;

static const char vcid_tcldlist_h[] = "$Id$";
#endif // !TCLDLIST_H_DEFINED
