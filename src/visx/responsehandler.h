///////////////////////////////////////////////////////////////////////
// responsehandler.h
// Rob Peters
// created: Tue May 18 16:21:09 1999
// written: Thu May 20 11:44:57 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSEHANDLER_H_DEFINED
#define RESPONSEHANDLER_H_DEFINED

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

class ExptDriver;

///////////////////////////////////////////////////////////////////////
//
// ResponseHandler class defintion
//
///////////////////////////////////////////////////////////////////////

class ResponseHandler {
public:
  // creators
  ResponseHandler(ExptDriver& ed, Tcl_Interp* interp, const string& s="");
  virtual ~ResponseHandler();

  // actions
  virtual int attend() const;
  virtual int ignore() const;

  // manipulators/accessors
  void setKeyRespPairs(const string& s) { 
	 itsKeyRespPairs = s; 
	 updateRegexps();
  }
  const string& getKeyRespPairs() const { return itsKeyRespPairs; }

  bool getUseFeedback() const { return itsUseFeedback; }
  void setUseFeedback(bool val) { itsUseFeedback = val; }

private:
  int handleResponse(const char* keysym);
  virtual int getRespFromKeysym(const char* keysym) const;
  int feedback(int response);
  static Tcl_ObjCmdProc handleCmd;
  int updateRegexps();

  struct RegExp_ResponseVal {
    RegExp_ResponseVal(Tcl_RegExp rx, int rv) : regexp(rx), resp_val(rv) {}
    Tcl_RegExp regexp;
    int resp_val;
  };

  ExptDriver& itsExptDriver;
  Tcl_Interp* itsInterp;
  string itsKeyRespPairs;
  vector<RegExp_ResponseVal> itsRegexps;
  bool itsUseFeedback;
};

static const char vcid_responsehandler_h[] = "$Header$";
#endif // !RESPONSEHANDLER_H_DEFINED
