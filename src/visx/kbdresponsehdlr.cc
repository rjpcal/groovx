///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:09:12 1999
// written: Mon Nov  8 19:20:26 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_CC_DEFINED
#define KBDRESPONSEHDLR_CC_DEFINED

#include "kbdresponsehdlr.h"

#include <tcl.h>
#include <vector>

#include "error.h"
#include "exptdriver.h"
#include "tclevalcmd.h"
#include "objtogl.h"
#include "toglconfig.h"
#include "sound.h"
#include "soundlist.h"
#include "reader.h"
#include "tclobjlock.h"
#include "writer.h"

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
// KbdResponseHdlr::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class KbdResponseHdlr::Impl {
public:
  Impl(KbdResponseHdlr* owner, const string& key_resp_pairs);
  ~Impl();

  // Delegand functions
  void serialize(ostream &os, IOFlag flag) const;
  void deserialize(istream &is, IOFlag flag);
  int charCount() const;

  void readFrom(Reader* reader);
  void writeTo(Writer* writer) const;

  void setInterp(Tcl_Interp* interp);

  const string& getKeyRespPairs() const
	 { return itsKeyRespPairs; }

  void setKeyRespPairs(const string& s)
	 {
		itsKeyRespPairs = s; 
		updateRegexps();
	 }

  bool getUseFeedback() const
	 { return itsUseFeedback; }

  void setUseFeedback(bool val)
	 { itsUseFeedback = val; }

  const char* getFeedbackPairs() const
	 { return itsFeedbackPairs.c_str(); }

  void setFeedbackPairs(const char* feedback_string)
	 {
		itsFeedbackPairs = feedback_string;
		updateFeedbacks();
	 }

  void rhBeginTrial() const
	 { attend(); }

  void rhAbortTrial() const;

  void rhHaltExpt() const
	 { ignore(); }

  void attend() const;
  void ignore() const;


  // Helper functions
private:
  void raiseBackgroundError(const char* msg) const;
  void raiseBackgroundError(const string& msg) const
	 { raiseBackgroundError(msg.c_str()); }

  static Tcl_ObjCmdProc handleCmd;
  int handleResponse(const char* keysym) const;
  int getRespFromKeysym(const char* keysym) const;
  int feedback(int response) const;
  void updateFeedbacks();
  void updateRegexps();

  int getCheckedListLength(Tcl_Obj* tcllist);

  Tcl_Obj* getCheckedListElement(Tcl_Obj* tcllist, int index);

  void checkedSplitList(Tcl_Obj* tcllist,
						 Tcl_Obj**& elements_out, int& length_out);

  Tcl_RegExp getCheckedRegexp(Tcl_Obj* patternObj);

  int getCheckedInt(Tcl_Obj* intObj);

  // data
private:
  KbdResponseHdlr* itsOwner;

  Tcl_Interp* itsInterp;

  struct RegExp_ResponseVal {
    RegExp_ResponseVal(Tcl_RegExp rx, int rv) : regexp(rx), resp_val(rv) {}
    Tcl_RegExp regexp;
    int resp_val;
  };

  string itsKeyRespPairs;
  mutable vector<RegExp_ResponseVal> itsRegexps;

  string itsFeedbackPairs;
  bool itsUseFeedback;

//    struct Condition_Feedback {
//    	 Condition_Feedback(Tcl_Obj* cond, Tcl_Obj* res);
//  	 Condition_Feedback(const Condition_Feedback& rhs);
//  	 Condition_Feedback& operator=(const Condition_Feedback& rhs);

//    	 TclObjPtr condition;
//    	 TclObjPtr result; 
//    };
  struct Condition_Feedback {
  	 Condition_Feedback(Tcl_Obj* cond, Tcl_Obj* res) :
  		condition(cond), result(res) {}		
  	 Tcl_Obj* condition;
  	 Tcl_Obj* result; 
  };

  mutable vector<Condition_Feedback> itsFeedbacks;
};

///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr::Impl method definitions
//
///////////////////////////////////////////////////////////////////////

KbdResponseHdlr::Impl::Impl(KbdResponseHdlr* owner,
									 const string& key_resp_pairs) :
  itsOwner(owner),
  itsInterp(0),
  itsKeyRespPairs(key_resp_pairs),
  itsRegexps(),
  itsFeedbackPairs(),
  itsUseFeedback(true),
  itsFeedbacks()
{
DOTRACE("KbdResponseHdlr::Impl::Impl");
}

KbdResponseHdlr::Impl::~Impl() {
DOTRACE("KbdResponseHdlr::Impl::~Impl");
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

  // Releases itsInterp for eventual destruction (itsInterp is
  // preserve'd in setInterp()).
  if (itsInterp != 0) {
	 Tcl_Release(itsInterp);
  }
}

void KbdResponseHdlr::Impl::serialize(ostream &os, IOFlag flag) const {
DOTRACE("KbdResponseHdlr::Impl::serialize");
  if (flag & BASES) { /* no bases to serialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsKeyRespPairs << endl;
  os << itsFeedbackPairs << endl;
  os << itsUseFeedback << endl;

  if (os.fail()) throw OutputError(ioTag);
}

void KbdResponseHdlr::Impl::deserialize(istream &is, IOFlag flag) {
DOTRACE("KbdResponseHdlr::Impl::deserialize");
  if (flag & BASES) { /* no bases to deserialize */ }
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  // XXX This is some sort of strange platform dependency..if the next
  // character in the stream is a space (the separator following the
  // typename), then we must pull it off the stream; however, in some
  // cases (using aCC/hp9000s700), the space is already gone by the
  // time we get here.
  DebugEvalNL(is.peek());
  if ( is.peek() == ' ' ) {
	 is.get();
  }

  getline(is, itsKeyRespPairs, '\n');
  updateRegexps();

  DebugEvalNL(is.peek());
  getline(is, itsFeedbackPairs, '\n');
  updateFeedbacks();

  int val;
  is >> val;
  itsUseFeedback = bool(val);

  if (is.fail()) throw InputError(ioTag);
}

int KbdResponseHdlr::Impl::charCount() const {
DOTRACE("KbdResponseHdlr::Impl::charCount");
  return (ioTag.length() + 1
			 + itsKeyRespPairs.length() + 1
			 + gCharCount<bool>(itsUseFeedback) + 1
			 + 1); // fudge factor
}

void KbdResponseHdlr::Impl::readFrom(Reader* reader) {
DOTRACE("KbdResponseHdlr::Impl::readFrom");

  reader->readValue("keyRespPairs", itsKeyRespPairs);
  reader->readValue("feedbackPairs", itsFeedbackPairs);
  reader->readValue("useFeedback", itsUseFeedback);

  updateRegexps();
  updateFeedbacks();
}

void KbdResponseHdlr::Impl::writeTo(Writer* writer) const {
DOTRACE("KbdResponseHdlr::Impl::writeTo");

  writer->writeValue("keyRespPairs", itsKeyRespPairs);
  writer->writeValue("feedbackPairs", itsFeedbackPairs);
  writer->writeValue("useFeedback", itsUseFeedback);
}

void KbdResponseHdlr::Impl::setInterp(Tcl_Interp* interp) {
DOTRACE("KbdResponseHdlr::Impl::setInterp");
  // can only set itsInterp once
  if (itsInterp == 0 && interp != 0) { 
	 itsInterp = interp;
	 Tcl_CreateObjCommand(itsInterp, "__KbdResponseHdlrPrivate::handle",
								 handleCmd, static_cast<ClientData>(this),
								 (Tcl_CmdDeleteProc *) NULL);
	 Tcl_Preserve(itsInterp);
  }
}

void KbdResponseHdlr::Impl::rhAbortTrial() const {
DOTRACE("KbdResponseHdlr::Impl::rhAbortTrial");
  Assert(itsInterp != 0);

  ignore();

  try {
	 const int ERR_INDEX = 1;
	 Sound* p = SoundList::theSoundList().getCheckedPtr(ERR_INDEX);
	 p->play();
  }
  catch (ErrorWithMsg& err) {
	 raiseBackgroundError(err.msg());
  }
}

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

void KbdResponseHdlr::Impl::attend() const {
DOTRACE("KbdResponseHdlr::Impl::attend");
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
	 raiseBackgroundError("error in KbdResponseHdlr::attend");
  }
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

void KbdResponseHdlr::Impl::ignore() const {
DOTRACE("KbdResponseHdlr::Impl::ignore");
  Assert(itsInterp != 0);

  try {
	 ObjTogl::theToglConfig()->bind("<KeyPress>", "{}");
  }
  catch (...) {
	 raiseBackgroundError("error in KbdResponseHdlr::ignore");
  }
}

///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr::Impl helper method definitions
//
///////////////////////////////////////////////////////////////////////

void KbdResponseHdlr::Impl::raiseBackgroundError(const char* msg) const {
DOTRACE("KbdResponseHdlr::Impl::raiseBackgroundError");
  Tcl_AppendResult(itsInterp, msg, (char*) 0);
  Tcl_BackgroundError(itsInterp);
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

int KbdResponseHdlr::Impl::handleCmd(ClientData clientData, Tcl_Interp*,
												 int objc, Tcl_Obj *const objv[]) {
DOTRACE("KbdResponseHdlr::Impl::handleCmd");
  // We assert that objc is 2 because this command should only ever
  // be invoked by a callback set up by KbdResponseHdlr, which
  // should never call this with the wrong number of args.
  Assert(objc==2);
  KbdResponseHdlr::Impl* impl =
	 static_cast<KbdResponseHdlr::Impl *>(clientData);
  return impl->handleResponse(Tcl_GetString(objv[1]));
}


int KbdResponseHdlr::Impl::handleResponse(const char* keysym) const {
DOTRACE("KbdResponseHdlr::Impl::handleResponse");
  exptDriver.edResponseSeen();

  itsOwner->ignore();

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

int KbdResponseHdlr::Impl::getRespFromKeysym(const char* keysym) const {
DOTRACE("KbdResponseHdlr::Impl::getRespFromKeysym");
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
		raiseBackgroundError("error executing regular expression "
									"for KbdResponseHdlr");
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
// KbdResponseHdlr::feedback --
//
// Call the user-defined feedback procedure with a given response
// value.
//
//--------------------------------------------------------------------

int KbdResponseHdlr::Impl::feedback(int response) const {
DOTRACE("KbdResponseHdlr::Impl::feedback");
  Assert(itsInterp != 0);

  DebugEvalNL(response);

  Tcl_SetVar2Ex(itsInterp, "resp_val", NULL,
					 Tcl_NewIntObj(response), TCL_GLOBAL_ONLY);

  for (int i = 0; i < itsFeedbacks.size(); ++i) {
	 int condition_result;
	 if (Tcl_ExprBooleanObj(itsInterp, itsFeedbacks[i].condition,
									&condition_result) != TCL_OK) {
		raiseBackgroundError("error evaluating boolean expression "
									"in KbdResponseHdlr::feedback");
		break;
	 }
	 
	 // If the condition was true...
	 if (condition_result != 0) {
		// Then evaluate the associated result script..,
		if (Tcl_EvalObjEx(itsInterp, itsFeedbacks[i].result,
								TCL_EVAL_GLOBAL) != TCL_OK) {
		  raiseBackgroundError("error evaluating feedback result script "
									  "in KbdResponseHdlr::feedback");
		}
		// ... and exit the loop since we only evaluate at most one
		// result script
		break;
	 }
  }

  Tcl_UnsetVar(itsInterp, "resp_val", 0);
  return TCL_OK;
}


//---------------------------------------------------------------------
//
// KbdResponseHdlr::updateFeedacks --
//
//---------------------------------------------------------------------

//  KbdResponseHdlr::Condition_Feedback::
//  Condition_Feedback(Tcl_Obj* cond, Tcl_Obj* res) :
//    condition(cond), result(res)
//  {
//    DOTRACE("Condition_Feedback(Tcl_Obj*, Tcl_Obj*)");
//    DebugEval(cond->refCount); DebugEvalNL(res->refCount);
//  }

//  KbdResponseHdlr::Condition_Feedback::
//  Condition_Feedback(const Condition_Feedback& rhs) :
//    condition(rhs.condition), result(rhs.result)
//  {
//    DOTRACE("Condition_Feedback(const Condition_Feedback&)");
//    Tcl_Obj* p = condition; DebugEval(p->refCount);
//    Tcl_Obj* q = result;    DebugEvalNL(q->refCount);
//  }

//  KbdResponseHdlr::Condition_Feedback&
//  KbdResponseHdlr::Condition_Feedback::
//  operator=(const Condition_Feedback& rhs)
//  {
//    DOTRACE("Condition_Feedback::operator=");
//    condition = rhs.condition; result = rhs.result; return *this;
//    Tcl_Obj* p = condition; DebugEval(p->refCount);
//    Tcl_Obj* q = result;    DebugEvalNL(q->refCount);
//  }

void KbdResponseHdlr::Impl::updateFeedbacks() {
DOTRACE("KbdResponseHdlr::Impl::updateFeedbacks");
  Assert(itsInterp != 0);

  itsFeedbacks.clear(); 

  try {
	 Tcl_Obj** pairs;
	 int num_pairs=0;
	 TclObjPtr pairs_list(Tcl_NewStringObj(itsFeedbackPairs.c_str(), -1));
	 checkedSplitList(pairs_list, pairs, num_pairs);

	 // Loop over the regexp/response value pairs
	 for (int i = 0; i < num_pairs; ++i) {

		Tcl_Obj* current_pair = pairs[i];

		// Check that the length of the "pair" is really 2
		int length = getCheckedListLength(current_pair);
		if (length != 2) {
		  raiseBackgroundError("\"pair\" did not have length 2 "
									  "in KbdResponseHdlr::updateFeedbacks");
		  return;
		}

		// First pair element
		Tcl_Obj *first_obj = getCheckedListElement(current_pair, 0);
		Tcl_IncrRefCount(first_obj);

		// Second pair element
		Tcl_Obj *second_obj = getCheckedListElement(current_pair, 1);
		Tcl_IncrRefCount(second_obj);
    
		itsFeedbacks.push_back(Impl::Condition_Feedback(first_obj, second_obj));
	 }

	 DebugPrintNL("updateFeedbacks success!");
  }
  catch (ErrorWithMsg& err) {
	 raiseBackgroundError(err.msg());
  }
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

void KbdResponseHdlr::Impl::updateRegexps() {
DOTRACE("KbdResponseHdlr::updateRegexps");
  Assert(itsInterp != 0);

  // Get rid of any old stored regexps/response value pairs.
  itsRegexps.clear();

  try {
	 Tcl_Obj** pairs;
	 int num_pairs=0;
	 TclObjPtr pairs_list(Tcl_NewStringObj(itsKeyRespPairs.c_str(), -1));
	 checkedSplitList(pairs_list, pairs, num_pairs);

	 // Loop over the regexp/response value pairs, and store in
	 // itsRegexps a compiled Tcl_RegExp and integer response value for
	 // each pair.
	 for (int i = 0; i < num_pairs; ++i) {

		TclObjPtr current_pair = pairs[i];

		// Check that the length of the "pair" is really 2
		int length = getCheckedListLength(current_pair);
		if (length != 2) {
		  raiseBackgroundError("\"pair\" did not have length 2 "
									  "in KbdResponseHdlr::updateRegexps");
		  return;
		}

		// First pair element
		Tcl_Obj* patternObj = getCheckedListElement(current_pair, 0);
		Tcl_RegExp regexp = getCheckedRegexp(patternObj);

		// Second pair element
		Tcl_Obj *response_valObj = getCheckedListElement(current_pair, 1);
		int response_val = getCheckedInt(response_valObj);
    
		itsRegexps.push_back(Impl::RegExp_ResponseVal(regexp, response_val));
	 }

	 DebugPrintNL("updateRegexps success!");
  }
  catch (ErrorWithMsg& err) {
	 raiseBackgroundError(err.msg());
  }
}

int KbdResponseHdlr::Impl::getCheckedListLength(Tcl_Obj* tcllist) {
DOTRACE("KbdResponseHdlr::Impl::getCheckedListLength");
  int length;
  if (Tcl_ListObjLength(itsInterp, tcllist, &length) != TCL_OK) {
	 throw ErrorWithMsg("error getting list length for KbdResponseHdlr");
  }
  return length;
}

Tcl_Obj* KbdResponseHdlr::Impl::getCheckedListElement(Tcl_Obj* tcllist, int index) {
DOTRACE("KbdResponseHdlr::Impl::getCheckedListElement");
  Tcl_Obj* element = NULL;
  if (Tcl_ListObjIndex(itsInterp, tcllist, index, &element) != TCL_OK) {
	 throw ErrorWithMsg("error getting list element for KbdResponseHdlr");
  }
  return element;
}

void KbdResponseHdlr::Impl::checkedSplitList(Tcl_Obj* tcllist,
							 Tcl_Obj**& elements_out, int& length_out) {
DOTRACE("KbdResponseHdlr::Impl::checkedSplitList");
  if ( Tcl_ListObjGetElements(itsInterp, tcllist, 
										&length_out, &elements_out) != TCL_OK ) {
	 throw ErrorWithMsg("error splitting list for KbdResponseHdlr");
  }
}

Tcl_RegExp KbdResponseHdlr::Impl::getCheckedRegexp(Tcl_Obj* patternObj) {
DOTRACE("KbdResponseHdlr::Impl::getCheckedRegexp");
  const int flags = 0;
  Tcl_RegExp regexp = Tcl_GetRegExpFromObj(itsInterp, patternObj, flags);
  if (!regexp) {
	 throw ErrorWithMsg("error creating a Tcl_RegExp for KbdResponseHdlr");
  }
  return regexp;
}

int KbdResponseHdlr::Impl::getCheckedInt(Tcl_Obj* intObj) {
DOTRACE("KbdResponseHdlr::Impl::getCheckedInt");
  int return_val=-1;
  if (Tcl_GetIntFromObj(itsInterp, intObj, &return_val) != TCL_OK) {
	 throw ErrorWithMsg("error getting int from Tcl_Obj for KbdResponseHdlr");
  }
  return return_val;
}


///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr method definitions
//
// All of these methods just delegate to KbdResponseHdlr::Impl methods
//
///////////////////////////////////////////////////////////////////////

KbdResponseHdlr::KbdResponseHdlr(const string& key_resp_pairs) : 
  ResponseHandler(),
  itsImpl(new Impl(this, key_resp_pairs))
{
DOTRACE("KbdResponseHdlr::KbdResponseHdlr");
}

KbdResponseHdlr::~KbdResponseHdlr() {
DOTRACE("KbdResponseHdlr::~KbdResponseHdlr");
  delete itsImpl;
}

void KbdResponseHdlr::serialize(ostream &os, IOFlag flag) const {
  itsImpl->serialize(os, flag);
}

void KbdResponseHdlr::deserialize(istream &is, IOFlag flag) {
  itsImpl->deserialize(is, flag);
}

int KbdResponseHdlr::charCount() const {
  return itsImpl->charCount();
}

void KbdResponseHdlr::readFrom(Reader* reader) {
  itsImpl->readFrom(reader);
}

void KbdResponseHdlr::writeTo(Writer* writer) const {
  itsImpl->writeTo(writer);
}

void KbdResponseHdlr::setInterp(Tcl_Interp* interp) {
  itsImpl->setInterp(interp);
}

const string& KbdResponseHdlr::getKeyRespPairs() const {
DOTRACE("KbdResponseHdlr::getKeyRespPairs");
  return itsImpl->getKeyRespPairs();
}

void KbdResponseHdlr::setKeyRespPairs(const string& s) {
DOTRACE("KbdResponseHdlr::setKeyRespPairs");
  itsImpl->setKeyRespPairs(s);
}

bool KbdResponseHdlr::getUseFeedback() const {
DOTRACE("KbdResponseHdlr::getUseFeedback");
  return itsImpl->getUseFeedback();
}

void KbdResponseHdlr::setUseFeedback(bool val) {
DOTRACE("KbdResponseHdlr::setUseFeedback");
  itsImpl->setUseFeedback(val);
}

const char* KbdResponseHdlr::getFeedbackPairs() const {
DOTRACE("KbdResponseHdlr::getFeedbackPairs");
  return itsImpl->getFeedbackPairs();
}

void KbdResponseHdlr::setFeedbackPairs(const char* feedback_string) {
DOTRACE("KbdResponseHdlr::setFeedbackPairs");
  itsImpl->setFeedbackPairs(feedback_string);
}

void KbdResponseHdlr::rhBeginTrial() const {
DOTRACE("KbdResponseHdlr::rhBeginTrial");
  itsImpl->attend();
}

void KbdResponseHdlr::rhAbortTrial() const {
  itsImpl->rhAbortTrial();
}

void KbdResponseHdlr::rhHaltExpt() const {
DOTRACE("KbdResponseHdlr::rhHaltExpt");
  itsImpl->rhHaltExpt();
}

void KbdResponseHdlr::attend() const {
  itsImpl->attend();
}

void KbdResponseHdlr::ignore() const {
  itsImpl->ignore();
}

static const char vcid_kbdresponsehdlr_cc[] = "$Header$";
#endif // !KBDRESPONSEHDLR_CC_DEFINED
