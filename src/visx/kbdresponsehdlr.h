///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 18:09:11 1999
// written: Wed Sep 25 19:02:43 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_H_DEFINED
#define KBDRESPONSEHDLR_H_DEFINED

#include "visx/eventresponsehdlr.h"


/// KbdResponseHdlr is just EventResponseHdlr with a default "keypress" event type.
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
