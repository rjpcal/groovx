///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 18:09:12 1999
// written: Fri Jan 18 16:07:01 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_CC_DEFINED
#define KBDRESPONSEHDLR_CC_DEFINED

#include "visx/kbdresponsehdlr.h"

#include "util/strings.h"

#include "util/trace.h"
#include "util/debug.h"

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
  EventResponseHdlr("")
{
  setEventSequence("<KeyPress>");
  setBindingSubstitution("%K");
}

KbdResponseHdlr::KbdResponseHdlr(const char* key_resp_pairs) :
  EventResponseHdlr(key_resp_pairs)
{
  setEventSequence("<KeyPress>");
  setBindingSubstitution("%K");
}

KbdResponseHdlr::~KbdResponseHdlr() {}

static const char vcid_kbdresponsehdlr_cc[] = "$Header$";
#endif // !KBDRESPONSEHDLR_CC_DEFINED
