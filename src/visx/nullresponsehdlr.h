///////////////////////////////////////////////////////////////////////
//
// nullresponsehdlr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:54:35 1999
// written: Mon Jun 21 19:11:15 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NULLRESPONSEHDLR_H_DEFINED
#define NULLRESPONSEHDLR_H_DEFINED

#ifndef RESPONSEHANDLER_H_DEFINED
#include "responsehandler.h"
#endif

class NullResponseHdlr : public ResponseHandler {
public:
  NullResponseHdlr();
  virtual ~NullResponseHdlr();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  // manipulators/accessors
  virtual void setInterp(Tcl_Interp* interp);

  // actions
  virtual void rhBeginTrial() const;
  virtual void rhAbortTrial() const;
  virtual void rhHaltExpt() const;
};

static const char vcid_nullresponsehdlr_h[] = "$Header$";
#endif // !NULLRESPONSEHDLR_H_DEFINED
