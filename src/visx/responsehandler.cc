///////////////////////////////////////////////////////////////////////
//
// responsehandler.cc
// Rob Peters
// created: Tue May 18 16:21:09 1999
// written: Thu May 20 11:45:01 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSEHANDLER_CC_DEFINED
#define RESPONSEHANDLER_CC_DEFINED

#include "responsehandler.h"

#include "exptdriver.h"
#include "tclobjlock.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// ResponseHandler method definitions
//
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
//
// ResponseHandler::ResponseHandler --
//
// This constructor preserves itsInterp so that the interp will not be
// destroyed at least until this ResponseHandler has Tcl_Release'd it.
//
//--------------------------------------------------------------------

ResponseHandler::ResponseHandler(ExptDriver& ed,
											Tcl_Interp* interp, const string& s) : 
  itsExptDriver(ed),
  itsInterp(interp),
  itsKeyRespPairs(s),
  itsRegexps(),
  itsUseFeedback(true)
{
DOTRACE("ResponseHandler::ResponseHandler");
  Tcl_Preserve(itsInterp);  
  Tcl_CreateObjCommand(itsInterp, "__ResponseHandlerPrivate::handle",
							  handleCmd, static_cast<ClientData>(this),
							  (Tcl_CmdDeleteProc *) NULL);
}

//--------------------------------------------------------------------
//
// ResponseHandler::~ResponseHandler --
//
// The destructor releases itsInterp for eventual destruction.
//
//--------------------------------------------------------------------

ResponseHandler::~ResponseHandler() {
DOTRACE("ResponseHandler::~ResponseHandler");
  // We must check if itsInterp has been tagged for deletion already,
  // since if it is then we must not attempt to use it to delete a Tcl
  // command (this results in "called Tcl_HashEntry on deleted
  // table"). Not deleting the command in that case does not cause a
  // resource leak, however, since the Tcl_Interp as part if its own
  // destruction will delete all commands associated with it.

  if ( !Tcl_InterpDeleted(itsInterp) ) {
	 DebugPrintNL("deleting Tcl command __ResponseHandlerPrivate::handle");

	 Tcl_DeleteCommand(itsInterp, "__ResponseHandlerPrivate::handle");
  }

  Tcl_Release(itsInterp);
}

//--------------------------------------------------------------------
//
// ResponseHandler::attend --
//
// When this procedure is invoked, the program listens to the events
// that are pertinent to the response policy. This procedure should be
// called when a trial is begun, and should be cancelled with ignore()
// when a response has been made so that events are not received
// during response processing and during the inter-trial period.
//
// Results:
// A usual Tcl result.
//
//--------------------------------------------------------------------

int ResponseHandler::attend() const {
DOTRACE("ResponseHandler::attend");
  static Tcl_Obj* bindCmdObj = 
    Tcl_NewStringObj("bind .togl <KeyPress> "
							"{ __ResponseHandlerPrivate::handle %K }", -1);
  static TclObjLock lock_(bindCmdObj);
  return Tcl_EvalObjEx(itsInterp, bindCmdObj, TCL_EVAL_GLOBAL);
}

//--------------------------------------------------------------------
//
// ResponseHandler::feedback --
//
// Call the user-defined feedback procedure with a given response
// value.
//
// Results:
// none.
//
// Side effects:
// Whatever the user-defined procedure "feedback" does.
//
//--------------------------------------------------------------------

int ResponseHandler::feedback(int response) {
DOTRACE("ResponseHandler::feedback");

  DebugEvalNL(response);

  static Tcl_Obj* feedbackObj = 
    Tcl_NewStringObj("::feedback", -1);
  static TclObjLock lock_(feedbackObj);

  static Tcl_Obj* feedbackObjv[2] = { feedbackObj, NULL };

  feedbackObjv[1] = Tcl_NewIntObj(response);
  TclObjLock lock2_(feedbackObjv[1]);

  return Tcl_EvalObjv(itsInterp, 2, feedbackObjv, TCL_EVAL_GLOBAL);
}

//--------------------------------------------------------------------
//
// ResponseHandler::getRespFromKeysysm --
//
// This procedure looks up a character string in the ResponseHandlers
// regexp table.
//
// Results: 
// Returns the response value associated with the first regexp in
// itsRegexps that matches the given character string, or returns -1
// if none of the regexps give a match.
//
//--------------------------------------------------------------------

int ResponseHandler::getRespFromKeysym(const char* keysym) const {
DOTRACE("ResponseHandler::getRespFromKeysym");
  // Get exactly one character -- the last character -- in keysym
  // (just before the terminating '\0'). This is the character that we
  // will take as the response. (The last character must be extracted
  // to handle the numeric keypad, where the keysysms are 'KP_0',
  // 'KP_3', etc., and we want just the 0 or the 3).
  const char* last_char = keysym;
  while ( *(last_char+1) != '\0' ) { ++last_char; }
    
  int resp = -1;
  for (int i = 0; i < itsRegexps.size(); ++i) {
    int found = Tcl_RegExpExec(itsInterp, itsRegexps[i].regexp, 
                               last_char, last_char);

	 // Return value of -1 indicates error while executing in the
	 // regular expression
    if (found == -1) return TCL_ERROR;

	 // Return value of 0 indicates no match was found, so do nothing
	 // and go to the next loop cycle
	 if (found == 0) continue;

	 // Return value of 1 indicates a match was found, so record the
	 // response value that corresponds with the regexp and exit the loop
    if (found == 1) {
      resp = itsRegexps[i].resp_val;
      break;
    }
  }
    
  return resp;
}

//--------------------------------------------------------------------
//
// ResponseHandler::handleCmd --
//
// This Tcl command procedure is intended to be private to the
// ResponseHandler. Its raison d'etre is to give a hook back into the
// C++ code from Tcl from the event binding that is set up in
// ResponseHandler::attend(). It passes control to another procedure
// to do the real work of handling the response.
//
//--------------------------------------------------------------------

int ResponseHandler::handleCmd(ClientData clientData, Tcl_Interp*,
										 int objc, Tcl_Obj *const objv[]) {
DOTRACE("ResponseHandler::handleCmd");
  // We assert that objc is 2 because this command should only ever
  // be invoked by a callback set up by ResponseHandler, which
  // should never call this with the wrong number of args.
  Assert(objc==2);
  ResponseHandler* rh = static_cast<ResponseHandler *>(clientData);
  return rh->handleResponse(Tcl_GetString(objv[1]));
}

//--------------------------------------------------------------------
//
// ResponseHandler::handleResponse --
//
// This procedure coordinates the various activities that must take
// place when a response occurs.
//
// Results:
// A usual Tcl result.
//
//--------------------------------------------------------------------

int ResponseHandler::handleResponse(const char* keysym) {
DOTRACE("ResponseHandler::handleResponse");
  itsExptDriver.tellResponseSeen();

  int result = ignore();
  if (result != TCL_OK) return result;

  int response = getRespFromKeysym(keysym);

  // If we had a valid response and itsUseFeedback is true, then give
  // the appropriate feedback for 'response'.
  if (response >= 0 && itsUseFeedback) {
    result = feedback(response);
    if (result != TCL_OK) return result;
  }

  itsExptDriver.processResponse(response);
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ResponseHandler::ignore --
// 
// When this procedure is invoked, the program ignores user-generated
// events (mouse or keyboard) that would otherwise signal a
// response. This effect is useful when the experiment is in between
// trials, when a current response is already being processed, and any
// other time when it is necessary to avoid an unintended
// key/button-press being interpreted as a response. The effect is
// cancelled by calling attend().
//
// Results:
// A usual Tcl result.
//
//--------------------------------------------------------------------

int ResponseHandler::ignore() const {
DOTRACE("ResponseHandler::ignore");
  static Tcl_Obj* unbindCmdObj = 
    Tcl_NewStringObj("bind .togl <KeyPress> {}", -1);
  static TclObjLock lock_(unbindCmdObj);
  return Tcl_EvalObjEx(itsInterp, unbindCmdObj, TCL_EVAL_GLOBAL);
}

//--------------------------------------------------------------------
//
// ResponseHandler::updateRegexps --
//
// Recompiles the internal table of regexps to correspond with the
// list of regexps and response values stored in the string
// itsKeyRespPairs.
//
// Results:
// Returns a usual Tcl result.
// 
//--------------------------------------------------------------------

int ResponseHandler::updateRegexps() {
DOTRACE("ResponseHandler::updateRegexps");

  // Get rid of any old stored regexps/response value pairs.
  itsRegexps.clear();


  // Make a Tcl_Obj out of the string itsKeyRespPairs, then extract
  // individual list elements out of this Tcl_Obj.
  Tcl_Obj* key_resp_pairsObj = 
    Tcl_NewStringObj(itsKeyRespPairs.c_str(), -1);

  Tcl_Obj** pairsObjs;
  int num_pairs=0;
  if ( Tcl_ListObjGetElements(itsInterp, key_resp_pairsObj, 
									  &num_pairs, &pairsObjs) != TCL_OK )
    return TCL_ERROR;


  // Loop over the regexp/response value pairs, and store in
  // itsRegexps a compiled Tcl_RegExp and integer response value for
  // each pair.
  for (int i = 0; i < num_pairs; ++i) {

	 // Get the i'th pair from the list of pairs
    Tcl_Obj* pairObj = pairsObjs[i];

    // Check that the length of the "pair" is really 2
    int length;
    if (Tcl_ListObjLength(itsInterp, pairObj, &length) != TCL_OK) 
		return TCL_ERROR;
    if (length != 2) return TCL_ERROR;

    // Get a Tcl_Obj from the first element of the pair, then get a
    // compiled regular expression from this Tcl_Obj.
    Tcl_Obj *regexpObj=NULL;
    if (Tcl_ListObjIndex(itsInterp, pairObj, 0, &regexpObj) != TCL_OK)
      return TCL_ERROR;
    Tcl_RegExp regexp = Tcl_GetRegExpFromObj(itsInterp, regexpObj, 0);
    if (!regexp) return TCL_ERROR;

	 // Get a Tcl_Obj from the second element of the pair; then get an
	 // integer from that Tcl_Obj.
	 Tcl_Obj *response_valObj=NULL;
    if (Tcl_ListObjIndex(itsInterp, pairObj, 1, &response_valObj) != TCL_OK)
      return TCL_ERROR;
    int response_val=-1;
    if (Tcl_GetIntFromObj(itsInterp, response_valObj, &response_val) != TCL_OK)
      return TCL_ERROR;
    
    // Push the pair onto the vector
    itsRegexps.push_back(RegExp_ResponseVal(regexp, response_val));
  }

  return TCL_OK;
}

static const char vcid_responsehandler_cc[] = "$Header$";
#endif // !RESPONSEHANDLER_CC_DEFINED
