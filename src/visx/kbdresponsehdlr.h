///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:09:11 1999
// written: Tue Nov  9 09:27:46 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_H_DEFINED
#define KBDRESPONSEHDLR_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef RESPONSEHANDLER_H_DEFINED
#include "responsehandler.h"
#endif


///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr class defintion
//
///////////////////////////////////////////////////////////////////////

class KbdResponseHdlr : public ResponseHandler {
public:
  // creators
  KbdResponseHdlr(const string& s="");
  virtual ~KbdResponseHdlr();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  // manipulators/accessors
  virtual void setInterp(Tcl_Interp* interp);

  void setKeyRespPairs(const string& s);
  const string& getKeyRespPairs() const;

  bool getUseFeedback() const;
  void setUseFeedback(bool val);

  const char* getFeedbackPairs() const;
  void setFeedbackPairs(const char* feedback_string);

  // actions
  virtual void rhBeginTrial() const;
  virtual void rhAbortTrial() const;
  virtual void rhHaltExpt() const;

private:
  class Impl;
  friend class KbdResponseHdlr::Impl;
  Impl* const itsImpl;
};

static const char vcid_kbdresponsehdlr_h[] = "$Header$";
#endif // !KBDRESPONSEHDLR_H_DEFINED
