///////////////////////////////////////////////////////////////////////
//
// value.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 28 11:21:32 1999
// written: Tue Sep 28 11:24:56 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_CC_DEFINED
#define VALUE_CC_DEFINED

#include "value.h"

ValueError::ValueError(const string& msg) :
  ErrorWithMsg(msg)
{}

Value::~Value() {}

static const char vcid_value_cc[] = "$Header$";
#endif // !VALUE_CC_DEFINED
