///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:09:12 1999
// written: Tue Nov  9 15:20:32 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_CC_DEFINED
#define KBDRESPONSEHDLR_CC_DEFINED

#include "kbdresponsehdlr.h"

#include <tcl.h>
#include <strstream.h>
#include <vector>

#include "error.h"
#include "exptdriver.h"
#include "objtogl.h"
#include "sound.h"
#include "soundlist.h"
#include "reader.h"
#include "tclevalcmd.h"
#include "tclobjlock.h"
#include "toglconfig.h"
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

  const string nullScript = "{}";

  const string eventSequence = "<KeyPress>";

  const string bindingSubstitution = "%K";

  unsigned long cmdCounter = 0;
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

  // Helper functions
private:
  // When this procedure is invoked, the program listens to the events
  // that are pertinent to the response policy. This procedure should
  // be called when a trial is begun, and should be cancelled with
  // ignore() when a response has been made so that events are not
  // received during response processing and during the inter-trial
  // period.
  void attend() const;

  // When this procedure is invoked, the program ignores
  // user-generated events (mouse or keyboard) that would otherwise
  // signal a response. This effect is useful when the experiment is
  // in between trials, when a current response is already being
  // processed, and any other time when it is necessary to avoid an
  // unintended key/button-press being interpreted as a response. The
  // effect is cancelled by calling attend().
  void ignore() const;


  void raiseBackgroundError(const char* msg) const throw();
  void raiseBackgroundError(const string& msg) const throw()
	 { raiseBackgroundError(msg.c_str()); }

  static Tcl_ObjCmdProc privateHandleCmd;
  void handleResponse(const char* keysym) const;
  int getRespFromKeysym(const char* keysym) const;
  void feedback(int response) const;
  void updateFeedbacks();
  void updateRegexps();

  int getCheckedListLength(Tcl_Obj* tcllist) const throw(ErrorWithMsg);

  Tcl_Obj* getCheckedListElement(
    Tcl_Obj* tcllist, int index) const throw(ErrorWithMsg);

  void checkedSplitList(Tcl_Obj* tcllist,
	 Tcl_Obj**& elements_out, int& length_out) const throw(ErrorWithMsg);

  Tcl_RegExp getCheckedRegexp(Tcl_Obj* patternObj) const throw(ErrorWithMsg);

  int getCheckedInt(Tcl_Obj* intObj) const throw(ErrorWithMsg);

  string getBindingScript() const
	 { return string("{ " + itsPrivateCmdName + " " +
						  bindingSubstitution + " }"); }

  void clearEventQueue() const throw()
	 {
		while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0)
		  { /* Empty loop body */ }
	 }

  bool isValidResponse(int response) const throw()
	 { return (response != ResponseHandler::INVALID_RESPONSE); }

  // We take the last character of the string as the response, in
  // order to handle the numeric keypad, where the keysysms are
  // 'KP_0', 'KP_3', etc., wheras we want just the 0 or the 3.
  const char* extractStringFromKeysym(const char* text) const
	 {
		while ( *(++text) != '\0' )
		  { ; }
		return (text-1);
	 }

  string getUniqueCmdName()
	 {
		const string privateHandleCmdName = "__KbdResponseHdlrPrivate::handle";

		vector<char> buf(privateHandleCmdName.length() + 32);
		ostrstream ost(&buf[0], privateHandleCmdName.length() + 32);
		ost << privateHandleCmdName << ++cmdCounter << '\0';		
		return string(&buf[0]);
	 }

  // data
private:
  KbdResponseHdlr* itsOwner;

  Tcl_Interp* itsInterp;

  mutable Tcl_Command itsTclCmdToken;
  const string itsPrivateCmdName;

  class RegExp_ResponseVal {
  public:
    RegExp_ResponseVal(Tcl_RegExp rx, int rv) : itsRegexp(rx), itsRespVal(rv) {}

	 bool matchesString(Tcl_Interp* interp,
							  const char* string_to_match) throw(ErrorWithMsg)
		{
		  static const int REGEX_ERROR        = -1;
		  static const int REGEX_NO_MATCH     =  0;
		  static const int REGEX_FOUND_MATCH  =  1;

		  int regex_result =
			 Tcl_RegExpExec(interp, itsRegexp, string_to_match, string_to_match);

		  switch (regex_result) {
		  case REGEX_ERROR:
			 throw ErrorWithMsg("error executing regular expression "
									  "for KbdResponseHdlr");

		  case REGEX_NO_MATCH:
			 return false;

		  case REGEX_FOUND_MATCH:
			 return true;
		
		  default: // "can't happen"
			 Assert(false);
		  }
		}

	 int responseValue() { return itsRespVal; }

  private:
    Tcl_RegExp itsRegexp;
    int itsRespVal;
  };

  string itsKeyRespPairs;
  mutable vector<RegExp_ResponseVal> itsRegexps;

  class Condition_Feedback {
  public:
  	 Condition_Feedback(Tcl_Obj* cond, Tcl_Obj* res) :
  		itsCondition(cond),
 		itsResultCmd(res, TclEvalCmd::THROW_EXCEPTION, TCL_EVAL_GLOBAL)
		{}

	 bool isTrue(Tcl_Interp* interp) throw(ErrorWithMsg)
		{
		  int expr_result;
		  if (Tcl_ExprBooleanObj(interp, itsCondition, &expr_result) != TCL_OK) {
			 throw ErrorWithMsg("error evaluating boolean expression "
									  "for KbdResponseHdlr");
		  }
		  return bool(expr_result);
		}

	 void invoke(Tcl_Interp* interp) throw(ErrorWithMsg)
		{ itsResultCmd.invoke(interp); }

	 bool invokeIfTrue(Tcl_Interp* interp) throw (ErrorWithMsg)
		{
		  if (isTrue(interp)) 
			 { invoke(interp); return true; }
		  return false;
		}

  private:
	 TclObjPtr itsCondition;
 	 TclEvalCmd itsResultCmd;
  };

  string itsFeedbackPairs;
  mutable vector<Condition_Feedback> itsFeedbacks;

  bool itsUseFeedback;
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
  itsTclCmdToken(0),
  itsPrivateCmdName(getUniqueCmdName()),
  itsKeyRespPairs(key_resp_pairs),
  itsRegexps(),
  itsFeedbackPairs(),
  itsFeedbacks(),
  itsUseFeedback(true)
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
	 ignore();
 
	 DebugPrintNL("deleting Tcl command " + itsPrivateCmdName);

	 DebugEvalNL((void*) itsTclCmdToken);
	 Tcl_DeleteCommandFromToken(itsInterp, itsTclCmdToken);
  }

  // (The matching Tcl_Preserve() call is in setInterp(), rather than in
  // the constructor).
  if (itsInterp != 0) {
	 Tcl_Release(itsInterp);
  }
}

void KbdResponseHdlr::Impl::serialize(ostream &os, IOFlag flag) const {
DOTRACE("KbdResponseHdlr::Impl::serialize");

  if (flag & TYPENAME) { os << ioTag << IO::SEP; }

  os << itsKeyRespPairs << endl;
  os << itsFeedbackPairs << endl;
  os << itsUseFeedback << endl;

  if (flag & BASES) { /* no bases to serialize */ }

  if (os.fail()) throw OutputError(ioTag);
}

void KbdResponseHdlr::Impl::deserialize(istream &is, IOFlag flag) {
DOTRACE("KbdResponseHdlr::Impl::deserialize");

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

  if (flag & BASES) { /* no bases to deserialize */ }

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
	 itsTclCmdToken = 
		Tcl_CreateObjCommand(itsInterp,
									const_cast<char*>(itsPrivateCmdName.c_str()),
									privateHandleCmd, static_cast<ClientData>(this),
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

///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr::Impl helper method definitions
//
///////////////////////////////////////////////////////////////////////

void KbdResponseHdlr::Impl::attend() const {
DOTRACE("KbdResponseHdlr::Impl::attend");
  clearEventQueue();
  
  try {
	 if (ObjTogl::toglHasBeenCreated()) {
		ObjTogl::theToglConfig()->bind(eventSequence.c_str(),
												 getBindingScript().c_str());
	 }
  }
  catch (ErrorWithMsg& err) {
	 raiseBackgroundError(err.msg());
  }
  catch (...) {
	 raiseBackgroundError("error creating event binding "
								 "in KbdResponseHdlr::attend");
  }
}

void KbdResponseHdlr::Impl::ignore() const {
DOTRACE("KbdResponseHdlr::Impl::ignore");
  try {
	 if (ObjTogl::toglHasBeenCreated()) {
		ObjTogl::theToglConfig()->bind(eventSequence.c_str(),
												 nullScript.c_str());
	 }
  }
  catch (ErrorWithMsg& err) {
	 raiseBackgroundError(err.msg());
  }
  catch (...) {
	 raiseBackgroundError("error creating event binding "
								 "in KbdResponseHdlr::ignore");
  }
}

void KbdResponseHdlr::Impl::raiseBackgroundError(const char* msg) const throw() {
DOTRACE("KbdResponseHdlr::Impl::raiseBackgroundError"); 
  DebugEvalNL(msg); 
  Assert(itsInterp != 0);
  Tcl_AppendResult(itsInterp, msg, (char*) 0);
  Tcl_BackgroundError(itsInterp);
}

//--------------------------------------------------------------------
//
// KbdResponseHdlr::privateHandleCmd --
//
// This Tcl command procedure gives a hook back into the C++ code from
// Tcl from the event binding that is set up in attend().
//
//--------------------------------------------------------------------

int KbdResponseHdlr::Impl::privateHandleCmd(
  ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]
  ) {
DOTRACE("KbdResponseHdlr::Impl::privateHandleCmd");
  // We assert that objc is 2 because this command should only ever
  // be invoked by a callback set up by KbdResponseHdlr, which
  // should never call this with the wrong number of args.
  Assert(objc==2);

  KbdResponseHdlr::Impl* impl =
	 static_cast<KbdResponseHdlr::Impl *>(clientData);

  try {
	 impl->handleResponse(Tcl_GetString(objv[1]));
  }
  catch (ErrorWithMsg& err) {
	 Tcl_AppendResult(interp, err.msg().c_str(), (char*) 0);
	 return TCL_ERROR;
  }
  catch (...) {
	 Tcl_AppendResult(interp, "an error of unknown type occurred", (char*) 0);
	 return TCL_ERROR;
  }

  return TCL_OK;
}


void KbdResponseHdlr::Impl::handleResponse(const char* keysym) const {
DOTRACE("KbdResponseHdlr::Impl::handleResponse");

  exptDriver.edResponseSeen();

  ignore();

  int response = getRespFromKeysym(keysym);

  if ( !isValidResponse(response) ) {
	 exptDriver.edAbortTrial();
  }

  else {
	 exptDriver.edProcessResponse(response);
	 if (itsUseFeedback) { feedback(response); }
  }
}

//--------------------------------------------------------------------
//
// KbdResponseHdlr::getRespFromKeysysm --
//
// This procedure returns the response value associated with the first
// regexp in itsRegexps that matches the given character string, or
// returns INVALID_RESPONSE if none of the regexps give a match.
//
//--------------------------------------------------------------------

int KbdResponseHdlr::Impl::getRespFromKeysym(const char* keysym) const {
DOTRACE("KbdResponseHdlr::Impl::getRespFromKeysym");
  Assert(itsInterp != 0);

  const char* response_string = extractStringFromKeysym(keysym);
    
  for (int i = 0; i < itsRegexps.size(); ++i) {
	 if (itsRegexps[i].matchesString(itsInterp, response_string)) {
		return itsRegexps[i].responseValue();
	 }
  }

  return ResponseHandler::INVALID_RESPONSE;
}

void KbdResponseHdlr::Impl::feedback(int response) const {
DOTRACE("KbdResponseHdlr::Impl::feedback");
  Assert(itsInterp != 0);

  DebugEvalNL(response);

  Tcl_SetVar2Ex(itsInterp, "resp_val", NULL,
					 Tcl_NewIntObj(response), TCL_GLOBAL_ONLY);

  bool feedbackGiven = false;
  for (int i = 0; i<itsFeedbacks.size() && !feedbackGiven; ++i) {
	 feedbackGiven = itsFeedbacks[i].invokeIfTrue(itsInterp);
  }

  Tcl_UnsetVar(itsInterp, "resp_val", 0);
}


//---------------------------------------------------------------------
//
// KbdResponseHdlr::updateFeedacks --
//
//---------------------------------------------------------------------

void KbdResponseHdlr::Impl::updateFeedbacks() {
DOTRACE("KbdResponseHdlr::Impl::updateFeedbacks");
  Assert(itsInterp != 0);

  itsFeedbacks.clear(); 

  try {
	 Tcl_Obj** pairs;
	 int num_pairs=0;
	 TclObjPtr pairs_list(Tcl_NewStringObj(itsFeedbackPairs.c_str(), -1));
	 checkedSplitList(pairs_list, pairs, num_pairs);

	 for (int i = 0; i < num_pairs; ++i) {

		TclObjPtr current_pair = pairs[i];

		// Check that the length of the "pair" is really 2
		if (getCheckedListLength(current_pair) != 2) {
		  raiseBackgroundError("\"pair\" did not have length 2 "
									  "in KbdResponseHdlr::updateFeedbacks");
		  return;
		}

		Tcl_Obj *condition = getCheckedListElement(current_pair, 0);
		Tcl_Obj *result = getCheckedListElement(current_pair, 1);
    
		itsFeedbacks.push_back(Impl::Condition_Feedback(condition, result));
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

	 for (int i = 0; i < num_pairs; ++i) {

		TclObjPtr current_pair = pairs[i];

		// Check that the length of the "pair" is really 2
		if (getCheckedListLength(current_pair) != 2) {
		  raiseBackgroundError("\"pair\" did not have length 2 "
									  "in KbdResponseHdlr::updateRegexps");
		  return;
		}

		Tcl_Obj* patternObj = getCheckedListElement(current_pair, 0);
		Tcl_RegExp regexp = getCheckedRegexp(patternObj);

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

int KbdResponseHdlr::Impl::getCheckedListLength(
  Tcl_Obj* tcllist
) const throw(ErrorWithMsg) {
DOTRACE("KbdResponseHdlr::Impl::getCheckedListLength");
  int length;
  if (Tcl_ListObjLength(itsInterp, tcllist, &length) != TCL_OK) {
	 throw ErrorWithMsg("error getting list length for KbdResponseHdlr");
  }
  return length;
}

Tcl_Obj* KbdResponseHdlr::Impl::getCheckedListElement(
  Tcl_Obj* tcllist, int index
) const throw(ErrorWithMsg) {
DOTRACE("KbdResponseHdlr::Impl::getCheckedListElement");

  Tcl_Obj* element = NULL;
  if (Tcl_ListObjIndex(itsInterp, tcllist, index, &element) != TCL_OK) {
	 throw ErrorWithMsg("error getting list element for KbdResponseHdlr");
  }
  return element;
}

void KbdResponseHdlr::Impl::checkedSplitList(
  Tcl_Obj* tcllist,
  Tcl_Obj**& elements_out,
  int& length_out
) const throw(ErrorWithMsg) {
DOTRACE("KbdResponseHdlr::Impl::checkedSplitList");

  if ( Tcl_ListObjGetElements(itsInterp, tcllist, 
										&length_out, &elements_out) != TCL_OK ) {
	 throw ErrorWithMsg("error splitting list for KbdResponseHdlr");
  }
}

Tcl_RegExp KbdResponseHdlr::Impl::getCheckedRegexp(
  Tcl_Obj* patternObj
) const throw(ErrorWithMsg) {
DOTRACE("KbdResponseHdlr::Impl::getCheckedRegexp");
  const int flags = 0;
  Tcl_RegExp regexp = Tcl_GetRegExpFromObj(itsInterp, patternObj, flags);
  if (!regexp) {
	 throw ErrorWithMsg("error creating a Tcl_RegExp for KbdResponseHdlr");
  }
  return regexp;
}

int KbdResponseHdlr::Impl::getCheckedInt(
  Tcl_Obj* intObj
) const throw(ErrorWithMsg) {
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
{}

KbdResponseHdlr::~KbdResponseHdlr()
  { delete itsImpl; }

void KbdResponseHdlr::serialize(ostream &os, IOFlag flag) const
  { itsImpl->serialize(os, flag); }

void KbdResponseHdlr::deserialize(istream &is, IOFlag flag)
  { itsImpl->deserialize(is, flag); }

int KbdResponseHdlr::charCount() const
  { return itsImpl->charCount(); }

void KbdResponseHdlr::readFrom(Reader* reader)
  { itsImpl->readFrom(reader); }

void KbdResponseHdlr::writeTo(Writer* writer) const
  { itsImpl->writeTo(writer); }

void KbdResponseHdlr::setInterp(Tcl_Interp* interp)
  { itsImpl->setInterp(interp); }

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

void KbdResponseHdlr::rhBeginTrial() const
  { itsImpl->rhBeginTrial(); }

void KbdResponseHdlr::rhAbortTrial() const
  { itsImpl->rhAbortTrial(); }

void KbdResponseHdlr::rhHaltExpt() const
  { itsImpl->rhHaltExpt(); }

static const char vcid_kbdresponsehdlr_cc[] = "$Header$";
#endif // !KBDRESPONSEHDLR_CC_DEFINED
