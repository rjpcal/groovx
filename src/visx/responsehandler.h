///////////////////////////////////////////////////////////////////////
// responsehandler.h
// Rob Peters
// created: Tue May 18 16:21:09 1999
// written: Fri Jun 11 20:23:38 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSEHANDLER_H_DEFINED
#define RESPONSEHANDLER_H_DEFINED

#ifndef TK_H_DEFINED
#include <tk.h>
#define TK_H_DEFINED
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

#ifndef IO_H_DEFINED
#include "io.h"
#endif

class ExptDriver;

///////////////////////////////////////////////////////////////////////
//
// ResponseHandler class defintion
//
///////////////////////////////////////////////////////////////////////

class ResponseHandler : public virtual IO {
public:
  // creators
  ResponseHandler(const string& s="");
  virtual ~ResponseHandler();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  // manipulators/accessors
  void setInterp(Tcl_Interp* interp);

  void setKeyRespPairs(const string& s) { 
	 itsKeyRespPairs = s; 
	 updateRegexps();
  }
  const string& getKeyRespPairs() const { return itsKeyRespPairs; }

  bool getUseFeedback() const { return itsUseFeedback; }
  void setUseFeedback(bool val) { itsUseFeedback = val; }

  // actions
  virtual void rhBeginTrial() const;
  virtual void rhAbortTrial() const;
  virtual void rhHaltExpt() const;

protected:
  virtual void attend() const;
  virtual void ignore() const;

private:
  static Tcl_ObjCmdProc handleCmd;
  static Tk_EventProc eventProc;
  int handleResponse(const char* keysym) const;
  virtual int getRespFromKeysym(const char* keysym) const;
  int feedback(int response) const;
  void updateRegexps();
  struct RegExp_ResponseVal {
    RegExp_ResponseVal(Tcl_RegExp rx, int rv) : regexp(rx), resp_val(rv) {}
    Tcl_RegExp regexp;
    int resp_val;
  };

  mutable Tcl_Interp* itsInterp;
  string itsKeyRespPairs;
  vector<RegExp_ResponseVal> itsRegexps;
  bool itsUseFeedback;

  mutable const char* itsRawResponse;
};

static const char vcid_responsehandler_h[] = "$Header$";
#endif // !RESPONSEHANDLER_H_DEFINED
