///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 18:09:11 1999
// written: Mon Jan 13 11:08:25 2003
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
