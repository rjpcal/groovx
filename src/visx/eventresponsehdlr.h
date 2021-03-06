/** @file visx/eventresponsehdlr.h collects and processes user
    responses during a psychophysics experiment */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Nov  9 15:30:54 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_VISX_EVENTRESPONSEHDLR_H_UTC20050626084015_DEFINED
#define GROOVX_VISX_EVENTRESPONSEHDLR_H_UTC20050626084015_DEFINED

#include "rutz/tracer.h"

#include "visx/responsehandler.h"

namespace rutz
{
  class fstring;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * EventResponseHandler provides a generic way to collect responses and
 * give feedback. It provides several hooks for Tcl code to define how
 * responses are interpreted, and how feedback is given. Clients should
 * select an event sequence of interest (for example, "\<KeyPress\>"), then
 * select an appropriate Tk binding substitution that will extract the
 * relevant event field (for example, "%K" gets the keycode from a KeyPress
 * event).
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

class EventResponseHdlr : public ResponseHandler
{
public:
  /** This tracer dynamically controls the tracing of \c
      EventResponseHdlr member functions. */
  static rutz::tracer tracer;

protected:
  /// Default constructor.
  EventResponseHdlr();

public:
  /// Default creator.
  static EventResponseHdlr* make();

  /// Virtual destructor.
  virtual ~EventResponseHdlr() noexcept;

  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  ////////////////////////////
  // manipulators/accessors //
  ////////////////////////////

  /// Use \a responseMap as the current input response map.
  void setInputResponseMap(const rutz::fstring& responseMap);

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
  const rutz::fstring& getEventSequence() const;

  /** Use \a seq as the binding sequence to be attended for
      responses. The event sequence should be specified in the format
      used in a Tk bind command. */
  void setEventSequence(const rutz::fstring& seq);

  /** Returns the current binding substitution. This will be in the
      format used in a Tk binding script (i.e. a percent sign
      followed by the appropriate character). */
  const rutz::fstring& getBindingSubstitution() const;

  /** Use \a sub as the current binding substitution. This must be in
      the format used in a Tk binding script (i.e. a percent sign
      followed by the appropriate character). When a response matching
      the event sequence specified by \c setEventSequence() is seen,
      the binding substitution will be evaluated, and checked for
      matches in the input response map to determine the integer
      response value. */
  void setBindingSubstitution(const rutz::fstring& sub);

  /** Returns the args and body of the Tcl code chunk that will be used to
      decode responses. */
  rutz::fstring getResponseProc() const;

  /** Specify a Tcl proc-style code chunk that will translate its args (which
      are the results of the binding substitution(s)) into an integer result
      that can be stored in a Response. */
  void setResponseProc(const rutz::fstring& args, const rutz::fstring& body);

  /** Causes the response handler to abort the trial if an invalid
      responses is seen. This is the default behavior. */
  void abortInvalidResponses();

  /** Causes the response handler to ignore invalid responses as if
      they did not happen. */
  void ignoreInvalidResponses();

  /** Set the maximum number of responses that will be allowed per activation
      cycle (the default is 1). After that number of responses has been
      recorded, the response handler will become inactive until explicitly
      reactivated. */
  void setMaxResponses(unsigned int count);

  /** Query the maximum number of responses allowed per activation cycle. */
  unsigned int getMaxResponses() const;

  /////////////
  // Actions //
  /////////////

  virtual void rhBeginTrial(nub::soft_ref<Toglet> widget, Trial& trial) const override;
  virtual void rhAbortTrial() const override;
  virtual void rhEndTrial() const override;
  virtual void rhHaltExpt() const override;
  virtual void rhAllowResponses(nub::soft_ref<Toglet> widget,
                                Trial& trial) const override;
  virtual void rhDenyResponses() const override;

private:
  EventResponseHdlr(const EventResponseHdlr&);
  EventResponseHdlr& operator=(const EventResponseHdlr&);

  class Impl;
  Impl* const rep;
};


#endif // !GROOVX_VISX_EVENTRESPONSEHDLR_H_UTC20050626084015_DEFINED
