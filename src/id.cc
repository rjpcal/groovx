///////////////////////////////////////////////////////////////////////
//
// id.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu May 27 18:32:00 1999
// written: Thu Jul 22 12:52:47 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ID_CC_DEFINED
#define ID_CC_DEFINED

#include "id.h"

#include <iostream.h>

#include "objlist.h"
#include "poslist.h"

//--------------------------------------------------------------------
//
// Id::operator!() specializations
//
//--------------------------------------------------------------------

template<>
bool Id<Position, PosList>::operator!() const {
  return !( PosList::thePosList().isValidId(this->toInt()) );
}

template<>
bool Id<GrObj, ObjList>::operator!() const {
  return !( ObjList::theObjList().isValidId(this->toInt()) );
}

//--------------------------------------------------------------------
//
// Id::get() specializations
//
//--------------------------------------------------------------------

template<>
Position* Id<Position, PosList>::get() const {
  return PosList::thePosList().getPtr(this->toInt());
}

template<>
GrObj* Id<GrObj, ObjList>::get() const {
  return ObjList::theObjList().getPtr(this->toInt());
}

//--------------------------------------------------------------------
//
// operator<<() and operator>>() definitions
//
//--------------------------------------------------------------------

template<class T, class C>
istream& operator>>(istream& is, Id<T, C>& id) {
  return (is >> id.itsIndex);
}

template<class T, class C>
ostream& operator<<(ostream& os, const Id<T, C>& id) {
  return (os << id.itsIndex);
}

//--------------------------------------------------------------------
//
// Explicit template instantiation requests
//
//--------------------------------------------------------------------

template class Id<Position, PosList>;
template class Id<GrObj, ObjList>;

template istream& operator>>(istream&, Id<Position, PosList>&);
template istream& operator>>(istream&, Id<GrObj, ObjList>&);

template ostream& operator<<(ostream&, const Id<Position, PosList>&);
template ostream& operator<<(ostream&, const Id<GrObj, ObjList>&);

static const char vcid_id_cc[] = "$Header$";
#endif // !ID_CC_DEFINED
