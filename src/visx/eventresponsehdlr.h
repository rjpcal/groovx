///////////////////////////////////////////////////////////////////////
//
// eventresponsehdlr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov  9 15:30:54 1999
// written: Wed Mar  8 08:10:20 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EVENTRESPONSEHDLR_H_DEFINED
#define EVENTRESPONSEHDLR_H_DEFINED

#ifndef RESPONSEHANDLER_H_DEFINED
#include "responsehandler.h"
#endif

class fixed_string;

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
 * @short Handles responses by using Tk event bindings. 
 **/
///////////////////////////////////////////////////////////////////////

class EventResponseHdlr : public ResponseHandler {
public:
  /// Default constructor.
  EventResponseHdlr();

  /// Construct with an initial input_response_map.
  EventResponseHdlr(const char* input_response_map);

  /// Virtual destructor.
  virtual ~EventResponseHdlr();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  // manipulators/accessors

  virtual void setInterp(Tcl_Interp* interp);

  /// Returns the current input response map.
  const fixed_string& getInputResponseMap() const;

  /// Use \a responseMap as the current input response map.
  void setInputResponseMap(const fixed_string& responseMap);

  /// Queries whether feedback will be given.
  bool getUseFeedback() const;

  /// Changes whether feedback will be given.
  void setUseFeedback(bool val);

  /// Returns the current feedback map. 
  const char* getFeedbackMap() const;

  /// Use \a feedbackMap as the current feedback map.
  void setFeedbackMap(const char* feedbackMap);

  /** Returns the event sequence which is currently attended for
      responses. The event sequence is specified in the format used
      in a Tk bind command. */
  const fixed_string& getEventSequence() const;

  /** Use \a seq as the binding sequence to be attended for
      responses. The event sequence should be specified in the format
      used in a Tk bind command. */
  void setEventSequence(const fixed_string& seq);

  /** Returns the current binding substitution. This will be in the
      format used in a Tk binding script (i.e. a percent sign
      followed by the appropriate character). */
  const fixed_string& getBindingSubstitution() const;

  /** Use \a sub as the current binding substitution. This must be in
      the format used in a Tk binding script (i.e. a percent sign
      followed by the appropriate character). When a response matching
      the event sequence specified by \c setEventSequence() is seen,
      the binding substitution will be evaluated, and checked for
      matches in the input response map to determine the integer
      response value. */
  void setBindingSubstitution(const fixed_string& sub);

  virtual void rhBeginTrial(Experiment* expt) const;
  virtual void rhAbortTrial(Experiment* expt) const;
  virtual void rhEndTrial(Experiment* expt) const;
  virtual void rhHaltExpt(Experiment* expt) const;

protected:
  /** This deprecated function is provided for compatibility with old
      \c KbdResponseHdlr interface, since the eventSequence and
      bindingSubstitution fields are ignored. */
  void oldSerialize(ostream &os, IOFlag flag) const;

  /** This deprecated function is provided for compatibility with old
      \c KbdResponseHdlr interface, since the eventSequence and
      bindingSubstitution fields are ignored. */
  void oldDeserialize(istream &is, IOFlag flag);

  /** This deprecated function is provided for compatibility with old
      \c KbdResponseHdlr interface, since the eventSequence and
      bindingSubstitution fields are ignored. */
  int oldCharCount() const;

private:
  class Impl;
  friend class EventResponseHdlr::Impl;
  Impl* const itsImpl;
};


static const char vcid_eventresponsehdlr_h[] = "$Header$";
#endif // !EVENTRESPONSEHDLR_H_DEFINED
