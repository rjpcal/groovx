///////////////////////////////////////////////////////////////////////
//
// eventresponsehdlr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov  9 15:30:54 1999
// written: Wed Dec  1 14:38:02 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EVENTRESPONSEHDLR_H_DEFINED
#define EVENTRESPONSEHDLR_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef RESPONSEHANDLER_H_DEFINED
#include "responsehandler.h"
#endif


///////////////////////////////////////////////////////////////////////
/**
 *
 * EventResponseHandler provides a generic way to collect responses
 * and give feedback. It provides several hooks for Tcl code to define
 * how responses are interpreted, and how feedback is given. Clients
 * should select an event sequence of interest (for example,
 * "<KeyPress>"), then select an appropriate Tk binding substitution
 * that will extract the relevant event field (for example, "%K" gets
 * the keycode from a KeyPress event).
 *
 * Then, an input response map can be used to define an association
 * between values of the event field, and arbitrary response
 * values. This map is in the form of a Tcl list, where each item in
 * the list is a pair of 1) a regexp that will be used to try to match
 * the event field when an event occurs, and 2) a response value that
 * will be sent back to the ExptDriver if the regexp matches. For
 * example, the input response map
 *
 * <pre>{ {{^[aA]$} 0} {{^[lL]$} 1} }</pre>
 *
 * defines two possible response types, one which associates a lower-
 * or upper-case 'a' with the repsonse value 0, and another which
 * associates a lower- or upper-case 'l' with the response value 1. If
 * none of the regexps match the event field, trial is aborted.
 *
 * Finally, feedback can be controlled by setting the feedback
 * map. This map is another Tcl list in which each element contains a
 * boolean expression as well as a Tcl script to evaluate if the
 * expression is true. The boolean expression can evaluate the Tcl
 * variable 'resp_val' to query the value of the current
 * response. Each boolean expression is tested in order, and the only
 * the first expression which evaluates to true will have its
 * associated feedback script executed. This means that a "catch-all"
 * feedback script can be defined by using an unconditionally true
 * value for the last expression. For example, the following feedback map
 *
 * <pre>
 { {{ $resp_val == [Block::currentTrialType 0] } { Sound::play $Sound::ok }} \
   {{ 1 } { Sound::play $Sound::err }} }</pre>
 *
 * will play the Sound::ok sound if the current response matches the
 * current trial type. Otherwise, it will play the Sound::err sound.
 *
 * @memo Handles responses by using Tk event bindings. 
 **/

class EventResponseHdlr : public ResponseHandler {
public:
  /// Default constructor.
  EventResponseHdlr(const string& input_response_map="");
  ///
  virtual ~EventResponseHdlr();

  ///
  virtual void serialize(ostream &os, IOFlag flag) const;
  ///
  virtual void deserialize(istream &is, IOFlag flag);
  ///
  virtual int charCount() const;

  ///
  virtual void readFrom(Reader* reader);
  ///
  virtual void writeTo(Writer* writer) const;

  // manipulators/accessors

  ///
  virtual void setInterp(Tcl_Interp* interp);

  ///
  const string& getInputResponseMap() const;
  ///
  void setInputResponseMap(const string& s);

  ///
  bool getUseFeedback() const;
  ///
  void setUseFeedback(bool val);

  ///
  const char* getFeedbackMap() const;
  ///
  void setFeedbackMap(const char* feedback_string);

  ///
  const string& getEventSequence() const;
  ///
  void setEventSequence(const string& seq);

  ///
  const string& getBindingSubstitution() const;
  ///
  void setBindingSubstitution(const string& sub);

  /** @name   Expt event sequence actions  */
  //@{
  ///
  virtual void rhBeginTrial(Experiment* expt) const;
  ///
  virtual void rhAbortTrial(Experiment* expt) const;
  ///
  virtual void rhEndTrial(Experiment* expt) const;
  ///
  virtual void rhHaltExpt(Experiment* expt) const;
  //@}

protected:
  /** @name  Old IO functions  
	*
	* These deprecated functions are provided for compatibility with
	* old KbdResponseHdlr interface--these functions ignore the
	* eventSequence and bindingSubstitution fields
	*
	**/
  //@{
  ///
  void oldSerialize(ostream &os, IOFlag flag) const;
  ///
  void oldDeserialize(istream &is, IOFlag flag);
  ///
  int oldCharCount() const;
  //@}

private:
  class Impl;
  friend class EventResponseHdlr::Impl;
  Impl* const itsImpl;
};


static const char vcid_eventresponsehdlr_h[] = "$Header$";
#endif // !EVENTRESPONSEHDLR_H_DEFINED
