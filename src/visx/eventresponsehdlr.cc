///////////////////////////////////////////////////////////////////////
//
// eventresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov  9 15:32:48 1999
// written: Mon Oct  9 18:25:47 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EVENTRESPONSEHDLR_CC_DEFINED
#define EVENTRESPONSEHDLR_CC_DEFINED

#include "eventresponsehdlr.h"

#include "sound.h"
#include "soundlist.h"
#include "response.h"
#include "trialbase.h"

#include "io/iolegacy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "gwt/widget.h"

#include "tcl/tclevalcmd.h"
#include "tcl/tclobjlock.h"

#include "util/arrays.h"
#include "util/error.h"
#include "util/strings.h"

#include <tcl.h>
#include <strstream.h>

#define DYNAMIC_TRACE_EXPR EventResponseHdlr::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string_literal ioTag("EventResponseHdlr");

  const string_literal nullScript("{}");

  Tcl::TclObjPtr theNullObject(Tcl_NewStringObj("",-1));
}

///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class EventResponseHdlr::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

  void legacySrlz(IO::LegacyWriter* writer) const;
  void legacyDesrlz(IO::LegacyReader* reader);

public:
  Impl(EventResponseHdlr* owner, const char* input_response_map);
  ~Impl();

  enum TrialState { ACTIVE, INACTIVE };

  // Delegand functions

  void oldLegacySrlz(IO::Writer* writer) const;
  void oldLegacyDesrlz(IO::Reader* reader);

  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;

  void setInterp(Tcl_Interp* interp);

  const fixed_string& getInputResponseMap() const
	 { return itsInputResponseMap; }

  void setInputResponseMap(const fixed_string& s)
	 {
		itsInputResponseMap = s;
		itsRegexpsAreDirty = true;
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
		itsFeedbacksAreDirty = true;
	 }

  const fixed_string& getEventSequence() const
	 { return itsEventSequence; }

  void setEventSequence(const fixed_string& seq)
	 { itsEventSequence = seq; }

  const fixed_string& getBindingSubstitution() const
	 { return itsBindingSubstitution; }

  void setBindingSubstitution(const fixed_string& sub)
	 { itsBindingSubstitution = sub.c_str(); }

  void abortInvalidResponses()
	 { itsAbortInvalidResponses = true; }

  void ignoreInvalidResponses()
	 { itsAbortInvalidResponses = false; }

  void rhBeginTrial(GWT::Widget& widget, TrialBase& trial) const
	 {
		becomeActive(widget, trial);
		attend();
	 }

  void rhAbortTrial() const;

  void rhEndTrial() const
	 { 
		if (INACTIVE == itsState) return;
		ignore();
		becomeInactive();
	 }

  void rhHaltExpt() const
	 {
		if (INACTIVE == itsState) return; 
		ignore();
		becomeInactive();
	 }

  // Helper functions
private:

  void becomeActive(GWT::Widget& widget, TrialBase& trial) const
    {
		itsWidget = &widget; Assert(&widget != 0);
		itsTrial = &trial;   Assert(&trial != 0);

		itsState = ACTIVE;
	 }

  void becomeInactive() const
    {
		itsState = INACTIVE;
		itsWidget = 0;
		itsTrial = 0;
	 }

  GWT::Widget& getWidget() const
	 {
		DebugEval((void*) itsWidget);
		Assert(itsState == ACTIVE);
		Assert(itsWidget != 0);
		return *itsWidget;
	 }


  TrialBase& getTrial() const
	 {
		Assert(itsState == ACTIVE);
		Assert(itsTrial != 0);
		return *itsTrial;
	 }

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

  static Tcl_ObjCmdProc privateHandleCmd;
  void handleResponse(const char* keysym) const;
  int getRespFromKeysym(const char* keysym) const;
  void feedback(int response) const;
  void updateFeedbacksIfNeeded() const;
  void updateRegexpsIfNeeded() const;

  int getCheckedListLength(Tcl_Obj* tcllist) const throw(ErrorWithMsg);

  Tcl_Obj* getCheckedListElement(
    Tcl_Obj* tcllist, int index) const throw(ErrorWithMsg);

  void checkedSplitList(Tcl_Obj* tcllist,
	 Tcl_Obj**& elements_out, int& length_out) const throw(ErrorWithMsg);

  int getCheckedInt(Tcl_Obj* intObj) const throw(ErrorWithMsg);

  dynamic_string getBindingScript() const
	 {
		return dynamic_string("{ ")
		  .append(itsPrivateCmdName).append(" ")
		  .append(itsBindingSubstitution).append(" }");
	 }

  void clearEventQueue() const
	 {
		while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0)
		  { /* Empty loop body */ }
	 }

  // We take the last character of the string as the response, in
  // order to handle the numeric keypad, where the keysysms are
  // 'KP_0', 'KP_3', etc., wheras we want just the 0 or the 3.
  const char* extractStringFromKeysym(const char* text) const
	 {
		while ( *(++text) != '\0' )
		  { ; }
		return (text-1);
	 }

  fixed_string getUniqueCmdName()
	 {
		const string_literal privateHandleCmdName(
		  "__EventResponseHdlrPrivate::handle");

		static unsigned long cmdCounter = 0;

		fixed_block<char> buf(privateHandleCmdName.length() + 32);
		ostrstream ost(&buf[0], buf.size());
		ost << privateHandleCmdName << ++cmdCounter << '\0';
		return &buf[0];
	 }

  // data
private:
  EventResponseHdlr* itsOwner;

  mutable TrialState itsState;

  mutable GWT::Widget* itsWidget;
  mutable TrialBase* itsTrial;

  Tcl_Interp* itsInterp;

  mutable Tcl_Command itsTclCmdToken;
  const fixed_string itsPrivateCmdName;

  class RegExp_ResponseVal {
  public:
    RegExp_ResponseVal(Tcl::TclObjPtr obj = theNullObject, int rv = -1) :
		itsPatternObj(obj),
		itsRespVal(rv)
		{}

	 RegExp_ResponseVal(const RegExp_ResponseVal& other) :
		itsPatternObj(other.itsPatternObj),
		itsRespVal(other.itsRespVal)
		{}

	 RegExp_ResponseVal& operator=(const RegExp_ResponseVal& other)
		{
		  itsPatternObj = other.itsPatternObj;
		  itsRespVal = other.itsRespVal;
		  return *this;
		}

	 bool matchesString(Tcl_Interp* interp,
							  const char* string_to_match) throw(ErrorWithMsg)
		{
		  static const int REGEX_ERROR        = -1;
		  static const int REGEX_NO_MATCH     =  0;
		  static const int REGEX_FOUND_MATCH  =  1;

		  DebugEval(string_to_match); DebugEvalNL(Tcl_GetString(itsPatternObj));

		  Tcl_RegExp regexp = getCheckedRegexp(interp, itsPatternObj);

		  int regex_result =
			 Tcl_RegExpExec(interp, regexp, string_to_match, string_to_match);

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

		  return false;
		}

	 int responseValue() { return itsRespVal; }

  private:
	 static Tcl_RegExp getCheckedRegexp(
		Tcl_Interp* interp, Tcl::TclObjPtr patternObj
		)
		throw(ErrorWithMsg)
		{
		  const int flags = 0;
		  Tcl_RegExp regexp = Tcl_GetRegExpFromObj(interp, patternObj, flags);
		  if (!regexp) {
			 throw ErrorWithMsg("error getting a Tcl_RegExp from ")
				      .appendMsg("'", Tcl_GetString(patternObj), "'");
		  }
		  return regexp;
		}

	 Tcl::TclObjPtr itsPatternObj;
    int itsRespVal;
  };

  fixed_string itsInputResponseMap;
  mutable dynamic_block<RegExp_ResponseVal> itsRegexps;

  mutable bool itsRegexpsAreDirty;

  class Condition_Feedback {
  public:
	 // This no-argument constructor puts the object in an invalid
	 // state, which we mark with itsIsValid=false. In order for the
	 // object to be used, it must be assigned to with the default
	 // assignment constructor.
  	 Condition_Feedback() :
		itsIsValid(false),
  		itsCondition(theNullObject),
 		itsResultCmd(theNullObject)
		{}

  	 Condition_Feedback(Tcl_Obj* cond, Tcl_Obj* res) :
		itsIsValid(false),
  		itsCondition(cond),
 		itsResultCmd(res, Tcl::TclEvalCmd::THROW_EXCEPTION, TCL_EVAL_GLOBAL)
		{
		  if (cond != 0 && res != 0)
			 itsIsValid = true;
		}

	 bool invokeIfTrue(Tcl_Interp* interp) throw (ErrorWithMsg)
		{
		  if (isTrue(interp))
			 { invoke(interp); return true; }
		  return false;
		}

  private:
	 bool isTrue(Tcl_Interp* interp) throw(ErrorWithMsg)
		{
		  Assert(itsIsValid);
		  int expr_result;
		  if (Tcl_ExprBooleanObj(interp, itsCondition, &expr_result) != TCL_OK)
			 {
				throw ErrorWithMsg("error evaluating boolean expression "
										 "for EventResponseHdlr");
			 }
		  return bool(expr_result);
		}

	 void invoke(Tcl_Interp* interp) throw(ErrorWithMsg)
		{ itsResultCmd.invoke(interp); }

	 bool itsIsValid;
	 Tcl::TclObjPtr itsCondition;
 	 Tcl::TclEvalCmd itsResultCmd;
  };

  fixed_string itsFeedbackMap;
  mutable dynamic_block<Condition_Feedback> itsFeedbacks;

  mutable bool itsFeedbacksAreDirty;

  bool itsUseFeedback;

  fixed_string itsEventSequence;
  fixed_string itsBindingSubstitution;

  bool itsAbortInvalidResponses;
};

///////////////////////////////////////////////////////////////////////
//
// Static member definitions
//
///////////////////////////////////////////////////////////////////////

Util::Tracer EventResponseHdlr::tracer;

///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr::Impl method definitions
//
///////////////////////////////////////////////////////////////////////

EventResponseHdlr::Impl::Impl(EventResponseHdlr* owner,
										const char* input_response_map) :
  itsOwner(owner),
  itsState(INACTIVE),
  itsWidget(0),
  itsTrial(0),
  itsInterp(0),
  itsTclCmdToken(0),
  itsPrivateCmdName(getUniqueCmdName().c_str()),
  itsInputResponseMap(input_response_map),
  itsRegexps(),
  itsRegexpsAreDirty(true),
  itsFeedbackMap(),
  itsFeedbacks(),
  itsFeedbacksAreDirty(true),
  itsUseFeedback(true),
  itsEventSequence("<KeyPress>"),
  itsBindingSubstitution("%K"),
  itsAbortInvalidResponses(true)
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
	 try
		{
		  if (ACTIVE == itsState) ignore();

		  DebugPrint("deleting Tcl command "); DebugPrintNL(itsPrivateCmdName);

		  DebugEvalNL((void*) itsTclCmdToken);
		  Tcl_DeleteCommandFromToken(itsInterp, itsTclCmdToken);
		}
	 catch (ErrorWithMsg& err)
		{ DebugEvalNL(err.msg_cstr()); }
	 catch (...)
		{ DebugPrintNL("an unknown error occurred"); }
  }
}

void EventResponseHdlr::Impl::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("EventResponseHdlr::Impl::legacySrlz");

  oldLegacySrlz(lwriter);

  lwriter->writeValue("eventSequence", itsEventSequence);
  lwriter->writeValue("bindingSubstitution", itsBindingSubstitution);
}

void EventResponseHdlr::Impl::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("EventResponseHdlr::Impl::legacyDesrlz");

  oldLegacyDesrlz(lreader);

  lreader->readValue("eventSequence", itsEventSequence);
  lreader->readValue("bindingSubstitution", itsBindingSubstitution);
}

void EventResponseHdlr::Impl::oldLegacySrlz(IO::Writer* writer) const {
DOTRACE("EventResponseHdlr::Impl::oldLegacySrlz");

  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 lwriter->setStringMode(IO::GETLINE_NEWLINE);

	 writer->writeValue("inputResponseMap", itsInputResponseMap);
	 writer->writeValue("feedbackMap", itsFeedbackMap);

	 lwriter->setFieldSeparator('\n');
	 writer->writeValue("useFeedback", itsUseFeedback);
  }
}

void EventResponseHdlr::Impl::oldLegacyDesrlz(IO::Reader* reader) {
DOTRACE("EventResponseHdlr::Impl::oldLegacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 lreader->setStringMode(IO::GETLINE_NEWLINE);
	 reader->readValue("inputResponseMap", itsInputResponseMap);
	 reader->readValue("feedbackMap", itsFeedbackMap);
	 reader->readValue("useFeedback", itsUseFeedback);

	 itsRegexpsAreDirty = true;
	 itsFeedbacksAreDirty = true;

#ifdef MIPSPRO_COMPILER
	 // The next character after itsUseFeedback had better be a
	 // newline, and we need to remove it from the stream. ... OK, now
	 // I've commented out the code that does this, since I believe
	 // it's no longer necessary.

//    	 int cc = lreader->getChar();
//    	 if ( cc != '\n' ) {
//    		DebugEvalNL(cc);
//    		throw IO::LogicError(ioTag.c_str());
//    	 }
#endif
  }
}

void EventResponseHdlr::Impl::readFrom(IO::Reader* reader) {
DOTRACE("EventResponseHdlr::Impl::readFrom");

  itsState = INACTIVE; 

  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 legacyDesrlz(lreader);
	 return;
  }

  reader->readValue("inputResponseMap", itsInputResponseMap);
  reader->readValue("feedbackMap", itsFeedbackMap);
  reader->readValue("useFeedback", itsUseFeedback);
  reader->readValue("eventSequence", itsEventSequence);
  reader->readValue("bindingSubstitution", itsBindingSubstitution);

  itsRegexpsAreDirty = true;
  itsFeedbacksAreDirty = true;
}

void EventResponseHdlr::Impl::writeTo(IO::Writer* writer) const {
DOTRACE("EventResponseHdlr::Impl::writeTo");

  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 legacySrlz(lwriter);
	 return;
  }

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
  }
}

void EventResponseHdlr::Impl::rhAbortTrial() const {
DOTRACE("EventResponseHdlr::Impl::rhAbortTrial");

  Assert(itsInterp != 0);

  if (INACTIVE == itsState) return;

  ignore();

  const int ERR_INDEX = 1;
  SoundList::SharedPtr p = SoundList::theSoundList().getCheckedPtr(ERR_INDEX);
  p->play();
}

///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr::Impl helper method definitions
//
///////////////////////////////////////////////////////////////////////

void EventResponseHdlr::Impl::attend() const {
DOTRACE("EventResponseHdlr::Impl::attend");
  clearEventQueue();

  // We need to check itsState here, since although itsState should
  // always be active when we enter this call, clearEventQueue() may
  // reenter this object and cause itsState to become active before
  // returning.
  if (itsState == INACTIVE) return;

  getWidget().bind(itsEventSequence.c_str(),
						 getBindingScript().c_str());
}

void EventResponseHdlr::Impl::ignore() const {
DOTRACE("EventResponseHdlr::Impl::ignore");

  getWidget().bind(itsEventSequence.c_str(),
  						 nullScript.c_str());
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
	 Tcl_AppendResult(interp, err.msg_cstr(), (char*) 0);
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

  Response theResponse;

  TrialBase& trial = getTrial();

  theResponse.setMsec(trial.trElapsedMsec());

  trial.trResponseSeen();

  ignore();

  theResponse.setVal(getRespFromKeysym(keysym)); DebugEvalNL(theResponse.val());

  if ( !theResponse.isValid() ) {
	 if ( itsAbortInvalidResponses )
		trial.trAbortTrial();
  }

  else {
	 trial.trRecordResponse(theResponse);
	 if (itsUseFeedback) { feedback(theResponse.val()); }
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

  DebugEvalNL(response_string);

  updateRegexpsIfNeeded();

  DebugEvalNL(itsRegexps.size());

  for (size_t i = 0; i < itsRegexps.size(); ++i) {

	 DebugEvalNL(i);

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

  updateFeedbacksIfNeeded();

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
// EventResponseHdlr::updateFeedacksIfNeeded --
//
//---------------------------------------------------------------------

void EventResponseHdlr::Impl::updateFeedbacksIfNeeded() const {
DOTRACE("EventResponseHdlr::Impl::updateFeedbacksIfNeeded");
  Assert(itsInterp != 0);

  if (!itsFeedbacksAreDirty) return;

  Tcl_Obj** pairs;
  int num_pairs=0;
  Tcl::TclObjPtr pairs_list(Tcl_NewStringObj(itsFeedbackMap.c_str(), -1));
  checkedSplitList(pairs_list, pairs, num_pairs);

  Assert(num_pairs >= 0);

  unsigned int uint_num_pairs = num_pairs;

  itsFeedbacks.resize(uint_num_pairs);

  for (unsigned int i = 0; i < uint_num_pairs; ++i) {

	 Tcl::TclObjPtr current_pair = pairs[i];

	 // Check that the length of the "pair" is really 2
	 if (getCheckedListLength(current_pair) != 2) {
		raiseBackgroundError("\"pair\" did not have length 2 "
									"in EventResponseHdlr::updateFeedbacksIfNeeded");
		return;
	 }

	 Tcl_Obj *condition = getCheckedListElement(current_pair, 0);
	 Tcl_Obj *result = getCheckedListElement(current_pair, 1);

	 itsFeedbacks.at(i) = Impl::Condition_Feedback(condition, result);
  }

  itsFeedbacksAreDirty = false;

  DebugPrintNL("updateFeedbacksIfNeeded success!");
}

//--------------------------------------------------------------------
//
// EventResponseHdlr::updateRegexpsIfNeeded --
//
// Recompiles the internal table of regexps to correspond with the
// list of regexps and response values stored in the string
// itsInputResponseMap.
//
//--------------------------------------------------------------------

void EventResponseHdlr::Impl::updateRegexpsIfNeeded() const {
DOTRACE("EventResponseHdlr::updateRegexpsIfNeeded");
  Assert(itsInterp != 0);

  if (!itsRegexpsAreDirty) return;

  Tcl_Obj** pairs;
  int num_pairs=0;
  Tcl::TclObjPtr pairs_list(Tcl_NewStringObj(itsInputResponseMap.c_str(), -1));
  checkedSplitList(pairs_list, pairs, num_pairs);

  Assert(num_pairs >= 0);
  unsigned int uint_num_pairs = num_pairs;

  itsRegexps.resize(uint_num_pairs);

  for (unsigned int i = 0; i < uint_num_pairs; ++i) {

	 Tcl::TclObjPtr current_pair = pairs[i];

	 // Check that the length of the "pair" is really 2
	 if (getCheckedListLength(current_pair) != 2) {
		raiseBackgroundError("\"pair\" did not have length 2 "
									"in EventResponseHdlr::updateRegexpsIfNeeded");
		return;
	 }

	 Tcl::TclObjPtr patternObj = getCheckedListElement(current_pair, 0);

	 Tcl::TclObjPtr response_valObj = getCheckedListElement(current_pair, 1);
	 int response_val = getCheckedInt(response_valObj);

	 itsRegexps.at(i) = Impl::RegExp_ResponseVal(patternObj, response_val);
  }

  itsRegexpsAreDirty = false;

  DebugPrintNL("updateRegexpsIfNeeded success!");
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


EventResponseHdlr::EventResponseHdlr() :
  ResponseHandler(),
  itsImpl(new Impl(this, ""))
{}

EventResponseHdlr::EventResponseHdlr(const char* input_response_map) :
  ResponseHandler(),
  itsImpl(new Impl(this, input_response_map))
{}

EventResponseHdlr::~EventResponseHdlr()
  { delete itsImpl; }

void EventResponseHdlr::readFrom(IO::Reader* reader)
  { itsImpl->readFrom(reader); }

void EventResponseHdlr::writeTo(IO::Writer* writer) const
  { itsImpl->writeTo(writer); }

void EventResponseHdlr::setInterp(Tcl_Interp* interp)
  { itsImpl->setInterp(interp); }

const fixed_string& EventResponseHdlr::getInputResponseMap() const {
DOTRACE("EventResponseHdlr::getInputResponseMap");
  return itsImpl->getInputResponseMap();
}

void EventResponseHdlr::setInputResponseMap(const fixed_string& s) {
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

const fixed_string& EventResponseHdlr::getEventSequence() const 
  { return itsImpl->getEventSequence(); }

void EventResponseHdlr::setEventSequence(const fixed_string& seq)
  { itsImpl->setEventSequence(seq); }

const fixed_string& EventResponseHdlr::getBindingSubstitution() const
  { return itsImpl->getBindingSubstitution(); }

void EventResponseHdlr::setBindingSubstitution(const fixed_string& sub)
  { itsImpl->setBindingSubstitution(sub); }

void EventResponseHdlr::abortInvalidResponses()
  { itsImpl->abortInvalidResponses(); }

void EventResponseHdlr::ignoreInvalidResponses()
  { itsImpl->ignoreInvalidResponses(); }

void EventResponseHdlr::rhBeginTrial(GWT::Widget& widget, TrialBase& trial) const
  { itsImpl->rhBeginTrial(widget, trial); }

void EventResponseHdlr::rhAbortTrial() const
  { itsImpl->rhAbortTrial(); }

void EventResponseHdlr::rhEndTrial() const
  { itsImpl->rhEndTrial(); }

void EventResponseHdlr::rhHaltExpt() const
  { itsImpl->rhHaltExpt(); }

void EventResponseHdlr::oldLegacySrlz(IO::Writer* writer) const
  { itsImpl->oldLegacySrlz(writer); }

void EventResponseHdlr::oldLegacyDesrlz(IO::Reader* reader)
  { itsImpl->oldLegacyDesrlz(reader); }

static const char vcid_eventresponsehdlr_cc[] = "$Header$";
#endif // !EVENTRESPONSEHDLR_CC_DEFINED
