///////////////////////////////////////////////////////////////////////
//
// nullresponsehdlr.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 18:54:35 1999
// written: Wed Jun 26 12:56:46 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NULLRESPONSEHDLR_H_DEFINED
#define NULLRESPONSEHDLR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(RESPONSEHANDLER_H_DEFINED)
#include "visx/responsehandler.h"
#endif

/// NullResponseHdlr implements ResponseHandler by ignoring all responses.
class NullResponseHdlr : public ResponseHandler
{
protected:
  NullResponseHdlr();
public:
  static NullResponseHdlr* make();

  virtual ~NullResponseHdlr();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  // actions
  virtual void rhBeginTrial(Util::SoftRef<GWT::Widget> widget,
                            TrialBase& trial) const;
  virtual void rhAbortTrial() const;
  virtual void rhEndTrial() const;
  virtual void rhHaltExpt() const;
  virtual void rhAllowResponses(Util::SoftRef<GWT::Widget> widget,
                                TrialBase& trial) const;
  virtual void rhDenyResponses() const;
};

static const char vcid_nullresponsehdlr_h[] = "$Header$";
#endif // !NULLRESPONSEHDLR_H_DEFINED
