///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:09:12 1999
// written: Tue Jul 20 14:09:15 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_CC_DEFINED
#define KBDRESPONSEHDLR_CC_DEFINED

#include "kbdresponsehdlr.h"

#include "exptdriver.h"
#include "tclevalcmd.h"
#include "objtogl.h"
#include "toglconfig.h"
#include "sound.h"
#include "soundlist.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

namespace {
  ExptDriver& exptDriver = ExptDriver::theExptDriver();

  const string ioTag = "KbdResponseHdlr";
}

///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr method definitions
//
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
//
// KbdResponseHdlr::KbdResponseHdlr --
//
// This constructor preserves itsInterp so that the interp will not be
// destroyed at least until this KbdResponseHdlr has Tcl_Release'd it.
//
//--------------------------------------------------------------------

KbdResponseHdlr::KbdResponseHdlr(const string& s) : 
  ResponseHandler(),
  itsKeyRespPairs(s),
  itsRegexps(),
  itsUseFeedback(true),
  itsRawResponse(NULL)
{
DOTRACE("KbdResponseHdlr::KbdResponseHdlr");
}

//--------------------------------------------------------------------
//
// KbdResponseHdlr::~KbdResponseHdlr --
//
// The destructor releases itsInterp for eventual destruction.
//
//--------------------------------------------------------------------

KbdResponseHdlr::~KbdResponseHdlr() {
DOTRACE("KbdResponseHdlr::~KbdResponseHdlr");
  // We must check if itsInterp has been tagged for deletion already,
  // since if it is then we must not attempt to use it to delete a Tcl
  // command (this results in "called Tcl_HashEntry on deleted
  // table"). Not deleting the command in that case does not cause a
  // resource leak, however, since the Tcl_Interp as part if its own
  // destruction will delete all commands associated with it.

  if ( !Tcl_InterpDeleted(itsInterp) ) {
	 DebugPrintNL("deleting Tcl command __KbdResponseHdlrPrivate::handle");

	 Tcl_DeleteCommand(itsInterp, "__KbdResponseHdlrPrivate::handle");
  }

  if (itsInterp != 0) {
	 Tcl_Release(itsInterp);
  }
}

void KbdResponseHdlr::serialize(ostream &os, IOFlag flag) const {
DOTRACE("KbdResponseHdlr::serialize");
  if (flag & BASES) { /* no bases to serialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsKeyRespPairs << endl;
  os << itsUseFeedback << endl;

  if (os.fail()) throw OutputError(ioTag);
}

void KbdResponseHdlr::deserialize(istream &is, IOFlag flag) {
DOTRACE("KbdResponseHdlr::deserialize");
  if (flag & BASES) { /* no bases to deserialize */ }
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  getline(is, itsKeyRespPairs, '\n');

  int val;
  is >> val;
  itsUseFeedback = bool(val);

  if (is.fail()) throw InputError(ioTag);
}

int KbdResponseHdlr::charCount() const {
DOTRACE("KbdResponseHdlr::charCount");
  return (ioTag.length() + 1
			 + itsKeyRespPairs.length() + 1
			 + gCharCount<bool>(itsUseFeedback) + 1
			 + 1); // fudge factor
}

void KbdResponseHdlr::setInterp(Tcl_Interp* interp) {
DOTRACE("KbdResponseHdlr::setInterp");
  // can only set itsInterp once
  if (itsInterp == 0 && interp != 0) { 
	 itsInterp = interp;
	 Tcl_CreateObjCommand(itsInterp, "__KbdResponseHdlrPrivate::handle",
								 handleCmd, static_cast<ClientData>(this),
								 (Tcl_CmdDeleteProc *) NULL);
	 Tcl_Preserve(itsInterp);
  }
}

const char* KbdResponseHdlr::getFeedbackPairs() const {
DOTRACE("KbdResponseHdlr::getFeedbackPairs");
  return itsFeedbackPairs.c_str(); 
}

void KbdResponseHdlr::setFeedbackPairs(const char* feedback_string) {
DOTRACE("KbdResponseHdlr::setFeedbackPairs");
  itsFeedbackPairs = feedback_string;
  updateFeedbacks();
}

void KbdResponseHdlr::rhBeginTrial() const {
DOTRACE("KbdResponseHdlr::rhBeginTrial");
  attend();
}

void KbdResponseHdlr::rhAbortTrial() const {
DOTRACE("KbdResponseHdlr::rhAbortTrial");
  Assert(itsInterp != 0);

  ignore();

  try {
	 const int ERR_INDEX = 1;
	 Sound* p = SoundList::theSoundList().getCheckedPtr(ERR_INDEX);
	 p->play();
  }
  catch (ErrorWithMsg& err) {
	 Tcl_BackgroundError(itsInterp);
	 Tcl_AddObjErrorInfo(itsInterp, err.msg().c_str(), -1);
  }
}

void KbdResponseHdlr::rhHaltExpt() const {
DOTRACE("KbdResponseHdlr::rhHaltExpt");
  ignore();
}

/*
  Here's a picture of the usual call-graph when a response occurs:

  * attend() creates event binding
  * response event occurs and triggers the binding
  
  1) response occurs
 */

//--------------------------------------------------------------------
//
// KbdResponseHdlr::attend --
//
// When this procedure is invoked, the program listens to the events
// that are pertinent to the response policy. This procedure should be
// called when a trial is begun, and should be cancelled with ignore()
// when a response has been made so that events are not received
// during response processing and during the inter-trial period.
//
//--------------------------------------------------------------------

void KbdResponseHdlr::attend() const {
DOTRACE("KbdResponseHdlr::attend");
  Assert(itsInterp != 0);

  // Clear the event queue before we rebind to input events
  while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0) {
	 ; // Empty loop body
  }
  
  try {
	 ObjTogl::theToglConfig()->bind("<KeyPress>",
											  "{ __KbdResponseHdlrPrivate::handle %K }");
  }
  catch (...) {
	 Tcl_BackgroundError(itsInterp);
  }
}

//--------------------------------------------------------------------
//
// KbdResponseHdlr::feedback --
//
// Call the user-defined feedback procedure with a given response
// value.
//
//--------------------------------------------------------------------

int KbdResponseHdlr::feedback(int response) const {
DOTRACE("KbdResponseHdlr::feedback");
  Assert(itsInterp != 0);

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
// KbdResponseHdlr::getRespFromKeysysm --
//
// This procedure looks up a character string in the KbdResponseHdlrs
// regexp table.
//
// Results: 
// Returns the response value associated with the first regexp in
// itsRegexps that matches the given character string, or returns -1
// if none of the regexps give a match.
//
//--------------------------------------------------------------------

int KbdResponseHdlr::getRespFromKeysym(const char* keysym) const {
DOTRACE("KbdResponseHdlr::getRespFromKeysym");
  Assert(itsInterp != 0);

  // Get exactly one character -- the last character -- in keysym
  // (just before the terminating '\0'). This is the character that we
  // will take as the response. (The last character must be extracted
  // to handle the numeric keypad, where the keysysms are 'KP_0',
  // 'KP_3', etc., and we want just the 0 or the 3).
  const char* last_char = keysym;
  while ( *(last_char+1) != '\0' ) { ++last_char; }
    
  int resp = ResponseHandler::INVALID_RESPONSE;

  for (int i = 0; i < itsRegexps.size(); ++i) {
    int found = Tcl_RegExpExec(itsInterp, itsRegexps[i].regexp, 
                               last_char, last_char);

	 // Return value of -1 indicates error while executing in the
	 // regular expression, so generate a background error
    if (found == -1) {
		Tcl_BackgroundError(itsInterp);
		return ResponseHandler::INVALID_RESPONSE;
	 }

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
// KbdResponseHdlr::handleCmd --
//
// This Tcl command procedure is intended to be private to the
// KbdResponseHdlr. Its raison d'etre is to give a hook back into the
// C++ code from Tcl from the event binding that is set up in
// KbdResponseHdlr::attend(). It passes control to another procedure
// to do the real work of handling the response.
//
//--------------------------------------------------------------------

int KbdResponseHdlr::handleCmd(ClientData clientData, Tcl_Interp*,
										 int objc, Tcl_Obj *const objv[]) {
DOTRACE("KbdResponseHdlr::handleCmd");
  // We assert that objc is 2 because this command should only ever
  // be invoked by a callback set up by KbdResponseHdlr, which
  // should never call this with the wrong number of args.
  Assert(objc==2);
  KbdResponseHdlr* rh = static_cast<KbdResponseHdlr *>(clientData);
  return rh->handleResponse(Tcl_GetString(objv[1]));
}

//--------------------------------------------------------------------
//
// KbdResponseHdlr::handleResponse --
//
// This procedure coordinates the various activities that must take
// place when a response occurs.
//
// Results:
// A usual Tcl result.
//
//--------------------------------------------------------------------

int KbdResponseHdlr::handleResponse(const char* keysym) const {
DOTRACE("KbdResponseHdlr::handleResponse");
  exptDriver.edResponseSeen();

  ignore();

  int response = getRespFromKeysym(keysym);

  // If we had an invalid response, instruct the exptDriver to abort
  // the trial
  if (response == ResponseHandler::INVALID_RESPONSE) {
	 exptDriver.edAbortTrial();
	 return TCL_OK;
  }
  // ... otherwise if itsUseFeedback is true, then give the
  // appropriate feedback for 'response'.
  else if (itsUseFeedback) {
    int result = feedback(response);
    if (result != TCL_OK) return result;
  }

  exptDriver.edProcessResponse(response);
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// KbdResponseHdlr::ignore --
// 
// When this procedure is invoked, the program ignores user-generated
// events (mouse or keyboard) that would otherwise signal a
// response. This effect is useful when the experiment is in between
// trials, when a current response is already being processed, and any
// other time when it is necessary to avoid an unintended
// key/button-press being interpreted as a response. The effect is
// cancelled by calling attend().
//
//--------------------------------------------------------------------

void KbdResponseHdlr::ignore() const {
DOTRACE("KbdResponseHdlr::ignore");
  Assert(itsInterp != 0);

  try {
	 ObjTogl::theToglConfig()->bind("<KeyPress>", "{}");
  }
  catch (...) {
	 Tcl_BackgroundError(itsInterp);
  }
}

//---------------------------------------------------------------------
//
// KbdResponseHdlr::updateFeedacks --
//
//---------------------------------------------------------------------

void KbdResponseHdlr::updateFeedbacks() {
DOTRACE("KbdResponseHdlr::updateFeedbacks");
  Assert(itsInterp != 0);

  itsFeedbacks.clear(); 

  TclObjPtr feedback_pairs_obj(Tcl_NewStringObj(itsFeedbackPairs.c_str(), -1));

  Tcl_Obj** pairs_objs;
  int num_pairs=0;
  if ( Tcl_ListObjGetElements(itsInterp, feedback_pairs_obj, 
										&num_pairs, &pairs_objs) != TCL_OK ) {
	 Tcl_BackgroundError(itsInterp);
    return;
  }

  // Loop over the regexp/response value pairs
  for (int i = 0; i < num_pairs; ++i) {

	 // Get the i'th pair from the list of pairs
    Tcl_Obj* pair_obj = pairs_objs[i];

    // Check that the length of the "pair" is really 2
    int length;
    if (Tcl_ListObjLength(itsInterp, pair_obj, &length) != TCL_OK) {
		Tcl_BackgroundError(itsInterp);
		return;
	 }
    if (length != 2) {
		Tcl_BackgroundError(itsInterp);
		return;
	 }

    // Get a Tcl_Obj from the first element of the pair.
    Tcl_Obj *first_obj=NULL;
    if (Tcl_ListObjIndex(itsInterp, pair_obj, 0, &first_obj) != TCL_OK) {
      Tcl_BackgroundError(itsInterp);
		return;
	 }

	 // Get a Tcl_Obj from the second element of the pair; then get an
	 // integer from that Tcl_Obj.
	 Tcl_Obj *second_obj=NULL;
    if (Tcl_ListObjIndex(itsInterp, pair_obj, 1, &second_obj) != TCL_OK) {
      Tcl_BackgroundError(itsInterp);
		return;
	 }
    
    // Push the pair onto the vector
    itsFeedbacks.push_back(Condition_Feedback(first_obj, second_obj));
  }

  DebugPrintNL("updateFeedbacks success!");
}

//--------------------------------------------------------------------
//
// KbdResponseHdlr::updateRegexps --
//
// Recompiles the internal table of regexps to correspond with the
// list of regexps and response values stored in the string
// itsKeyRespPairs.
//
//--------------------------------------------------------------------

void KbdResponseHdlr::updateRegexps() {
DOTRACE("KbdResponseHdlr::updateRegexps");
  Assert(itsInterp != 0);

  // Get rid of any old stored regexps/response value pairs.
  itsRegexps.clear();

  DebugEvalNL(itsKeyRespPairs);

  // Make a Tcl_Obj out of the string itsKeyRespPairs, then extract
  // individual list elements out of this Tcl_Obj.
  Tcl_Obj* key_resp_pairsObj = 
    Tcl_NewStringObj(itsKeyRespPairs.c_str(), -1);

  Tcl_Obj** pairsObjs;
  int num_pairs=0;
  if ( Tcl_ListObjGetElements(itsInterp, key_resp_pairsObj, 
										&num_pairs, &pairsObjs) != TCL_OK ) {
	 Tcl_BackgroundError(itsInterp);
    return;
  }

  // Loop over the regexp/response value pairs, and store in
  // itsRegexps a compiled Tcl_RegExp and integer response value for
  // each pair.
  for (int i = 0; i < num_pairs; ++i) {

	 // Get the i'th pair from the list of pairs
    Tcl_Obj* pairObj = pairsObjs[i];

    // Check that the length of the "pair" is really 2
    int length;
    if (Tcl_ListObjLength(itsInterp, pairObj, &length) != TCL_OK) {
		Tcl_BackgroundError(itsInterp);
		return;
	 }
    if (length != 2) {
		Tcl_BackgroundError(itsInterp);
		return;
	 }

    // Get a Tcl_Obj from the first element of the pair, then get a
    // compiled regular expression from this Tcl_Obj.
    Tcl_Obj *regexpObj=NULL;
    if (Tcl_ListObjIndex(itsInterp, pairObj, 0, &regexpObj) != TCL_OK) {
      Tcl_BackgroundError(itsInterp);
		return;
	 }
    Tcl_RegExp regexp = Tcl_GetRegExpFromObj(itsInterp, regexpObj, 0);
    if (!regexp) {
		Tcl_BackgroundError(itsInterp);
		return;
	 }

	 // Get a Tcl_Obj from the second element of the pair; then get an
	 // integer from that Tcl_Obj.
	 Tcl_Obj *response_valObj=NULL;
    if (Tcl_ListObjIndex(itsInterp, pairObj, 1, &response_valObj) != TCL_OK) {
      Tcl_BackgroundError(itsInterp);
		return;
	 }
    int response_val=-1;
    if (Tcl_GetIntFromObj(itsInterp, response_valObj, &response_val)
		  != TCL_OK) {
      Tcl_BackgroundError(itsInterp);
		return;
	 }
    
    // Push the pair onto the vector
    itsRegexps.push_back(RegExp_ResponseVal(regexp, response_val));
  }

  DebugPrintNL("updateRegexps success!");
}

static const char vcid_kbdresponsehdlr_cc[] = "$Header$";
#endif // !KBDRESPONSEHDLR_CC_DEFINED
