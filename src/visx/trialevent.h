///////////////////////////////////////////////////////////////////////
//
// trialevent.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 25 12:45:05 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef TRIALEVENT_H_DEFINED
#define TRIALEVENT_H_DEFINED

#include "io/io.h"
#include "io/tclprocwrapper.h"

#include "tcl/tcltimer.h"

#include "util/stopwatch.h"
#include "util/ref.h"

#include <vector>

class OutputFile;
class Trial;

namespace Util
{
  template <class T> class FwdIter;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c TrialEvent is an abstract class that models all events that can
 * be scheduled to occur during a trial. Subclasses of \c TrialEvent
 * must override \c invoke() to perform whatever specific action that
 * type of event represents. The base class provides functionality to
 * set the requested delay time, to \c schedule() an event to occur,
 * and to \c cancel() a pending event. The timing accuracy with which
 * \c invoke() is called will depend on the accuracy of the underlying
 * system-provided event loop.
 *
 **/
///////////////////////////////////////////////////////////////////////

class TrialEvent : public IO::IoObject
{
protected:
  /// Construct with a requested delay of \a msec.
  TrialEvent(unsigned int msec);

public:
  /// Destructor cancels any pending callback to \c invoke().
  virtual ~TrialEvent() throw();

  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  /// Return the current requested delay time, in milliseconds.
  unsigned int getDelay() const { return itsRequestedDelay; }

  /// Set the requested delay time to \a msec milliseconds.
  void setDelay(unsigned int msec) { itsRequestedDelay = msec; }

  /// Queries whether there is a pending callback to \c invoke().
  bool isPending() const { return itsTimer.isPending(); }

  /** Schedules the event to occur after the requested delay, and cancels any
      previously pending events that had not yet triggered. After this call,
      \c invoke() will be called after the requested delay (to within the
      accuracy provided by the applications's event loop), unless there is an
      intervening call to \c cancel(). If the requested delay is negative or
      zero, the \c invoke() callback is triggered immediately without
      involving the event loop. The function returns the actual delay that was
      requested from the event loop (this may differ from the ideal delay
      request since the TrialEvent will try to learn what the typical error is
      between ideal/actual delays, and then compensate accordingly). Finally,
      The minimum_msec parameter specifies a minimum delay time; this may
      be used to ensure that proper relative ordering of TrialEvent's is
      maintained, even if the event loop is getting slowed down overall.  */
  unsigned int schedule(Trial& trial, unsigned int minimum_msec = 0);

  /** Cancels a pending event. That is, if \c cancel() is called after
      \c schedule() has been called but before \c invoke() has been
      triggered, then \c invoke() will not be called until the event
      is rescheduled. If there is no pending event, this function does
      nothing. */
  void cancel();

protected:
  /** This function will be called after the requested delay after a
      call to \c schedule(). Subclasses must override this function to
      take whatever specific action is desired when the callback
      triggers. The function is called internally by \c TrialEvent, so
      subclasses should not call this function directly. */
  virtual void invoke(Trial& trial) = 0;

private:
  void invokeTemplate();

  TrialEvent(const TrialEvent&);
  TrialEvent& operator=(const TrialEvent&);

  Tcl::Timer itsTimer;
  unsigned int itsRequestedDelay;
  Trial* itsTrial;

  // Diagnostic stuff
  mutable double itsEstimatedOffset;
  mutable double itsTotalOffset;
  mutable double itsTotalError;
  mutable int itsInvokeCount;
};



//  ###################################################################
//  ===================================================================

/// TrialEvent subclass that does nothing in its invoke() function.
/** Why is this useful? It may be helpful in testing the runtime overhead
    involved in TrialEvent callbacks. */
class NullTrialEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  NullTrialEvent(unsigned int msec = 0);

  /// Virtual destructor.
  virtual ~NullTrialEvent() throw();

  virtual void invoke(Trial&);

public:
  /// Default creator.
  static NullTrialEvent* make() { return new NullTrialEvent; }
};

//  ###################################################################
//  ===================================================================

/// TrialEvent subclass that just binds to an arbitrary Trial member function.
class TrialMemFuncEvent : public TrialEvent
{
public:
  typedef void (Trial::* CallbackType)();

  /// Overridden to return the typename that was passed to the constructor.
  virtual fstring objTypename() const;

  /// Make a new event bound to a given Trial member function.
  static TrialMemFuncEvent* make(CallbackType callback,
                                 const fstring& type,
                                 unsigned int msec = 0);

protected:
  /// Constructor.
  TrialMemFuncEvent(CallbackType callback,
                    const fstring& type,
                    unsigned int msec = 0);

  /// Virtual destructor.
  virtual ~TrialMemFuncEvent() throw();

  virtual void invoke(Trial& trial);

private:
  CallbackType itsCallback;
  fstring itsTypename;
};

//  ###################################################################
//  ===================================================================

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


//  ###################################################################
//  ===================================================================

/// TrialEvent subclass to send a single byte to an output file.
class FileWriteEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  FileWriteEvent(unsigned int msec = 0);

  /// Virtual destructor.
  virtual ~FileWriteEvent() throw();

  virtual void invoke(Trial& trial);

public:
  /// Default creator.
  static FileWriteEvent* make() { return new FileWriteEvent; }

  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  /// Get the byte that will be written to the file.
  int getByte() const;

  /// Set the byte that will be written to the file.
  void setByte(int b);

  /// Get the file object that will be written to.
  Util::Ref<OutputFile> getFile() const;

  /// Set the file object that will be written to.
  void setFile(Util::Ref<OutputFile> file);

private:
  Util::Ref<OutputFile> itsFile;
  int itsByte;
};

//  ###################################################################
//  ===================================================================

/// TrialEvent subclass to call an arbitrary piece of Tcl code.
class GenericEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  GenericEvent(unsigned int msec = 0);

  /// Virtual destructor.
  virtual ~GenericEvent() throw();

  virtual void invoke(Trial& trial);

public:
  /// Default creator.
  static GenericEvent* make() { return new GenericEvent; }

  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  /// Get a string containing the Tcl callback script.
  fstring getCallback() const;

  /// Set the Tcl callback script to be triggered.
  void setCallback(const fstring& script);

private:
  Util::Ref<Tcl::ProcWrapper> itsCallback;
};

//  ###################################################################
//  ===================================================================

/// MultiEvent allows multiple events to be linked together in sequence.
/** Compared with just scheduling multiple individual events, the advantage
    of MultiEvent is that it does not require a trip back into the event
    loop in between events. This allows for greater timing precision when
    events must be executed in precise sequence. */
class MultiEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  MultiEvent(unsigned int msec = 0);

  /// Virtual destructor.
  virtual ~MultiEvent() throw();

  virtual void invoke(Trial& trial);

public:
  /// Default creator.
  static MultiEvent* make() { return new MultiEvent; }

  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  /// Returns an iterator to all the events in the sequence.
  Util::FwdIter<const Util::Ref<TrialEvent> > getEvents() const;

  /// Add a new event and return its index in the sequence.
  unsigned int addEvent(Util::Ref<TrialEvent> event);

  /// Erase the event at the given index.
  void eraseEventAt(unsigned int index);

  /// Erase all events in the sequence.
  void clearEvents();

private:
  std::vector<Util::Ref<TrialEvent> > itsEvents;
};

static const char vcid_trialevent_h[] = "$Header$";
#endif // !TRIALEVENT_H_DEFINED
