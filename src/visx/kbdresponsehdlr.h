///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 18:09:11 1999
// written: Wed Jan 30 11:25:46 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_H_DEFINED
#define KBDRESPONSEHDLR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(EVENTRESPONSEHDLR_H_DEFINED)
#include "visx/eventresponsehdlr.h"
#endif


///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr class defintion
//
///////////////////////////////////////////////////////////////////////

class KbdResponseHdlr : public EventResponseHdlr
{
protected:
  KbdResponseHdlr();
public:
  static KbdResponseHdlr* make();

  virtual ~KbdResponseHdlr();
};

static const char vcid_kbdresponsehdlr_h[] = "$Header$";
#endif // !KBDRESPONSEHDLR_H_DEFINED
