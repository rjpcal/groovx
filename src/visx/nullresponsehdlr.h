///////////////////////////////////////////////////////////////////////
//
// nullresponsehdlr.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 18:54:35 1999
// written: Sat Jul 21 20:11:10 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NULLRESPONSEHDLR_H_DEFINED
#define NULLRESPONSEHDLR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(RESPONSEHANDLER_H_DEFINED)
#include "responsehandler.h"
#endif

class NullResponseHdlr : public ResponseHandler {
protected:
  NullResponseHdlr();
public:
  static NullResponseHdlr* make();

  virtual ~NullResponseHdlr();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  // actions
  virtual void rhBeginTrial(Util::WeakRef<GWT::Widget> widget,
                            TrialBase& trial) const;
  virtual void rhAbortTrial() const;
  virtual void rhEndTrial() const;
  virtual void rhHaltExpt() const;
  virtual void rhAllowResponses(Util::WeakRef<GWT::Widget> widget,
                                TrialBase& trial) const;
  virtual void rhDenyResponses() const;
};

static const char vcid_nullresponsehdlr_h[] = "$Header$";
#endif // !NULLRESPONSEHDLR_H_DEFINED
