///////////////////////////////////////////////////////////////////////
//
// masterptr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct  9 08:19:46 2000
// written: Mon Oct  9 13:28:24 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASTERPTR_CC_DEFINED
#define MASTERPTR_CC_DEFINED

#include "masterptr.h"

#include "util/error.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// MasterPtrBase member definitions
//
///////////////////////////////////////////////////////////////////////

void MasterPtrBase::throwErrorWithMsg(const char* msg) {
DOTRACE("MasterPtrBase::throwErrorWithMsg");
  throw ErrorWithMsg(msg);
}

MasterPtrBase::MasterPtrBase() :
  itsRefCount(0)
{
DOTRACE("MasterPtrBase::MasterPtrBase");
}

MasterPtrBase::~MasterPtrBase()
{
DOTRACE("MasterPtrBase::~MasterPtrBase");
}

void MasterPtrBase::incrRefCount() {
DOTRACE("MasterPtrBase::incrRefCount");
  ++itsRefCount;
}

void MasterPtrBase::decrRefCount() {
DOTRACE("MasterPtrBase::decrRefCount");
  --itsRefCount;
  if (itsRefCount <= 0)
	 delete this;
}

bool MasterPtrBase::isShared() const {
DOTRACE("MasterPtrBase::isShared");
  return (itsRefCount > 1);
}

bool MasterPtrBase::isUnshared() const {
DOTRACE("MasterPtrBase::isUnshared");
  return (itsRefCount <= 1);
}

int MasterPtrBase::refCount() const {
DOTRACE("MasterPtrBase::refCount");
  return itsRefCount;
}

///////////////////////////////////////////////////////////////////////
//
// NullMasterPtr member definitions
//
///////////////////////////////////////////////////////////////////////

NullMasterPtr::NullMasterPtr() :
	 MasterPtrBase()
{
DOTRACE("NullMasterPtr::NullMasterPtr");
}

NullMasterPtr::~NullMasterPtr() {
DOTRACE("NullMasterPtr::~NullMasterPtr");
}

bool NullMasterPtr::isValid() const {
DOTRACE("NullMasterPtr::isValid");
  return false;
}

bool NullMasterPtr::operator==(const MasterPtrBase& other) { 
DOTRACE("NullMasterPtr::operator==");
  bool otherIsNull = (dynamic_cast<const NullMasterPtr*>(&other) != 0);
  return otherIsNull;
}

///////////////////////////////////////////////////////////////////////
//
// MasterIoPtr member definitions
//
///////////////////////////////////////////////////////////////////////

MasterIoPtr::MasterIoPtr()
{
DOTRACE("MasterIoPtr::MasterIoPtr");
}

MasterIoPtr::~MasterIoPtr()
{
DOTRACE("MasterIoPtr::~MasterIoPtr");
}

static const char vcid_masterptr_cc[] = "$Header$";
#endif // !MASTERPTR_CC_DEFINED
