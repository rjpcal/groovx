///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun 21 18:09:12 1999
// written: Wed Mar 19 12:46:30 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_CC_DEFINED
#define KBDRESPONSEHDLR_CC_DEFINED

#include "visx/kbdresponsehdlr.h"

#include "util/strings.h"

#include "util/trace.h"

///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr method definitions
//
///////////////////////////////////////////////////////////////////////

KbdResponseHdlr* KbdResponseHdlr::make()
{
DOTRACE("KbdResponseHdlr::make");
  return new KbdResponseHdlr;
}

KbdResponseHdlr::KbdResponseHdlr() :
  EventResponseHdlr()
{
  setEventSequence("<KeyPress>");
  setBindingSubstitution("%K");
}

KbdResponseHdlr::~KbdResponseHdlr() {}

static const char vcid_kbdresponsehdlr_cc[] = "$Header$";
#endif // !KBDRESPONSEHDLR_CC_DEFINED
