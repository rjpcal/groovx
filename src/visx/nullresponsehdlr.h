///////////////////////////////////////////////////////////////////////
//
// nullresponsehdlr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:54:35 1999
// written: Thu Mar 30 09:50:01 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NULLRESPONSEHDLR_H_DEFINED
#define NULLRESPONSEHDLR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(RESPONSEHANDLER_H_DEFINED)
#include "responsehandler.h"
#endif

class NullResponseHdlr : public ResponseHandler {
public:
  NullResponseHdlr();
  virtual ~NullResponseHdlr();

  virtual void serialize(ostream &os, IO::IOFlag flag) const;
  virtual void deserialize(istream &is, IO::IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  // manipulators/accessors
  virtual void setInterp(Tcl_Interp* interp);

  // actions
  virtual void rhBeginTrial(Experiment* expt) const;
  virtual void rhAbortTrial(Experiment* expt) const;
  virtual void rhEndTrial(Experiment* expt) const;
  virtual void rhHaltExpt(Experiment* expt) const;
};

static const char vcid_nullresponsehdlr_h[] = "$Header$";
#endif // !NULLRESPONSEHDLR_H_DEFINED
