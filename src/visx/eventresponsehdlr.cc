///////////////////////////////////////////////////////////////////////
//
// eventresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov  9 15:32:48 1999
// written: Tue Dec  7 19:01:28 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EVENTRESPONSEHDLR_CC_DEFINED
#define EVENTRESPONSEHDLR_CC_DEFINED

#include "eventresponsehdlr.h"

#include <tcl.h>
#include <strstream.h>
#include <vector>

#include "error.h"
#include "experiment.h"
#include "sound.h"
#include "soundlist.h"
#include "reader.h"
#include "tclevalcmd.h"
#include "tclobjlock.h"
#include "widget.h"
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
  const string ioTag = "EventResponseHdlr";

  const string nullScript = "{}";

  unsigned long cmdCounter = 0;
}

///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class EventResponseHdlr::Impl {
public:
  Impl(EventResponseHdlr* owner, const string& input_response_map);
  ~Impl();

  // Delegand functions
  void serialize(ostream &os, IOFlag flag) const;
  void deserialize(istream &is, IOFlag flag);
  int charCount() const;

  void oldSerialize(ostream &os, IOFlag flag) const;
  void oldDeserialize(istream &is, IOFlag flag);
  int oldCharCount() const;

  void readFrom(Reader* reader);
  void writeTo(Writer* writer) const;

  void setInterp(Tcl_Interp* interp);

  const string& getInputResponseMap() const
	 { return itsInputResponseMap; }

  void setInputResponseMap(const string& s)
	 {
		itsInputResponseMap = s; 
		updateRegexps();
	 }

  bool getUseFeedback() const
	 { return itsUseFeedback; }

  void setUseFeedback(bool val)
	 { itsUseFeedback = val; }

  const char* getFeedbackMap() const
	 { return itsFeedbackMap.c_str(); }

  void setFeedbackMap(const char* feedback_string)
	 {
		itsFeedbackMap = feedback_string;
		updateFeedbacks();
	 }

  const string& getEventSequence() const
	 { return itsEventSequence; }

  void setEventSequence(const string& seq)
	 { itsEventSequence = seq; }

  const string& getBindingSubstitution() const
	 { return itsBindingSubstitution; }

  void setBindingSubstitution(const string& sub)
	 { itsBindingSubstitution = sub; }

  void rhBeginTrial(Experiment* expt) const
	 { itsExperiment = expt; attend(expt); }

  void rhAbortTrial(Experiment* expt) const;

  void rhEndTrial(Experiment* expt) const
	 { itsExperiment = expt; ignore(expt); }

  void rhHaltExpt(Experiment* expt) const
	 { itsExperiment = expt; ignore(expt); }

  // Helper functions
private:
  Experiment& getExpt() const
	 {
		if (itsExperiment == 0)
		  { throw ErrorWithMsg("EventResponseHdlr::itsExperiment is NULL"); }
		return *itsExperiment;
	 }

  // When this procedure is invoked, the program listens to the events
  // that are pertinent to the response policy. This procedure should
  // be called when a trial is begun, and should be cancelled with
  // ignore() when a response has been made so that events are not
  // received during response processing and during the inter-trial
  // period.
  void attend(Experiment* expt) const;

  // When this procedure is invoked, the program ignores
  // user-generated events (mouse or keyboard) that would otherwise
  // signal a response. This effect is useful when the experiment is
  // in between trials, when a current response is already being
  // processed, and any other time when it is necessary to avoid an
  // unintended key/button-press being interpreted as a response. The
  // effect is cancelled by calling attend().
  void ignore(Experiment* expt) const;


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
						  itsBindingSubstitution + " }"); }

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
		const string privateHandleCmdName = "__EventResponseHdlrPrivate::handle";

		vector<char> buf(privateHandleCmdName.length() + 32);
		ostrstream ost(&buf[0], privateHandleCmdName.length() + 32);
		ost << privateHandleCmdName << ++cmdCounter << '\0';		
		return string(&buf[0]);
	 }

  // data
private:
  EventResponseHdlr* itsOwner;

  mutable Experiment* itsExperiment;

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
									  "for EventResponseHdlr");

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

  string itsInputResponseMap;
  mutable vector<RegExp_ResponseVal> itsRegexps;

  class Condition_Feedback {
  public:
  	 Condition_Feedback(Tcl_Obj* cond, Tcl_Obj* res) :
  		itsCondition(cond),
 		itsResultCmd(res, Tcl::TclEvalCmd::THROW_EXCEPTION, TCL_EVAL_GLOBAL)
		{}

	 bool isTrue(Tcl_Interp* interp) throw(ErrorWithMsg)
		{
		  int expr_result;
		  if (Tcl_ExprBooleanObj(interp, itsCondition, &expr_result) != TCL_OK) {
			 throw ErrorWithMsg("error evaluating boolean expression "
									  "for EventResponseHdlr");
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
	 Tcl::TclObjPtr itsCondition;
 	 Tcl::TclEvalCmd itsResultCmd;
  };

  string itsFeedbackMap;
  mutable vector<Condition_Feedback> itsFeedbacks;

  bool itsUseFeedback;

  string itsEventSequence;
  string itsBindingSubstitution;
};

///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr::Impl method definitions
//
///////////////////////////////////////////////////////////////////////

EventResponseHdlr::Impl::Impl(EventResponseHdlr* owner,
										const string& input_response_map) :
  itsOwner(owner),
  itsExperiment(0),
  itsInterp(0),
  itsTclCmdToken(0),
  itsPrivateCmdName(getUniqueCmdName()),
  itsInputResponseMap(input_response_map),
  itsRegexps(),
  itsFeedbackMap(),
  itsFeedbacks(),
  itsUseFeedback(true),
  itsEventSequence("<KeyPress>"),
  itsBindingSubstitution("%K")
{
DOTRACE("EventResponseHdlr::Impl::Impl");
}

EventResponseHdlr::Impl::~Impl() {
DOTRACE("EventResponseHdlr::Impl::~Impl");

  // We must check if itsInterp has been tagged for deletion already,
  // since if it is then we must not attempt to use it to delete a Tcl
  // command (this results in "called Tcl_HashEntry on deleted
  // table"). Not deleting the command in that case does not cause a
  // resource leak, however, since the Tcl_Interp as part if its own
  // destruction will delete all commands associated with it.

  if ( !Tcl_InterpDeleted(itsInterp) ) {
	 if (itsExperiment != 0)
		ignore(itsExperiment);
 
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

void EventResponseHdlr::Impl::serialize(ostream &os, IOFlag flag) const {
DOTRACE("EventResponseHdlr::Impl::serialize");

  if (flag & TYPENAME) { os << ioTag << IO::SEP; }

  oldSerialize(os, flag);

  os << itsEventSequence << endl;
  os << itsBindingSubstitution << endl;

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { /* no bases to serialize */ }
}

void EventResponseHdlr::Impl::deserialize(istream &is, IOFlag flag) {
DOTRACE("EventResponseHdlr::Impl::deserialize");

  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  oldDeserialize(is, flag);

  getline(is, itsEventSequence, '\n');
  getline(is, itsBindingSubstitution, '\n');

  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { /* no bases to deserialize */ }
}

int EventResponseHdlr::Impl::charCount() const {
DOTRACE("EventResponseHdlr::Impl::charCount");
  return(ioTag.length() + 1
			+ oldCharCount()
			+ itsEventSequence.length() + 1
			+ itsBindingSubstitution.length() + 1);
}

void EventResponseHdlr::Impl::oldSerialize(ostream &os, IOFlag) const {
DOTRACE("EventResponseHdlr::Impl::oldSerialize");
  os << itsInputResponseMap << endl;
  os << itsFeedbackMap << endl;
  os << itsUseFeedback << endl;
}

void EventResponseHdlr::Impl::oldDeserialize(istream &is, IOFlag flag) {
DOTRACE("EventResponseHdlr::Impl::oldDeserialize");
  // XXX This is some sort of strange platform dependency..if the next
  // character in the stream is a space (the separator following the
  // typename), then we must pull it off the stream; however, in some
  // cases (using aCC/hp9000s700), the space is already gone by the
  // time we get here.
  DebugEvalNL(is.peek());
  if ( is.peek() == ' ' ) {
	 is.get();
  }

  getline(is, itsInputResponseMap, '\n');
  updateRegexps();

  DebugEvalNL(is.peek());
  getline(is, itsFeedbackMap, '\n');
  updateFeedbacks();

  int val;
  is >> val;
  itsUseFeedback = bool(val);
}

int EventResponseHdlr::Impl::oldCharCount() const {
DOTRACE("EventResponseHdlr::Impl::oldCharCount");
  return (itsInputResponseMap.length() + 1
			 + itsFeedbackMap.length() + 1
			 + gCharCount<bool>(itsUseFeedback) + 1
			 + 1); // fudge factor
}

void EventResponseHdlr::Impl::readFrom(Reader* reader) {
DOTRACE("EventResponseHdlr::Impl::readFrom");

  reader->readValue("inputResponseMap", itsInputResponseMap);
  reader->readValue("feedbackMap", itsFeedbackMap);
  reader->readValue("useFeedback", itsUseFeedback);
  reader->readValue("eventSequence", itsEventSequence);
  reader->readValue("bindingSubstitution", itsBindingSubstitution);

  updateRegexps();
  updateFeedbacks();
}

void EventResponseHdlr::Impl::writeTo(Writer* writer) const {
DOTRACE("EventResponseHdlr::Impl::writeTo");

  writer->writeValue("inputResponseMap", itsInputResponseMap);
  writer->writeValue("feedbackMap", itsFeedbackMap);
  writer->writeValue("useFeedback", itsUseFeedback);
  writer->writeValue("eventSequence", itsEventSequence);
  writer->writeValue("bindingSubstitution", itsBindingSubstitution);
}

void EventResponseHdlr::Impl::setInterp(Tcl_Interp* interp) {
DOTRACE("EventResponseHdlr::Impl::setInterp");
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

void EventResponseHdlr::Impl::rhAbortTrial(Experiment* expt) const {
DOTRACE("EventResponseHdlr::Impl::rhAbortTrial");
  itsExperiment = expt; 

  Assert(itsInterp != 0);

  ignore(expt);

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
// EventResponseHdlr::Impl helper method definitions
//
///////////////////////////////////////////////////////////////////////

void EventResponseHdlr::Impl::attend(Experiment* expt) const {
DOTRACE("EventResponseHdlr::Impl::attend");
  clearEventQueue();
  
  try {
	 expt->getWidget()->bind(itsEventSequence.c_str(),
									 getBindingScript().c_str());
  }
  catch (ErrorWithMsg& err) {
	 raiseBackgroundError(err.msg());
  }
  catch (...) {
	 raiseBackgroundError("error creating event binding "
								 "in EventResponseHdlr::attend");
  }
}

void EventResponseHdlr::Impl::ignore(Experiment* expt) const {
DOTRACE("EventResponseHdlr::Impl::ignore");
  try {
	 expt->getWidget()->bind(itsEventSequence.c_str(),
									 nullScript.c_str());
  }
  catch (ErrorWithMsg& err) {
	 raiseBackgroundError(err.msg());
  }
  catch (...) {
	 raiseBackgroundError("error creating event binding "
								 "in EventResponseHdlr::ignore");
  }
}

void EventResponseHdlr::Impl::raiseBackgroundError(const char* msg) const throw() {
DOTRACE("EventResponseHdlr::Impl::raiseBackgroundError"); 
  DebugEvalNL(msg); 
  Assert(itsInterp != 0);
  Tcl_AppendResult(itsInterp, msg, (char*) 0);
  Tcl_BackgroundError(itsInterp);
}

//--------------------------------------------------------------------
//
// EventResponseHdlr::privateHandleCmd --
//
// This Tcl command procedure gives a hook back into the C++ code from
// Tcl from the event binding that is set up in attend().
//
//--------------------------------------------------------------------

int EventResponseHdlr::Impl::privateHandleCmd(
  ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]
  ) {
DOTRACE("EventResponseHdlr::Impl::privateHandleCmd");
  // We assert that objc is 2 because this command should only ever
  // be invoked by a callback set up by EventResponseHdlr, which
  // should never call this with the wrong number of args.
  Assert(objc==2);

  EventResponseHdlr::Impl* impl =
	 static_cast<EventResponseHdlr::Impl *>(clientData);

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


void EventResponseHdlr::Impl::handleResponse(const char* keysym) const {
DOTRACE("EventResponseHdlr::Impl::handleResponse");

  getExpt().edResponseSeen();

  ignore(&(getExpt()));

  int response = getRespFromKeysym(keysym);

  if ( !isValidResponse(response) ) {
	 getExpt().edAbortTrial();
  }

  else {
	 getExpt().edProcessResponse(response);
	 if (itsUseFeedback) { feedback(response); }
  }
}

//--------------------------------------------------------------------
//
// EventResponseHdlr::getRespFromKeysysm --
//
// This procedure returns the response value associated with the first
// regexp in itsRegexps that matches the given character string, or
// returns INVALID_RESPONSE if none of the regexps give a match.
//
//--------------------------------------------------------------------

int EventResponseHdlr::Impl::getRespFromKeysym(const char* keysym) const {
DOTRACE("EventResponseHdlr::Impl::getRespFromKeysym");
  Assert(itsInterp != 0);

  const char* response_string = extractStringFromKeysym(keysym);
    
  for (size_t i = 0; i < itsRegexps.size(); ++i) {
	 if (itsRegexps[i].matchesString(itsInterp, response_string)) {
		return itsRegexps[i].responseValue();
	 }
  }

  return ResponseHandler::INVALID_RESPONSE;
}

void EventResponseHdlr::Impl::feedback(int response) const {
DOTRACE("EventResponseHdlr::Impl::feedback");
  Assert(itsInterp != 0);

  DebugEvalNL(response);

  Tcl_SetVar2Ex(itsInterp, "resp_val", NULL,
					 Tcl_NewIntObj(response), TCL_GLOBAL_ONLY);

  bool feedbackGiven = false;
  for (size_t i = 0; i<itsFeedbacks.size() && !feedbackGiven; ++i) {
	 feedbackGiven = itsFeedbacks[i].invokeIfTrue(itsInterp);
  }

  Tcl_UnsetVar(itsInterp, "resp_val", 0);
}


//---------------------------------------------------------------------
//
// EventResponseHdlr::updateFeedacks --
//
//---------------------------------------------------------------------

void EventResponseHdlr::Impl::updateFeedbacks() {
DOTRACE("EventResponseHdlr::Impl::updateFeedbacks");
  Assert(itsInterp != 0);

  itsFeedbacks.clear(); 

  try {
	 Tcl_Obj** pairs;
	 int num_pairs=0;
	 Tcl::TclObjPtr pairs_list(Tcl_NewStringObj(itsFeedbackMap.c_str(), -1));
	 checkedSplitList(pairs_list, pairs, num_pairs);

	 for (int i = 0; i < num_pairs; ++i) {

		Tcl::TclObjPtr current_pair = pairs[i];

		// Check that the length of the "pair" is really 2
		if (getCheckedListLength(current_pair) != 2) {
		  raiseBackgroundError("\"pair\" did not have length 2 "
									  "in EventResponseHdlr::updateFeedbacks");
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
// EventResponseHdlr::updateRegexps --
//
// Recompiles the internal table of regexps to correspond with the
// list of regexps and response values stored in the string
// itsInputResponseMap.
//
//--------------------------------------------------------------------

void EventResponseHdlr::Impl::updateRegexps() {
DOTRACE("EventResponseHdlr::updateRegexps");
  Assert(itsInterp != 0);

  // Get rid of any old stored regexps/response value pairs.
  itsRegexps.clear();

  try {
	 Tcl_Obj** pairs;
	 int num_pairs=0;
	 Tcl::TclObjPtr pairs_list(Tcl_NewStringObj(itsInputResponseMap.c_str(), -1));
	 checkedSplitList(pairs_list, pairs, num_pairs);

	 for (int i = 0; i < num_pairs; ++i) {

		Tcl::TclObjPtr current_pair = pairs[i];

		// Check that the length of the "pair" is really 2
		if (getCheckedListLength(current_pair) != 2) {
		  raiseBackgroundError("\"pair\" did not have length 2 "
									  "in EventResponseHdlr::updateRegexps");
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

int EventResponseHdlr::Impl::getCheckedListLength(
  Tcl_Obj* tcllist
) const throw(ErrorWithMsg) {
DOTRACE("EventResponseHdlr::Impl::getCheckedListLength");
  int length;
  if (Tcl_ListObjLength(itsInterp, tcllist, &length) != TCL_OK) {
	 throw ErrorWithMsg("error getting list length for EventResponseHdlr");
  }
  return length;
}

Tcl_Obj* EventResponseHdlr::Impl::getCheckedListElement(
  Tcl_Obj* tcllist, int index
) const throw(ErrorWithMsg) {
DOTRACE("EventResponseHdlr::Impl::getCheckedListElement");

  Tcl_Obj* element = NULL;
  if (Tcl_ListObjIndex(itsInterp, tcllist, index, &element) != TCL_OK) {
	 throw ErrorWithMsg("error getting list element for EventResponseHdlr");
  }
  return element;
}

void EventResponseHdlr::Impl::checkedSplitList(
  Tcl_Obj* tcllist,
  Tcl_Obj**& elements_out,
  int& length_out
) const throw(ErrorWithMsg) {
DOTRACE("EventResponseHdlr::Impl::checkedSplitList");

  if ( Tcl_ListObjGetElements(itsInterp, tcllist, 
										&length_out, &elements_out) != TCL_OK ) {
	 throw ErrorWithMsg("error splitting list for EventResponseHdlr");
  }
}

Tcl_RegExp EventResponseHdlr::Impl::getCheckedRegexp(
  Tcl_Obj* patternObj
) const throw(ErrorWithMsg) {
DOTRACE("EventResponseHdlr::Impl::getCheckedRegexp");
  const int flags = 0;
  Tcl_RegExp regexp = Tcl_GetRegExpFromObj(itsInterp, patternObj, flags);
  if (!regexp) {
	 throw ErrorWithMsg("error creating a Tcl_RegExp for EventResponseHdlr");
  }
  return regexp;
}

int EventResponseHdlr::Impl::getCheckedInt(
  Tcl_Obj* intObj
) const throw(ErrorWithMsg) {
DOTRACE("EventResponseHdlr::Impl::getCheckedInt");
  int return_val=-1;
  if (Tcl_GetIntFromObj(itsInterp, intObj, &return_val) != TCL_OK) {
	 throw ErrorWithMsg("error getting int from Tcl_Obj for EventResponseHdlr");
  }
  return return_val;
}


///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr method definitions
//
// All of these methods just delegate to EventResponseHdlr::Impl methods
//
///////////////////////////////////////////////////////////////////////

EventResponseHdlr::EventResponseHdlr(const string& input_response_map) : 
  ResponseHandler(),
  itsImpl(new Impl(this, input_response_map))
{}

EventResponseHdlr::~EventResponseHdlr()
  { delete itsImpl; }

void EventResponseHdlr::serialize(ostream &os, IOFlag flag) const
  { itsImpl->serialize(os, flag); }

void EventResponseHdlr::deserialize(istream &is, IOFlag flag)
  { itsImpl->deserialize(is, flag); }

int EventResponseHdlr::charCount() const
  { return itsImpl->charCount(); }

void EventResponseHdlr::readFrom(Reader* reader)
  { itsImpl->readFrom(reader); }

void EventResponseHdlr::writeTo(Writer* writer) const
  { itsImpl->writeTo(writer); }

void EventResponseHdlr::setInterp(Tcl_Interp* interp)
  { itsImpl->setInterp(interp); }

const string& EventResponseHdlr::getInputResponseMap() const {
DOTRACE("EventResponseHdlr::getInputResponseMap");
  return itsImpl->getInputResponseMap();
}

void EventResponseHdlr::setInputResponseMap(const string& s) {
DOTRACE("EventResponseHdlr::setInputResponseMap");
  itsImpl->setInputResponseMap(s);
}

bool EventResponseHdlr::getUseFeedback() const {
DOTRACE("EventResponseHdlr::getUseFeedback");
  return itsImpl->getUseFeedback();
}

void EventResponseHdlr::setUseFeedback(bool val) {
DOTRACE("EventResponseHdlr::setUseFeedback");
  itsImpl->setUseFeedback(val);
}

const char* EventResponseHdlr::getFeedbackMap() const {
DOTRACE("EventResponseHdlr::getFeedbackMap");
  return itsImpl->getFeedbackMap();
}

void EventResponseHdlr::setFeedbackMap(const char* feedback_string) {
DOTRACE("EventResponseHdlr::setFeedbackMap");
  itsImpl->setFeedbackMap(feedback_string);
}

const string& EventResponseHdlr::getEventSequence() const {
  return itsImpl->getEventSequence();
}

void EventResponseHdlr::setEventSequence(const string& seq) {
  itsImpl->setEventSequence(seq);
}

const string& EventResponseHdlr::getBindingSubstitution() const {
  return itsImpl->getBindingSubstitution();
}

void EventResponseHdlr::setBindingSubstitution(const string& sub) {
  itsImpl->setBindingSubstitution(sub);
}

void EventResponseHdlr::rhBeginTrial(Experiment* expt) const
  { itsImpl->rhBeginTrial(expt); }

void EventResponseHdlr::rhAbortTrial(Experiment* expt) const
  { itsImpl->rhAbortTrial(expt); }

void EventResponseHdlr::rhEndTrial(Experiment* expt) const
  { itsImpl->rhEndTrial(expt); }

void EventResponseHdlr::rhHaltExpt(Experiment* expt) const
  { itsImpl->rhHaltExpt(expt); }

void EventResponseHdlr::oldSerialize(ostream &os, IOFlag flag) const {
  itsImpl->oldSerialize(os, flag);
}

void EventResponseHdlr::oldDeserialize(istream &is, IOFlag flag) {
  itsImpl->oldDeserialize(is, flag);
}

int EventResponseHdlr::oldCharCount() const {
  return itsImpl->oldCharCount();
}

static const char vcid_eventresponsehdlr_cc[] = "$Header$";
#endif // !EVENTRESPONSEHDLR_CC_DEFINED
