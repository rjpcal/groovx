/** @file visx/trialevent.h events to be scheduled during a Trial */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jun 25 12:45:05 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_TRIALEVENT_H_UTC20050626084015_DEFINED
#define GROOVX_VISX_TRIALEVENT_H_UTC20050626084015_DEFINED

#include "io/io.h"

#include "nub/ref.h"
#include "nub/timer.h"

#include "tcl-io/tclprocwrapper.h"

#include <memory>
#include <vector>

namespace rutz
{
  template <class T> class fwd_iter;
}

namespace nub
{
  class scheduler;
}

class output_file;
class Trial;

//  #######################################################
//  =======================================================

/// \c TrialEvent represents events scheduled to occur during a \c Trial.

/** \c TrialEvent is an abstract class that models all events that can
    be scheduled to occur during a trial. Subclasses of \c TrialEvent
    must override \c invoke() to perform whatever specific action that
    type of event represents. The base class provides functionality to
    set the requested delay time, to \c schedule() an event to occur,
    and to \c cancel() a pending event. The timing accuracy with which
    \c invoke() is called will depend on the accuracy of the
    underlying system-provided event loop. */

class TrialEvent : public io::serializable
{
protected:
  /// Construct with a requested delay of \a msec.
  TrialEvent(unsigned int msec);

public:
  /// Destructor cancels any pending callback to \c invoke().
  virtual ~TrialEvent() noexcept;

  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  /// Return the current requested delay time, in milliseconds.
  unsigned int getDelay() const { return itsRequestedDelay; }

  /// Set the requested delay time to \a msec milliseconds.
  void setDelay(unsigned int msec) { itsRequestedDelay = msec; }

  /// Queries whether there is a pending callback to \c invoke().
  bool is_pending() const { return itsTimer.is_pending(); }

  /// Schedules the event to occur after the requested delay,
  /** Cancels any previously pending events that had not yet
      triggered. After this call, \c invoke() will be called after the
      requested delay (to within the accuracy provided by the
      applications's event loop), unless there is an intervening call
      to \c cancel(). If the requested delay is negative or zero, the
      \c invoke() callback is triggered immediately without involving
      the event loop. The function returns the actual delay that was
      requested from the event loop (this may differ from the ideal
      delay request since the TrialEvent will try to learn what the
      typical error is between ideal/actual delays, and then
      compensate accordingly). Finally, The minimum_msec parameter
      specifies a minimum delay time; this may be used to ensure that
      proper relative ordering of TrialEvent's is maintained, even if
      the event loop is getting slowed down overall.  */
  unsigned int schedule(std::shared_ptr<nub::scheduler> s,
                        Trial& trial,
                        unsigned int minimum_msec = 0);

  /// Cancels a pending event.
  /** That is, if \c cancel() is called after \c schedule() has been
      called but before \c invoke() has been triggered, then \c
      invoke() will not be called until the event is rescheduled. If
      there is no pending event, this function does nothing. */
  void cancel();

  /// Called after the requested delay after a call to \c schedule().
  /** Subclasses must override this function to take whatever specific
      action is desired when the callback triggers. The function is
      called internally by \c TrialEvent, so subclasses should not
      call this function directly. */
  virtual void invoke(Trial& trial) = 0;

private:
  void invokeTemplate();

  TrialEvent(const TrialEvent&);
  TrialEvent& operator=(const TrialEvent&);

  nub::timer itsTimer;
  unsigned int itsRequestedDelay;
  Trial* itsTrial;

  // Diagnostic stuff
  mutable double itsEstimatedOffset;
  mutable double itsTotalOffset;
  mutable double itsTotalError;
  mutable int itsInvokeCount;
};



//  #######################################################
//  =======================================================

/// TrialEvent subclass that does nothing in its invoke() function.
/** Why is this useful? It may be helpful in testing the runtime
    overhead involved in TrialEvent callbacks. */
class NullTrialEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  NullTrialEvent(unsigned int msec = 0);

  /// Virtual destructor.
  virtual ~NullTrialEvent() noexcept;

  virtual void invoke(Trial&) override;

public:
  /// Default creator.
  static NullTrialEvent* make() { return new NullTrialEvent; }
};

//  #######################################################
//  =======================================================

/// TrialEvent subclass that binds to an arbitrary Trial member function.
class TrialMemFuncEvent : public TrialEvent
{
public:
  typedef void (Trial::* CallbackType)();

  /// Overridden to return the typename that was passed to the constructor.
  virtual rutz::fstring obj_typename() const override;

  /// Make a new event bound to a given Trial member function.
  static TrialMemFuncEvent* make(CallbackType callback,
                                 const rutz::fstring& type,
                                 unsigned int msec = 0);

protected:
  /// Constructor.
  TrialMemFuncEvent(CallbackType callback,
                    const rutz::fstring& type,
                    unsigned int msec = 0);

  /// Virtual destructor.
  virtual ~TrialMemFuncEvent() noexcept;

  virtual void invoke(Trial& trial) override;

private:
  CallbackType itsCallback;
  rutz::fstring itsTypename;
};

//  #######################################################
//  =======================================================

//  Here's a set of creator functions that return different
//  TrialMemFuncEvent objects bound to different Trial member functions.

/// Return a TrialMemFuncEvent bound to Trial::trAbortTrial.
TrialEvent* makeAbortTrialEvent();
/// Return a TrialMemFuncEvent bound to Trial::trDraw.
TrialEvent* makeDrawEvent();
/// Return a TrialMemFuncEvent bound to Trial::trRender.
TrialEvent* makeRenderEvent();
/// Return a TrialMemFuncEvent bound to Trial::trEndTrial.
TrialEvent* makeEndTrialEvent();
/// Return a TrialMemFuncEvent bound to Trial::trNextNode.
TrialEvent* makeNextNodeEvent();
/// Return a TrialMemFuncEvent bound to Trial::trAllowResponses.
TrialEvent* makeAllowResponsesEvent();
/// Return a TrialMemFuncEvent bound to Trial::trDenyResponses.
TrialEvent* makeDenyResponsesEvent();
/// Return a TrialMemFuncEvent bound to Trial::trUndraw.
TrialEvent* makeUndrawEvent();
/// Return a TrialMemFuncEvent bound to Trial::trRenderBack.
TrialEvent* makeRenderBackEvent();
/// Return a TrialMemFuncEvent bound to Trial::trRenderFront.
TrialEvent* makeRenderFrontEvent();
/// Return a TrialMemFuncEvent bound to Trial::trSwapBuffers.
TrialEvent* makeSwapBuffersEvent();
/// Return a TrialMemFuncEvent bound to Trial::trClearBuffer.
TrialEvent* makeClearBufferEvent();
/// Return a TrialMemFuncEvent bound to Trial::trFinishDrawing.
TrialEvent* makeFinishDrawingEvent();


//  #######################################################
//  =======================================================

/// TrialEvent subclass to send a single byte to an output file.
class FileWriteEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  FileWriteEvent(unsigned int msec = 0);

  /// Virtual destructor.
  virtual ~FileWriteEvent() noexcept;

  virtual void invoke(Trial& trial) override;

public:
  /// Default creator.
  static FileWriteEvent* make() { return new FileWriteEvent; }

  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  /// Get the byte that will be written to the file.
  int getByte() const;

  /// Set the byte that will be written to the file.
  void setByte(int b);

  /// Get the file object that will be written to.
  nub::ref<output_file> getFile() const;

  /// Set the file object that will be written to.
  void setFile(nub::ref<output_file> file);

private:
  nub::ref<output_file> itsFile;
  int itsByte;
};

//  #######################################################
//  =======================================================

/// TrialEvent subclass to call an arbitrary piece of Tcl code.
class GenericEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  GenericEvent(unsigned int msec = 0);

  /// Virtual destructor.
  virtual ~GenericEvent() noexcept;

  virtual void invoke(Trial& trial) override;

public:
  /// Default creator.
  static GenericEvent* make() { return new GenericEvent; }

  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  /// Get a string containing the Tcl callback script.
  rutz::fstring getCallback() const;

  /// Set the Tcl callback script to be triggered.
  void setCallback(const rutz::fstring& script);

private:
  nub::ref<tcl::ProcWrapper> itsCallback;
};

//  #######################################################
//  =======================================================

/// MultiEvent allows multiple events to be linked together in sequence.
/** Compared with just scheduling multiple individual events, the
    advantage of MultiEvent is that it does not require a trip back
    into the event loop in between events. This allows for greater
    timing precision when events must be executed in precise
    sequence. */
class MultiEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  MultiEvent(unsigned int msec = 0);

  /// Virtual destructor.
  virtual ~MultiEvent() noexcept;

  virtual void invoke(Trial& trial) override;

public:
  /// Default creator.
  static MultiEvent* make() { return new MultiEvent; }

  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  /// Returns an iterator to all the events in the sequence.
  rutz::fwd_iter<const nub::ref<TrialEvent> > getEvents() const;

  /// Add a new event and return its index in the sequence.
  size_t addEvent(nub::ref<TrialEvent> event);

  /// Erase the event at the given index.
  void eraseEventAt(unsigned int index);

  /// Erase all events in the sequence.
  void clearEvents();

private:
  std::vector<nub::ref<TrialEvent> > itsEvents;
};

#endif // !GROOVX_VISX_TRIALEVENT_H_UTC20050626084015_DEFINED
