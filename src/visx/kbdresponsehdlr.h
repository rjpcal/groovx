///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:09:11 1999
// written: Wed Jul 21 15:53:02 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_H_DEFINED
#define KBDRESPONSEHDLR_H_DEFINED

#ifndef TCL_H_DEFINED
#include <tcl.h>
#define TCL_H_DEFINED
#endif

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef RESPONSEHANDLER_H_DEFINED
#include "responsehandler.h"
#endif

#ifndef TCLOBJLOCK_H_DEFINED
#include "tclobjlock.h"
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

  // manipulators/accessors
  virtual void setInterp(Tcl_Interp* interp);

  void setKeyRespPairs(const string& s) { 
	 itsKeyRespPairs = s; 
	 updateRegexps();
  }
  const string& getKeyRespPairs() const { return itsKeyRespPairs; }

  bool getUseFeedback() const { return itsUseFeedback; }
  void setUseFeedback(bool val) { itsUseFeedback = val; }

  const char* getFeedbackPairs() const;
  void setFeedbackPairs(const char* feedback_string);

  // actions
  virtual void rhBeginTrial() const;
  virtual void rhAbortTrial() const;
  virtual void rhHaltExpt() const;

protected:
  virtual void attend() const;
  virtual void ignore() const;

private:
  static Tcl_ObjCmdProc handleCmd;
  int handleResponse(const char* keysym) const;
  virtual int getRespFromKeysym(const char* keysym) const;
  int feedback(int response) const;
  void updateFeedbacks();
  void updateRegexps();
  struct RegExp_ResponseVal {
    RegExp_ResponseVal(Tcl_RegExp rx, int rv) : regexp(rx), resp_val(rv) {}
    Tcl_RegExp regexp;
    int resp_val;
  };

  string itsKeyRespPairs;
  mutable vector<RegExp_ResponseVal> itsRegexps;

  string itsFeedbackPairs;
  bool itsUseFeedback;

  struct Condition_Feedback {
  	 Condition_Feedback(Tcl_Obj* cond, Tcl_Obj* res);
	 Condition_Feedback(const Condition_Feedback& rhs);
	 Condition_Feedback& operator=(const Condition_Feedback& rhs);

  	 TclObjPtr condition;
  	 TclObjPtr result; 
  };
//    struct Condition_Feedback {
//  	 Condition_Feedback(Tcl_Obj* cond, Tcl_Obj* res) :
//  		condition(cond), result(res) {}		
//  	 Tcl_Obj* condition;
//  	 Tcl_Obj* result; 
//    };
  mutable vector<Condition_Feedback> itsFeedbacks;

  mutable const char* itsRawResponse;
};

static const char vcid_kbdresponsehdlr_h[] = "$Header$";
#endif // !KBDRESPONSEHDLR_H_DEFINED
