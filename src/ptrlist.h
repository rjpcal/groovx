///////////////////////////////////////////////////////////////////////
//
// ptrlist.h
// Rob Peters
// created: Fri Apr 23 00:35:31 1999
// written: Fri Oct 27 16:45:33 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_H_DEFINED
#define PTRLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOPTRLIST_H_DEFINED)
#include "ioptrlist.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IDITEM_H_DEFINED)
#include "iditem.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * PtrList is templatized container that stores reference counted
 * smart pointers. Objects are accessed by integer indices into
 * the PtrList. There are no operations on the capacity of PtrList;
 * any necessary resizing is done when necessary in an insert
 * call. PtrList is what Jeff Alger (in C++ for Real Programmers)
 * calls a "teenage mutable serializable indexed ninja collection".
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class PtrList : public IoPtrList {
public:
  /// Default constructor makes an empty list.
  PtrList();

  /// Virtual destructor.
  virtual ~PtrList();
};

static const char vcid_ptrlist_h[] = "$Header$";
#endif // !PTRLIST_H_DEFINED
