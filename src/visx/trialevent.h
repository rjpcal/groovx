///////////////////////////////////////////////////////////////////////
//
// trialevent.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 25 12:45:05 1999
// written: Sun Aug 26 08:35:12 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALEVENT_H_DEFINED
#define TRIALEVENT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STOPWATCH_H_DEFINED)
#include "util/stopwatch.h"
#endif

namespace Util
{
  class ErrorHandler;
};

class Experiment;
class TrialBase;

struct Tcl_TimerToken_;
typedef struct Tcl_TimerToken_* Tcl_TimerToken;
typedef void* ClientData;

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
  TrialEvent(int msec);

public:
  /// Destructor cancels any pending callback to \c invoke().
  virtual ~TrialEvent();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  /// Return the current requested delay time, in milliseconds.
  int getDelay() const { return itsRequestedDelay; }

  /// Set the requested delay time to \a msec milliseconds.
  void setDelay(int msec) { itsRequestedDelay = msec; }

  /// Queries whether there is a pending callback to \c invoke().
  bool isPending() const { return itsIsPending; }

  /** Schedules the event to occur after the requested delay, and
      cancels any previously pending events that had not yet
      triggered. After this call, \c invoke() will be called after the
      requested delay (to within the accuracy provided by the
      applications's event loop), unless there is an intervening call
      to \c cancel(). If the requested delay is negative or zero, the
      \c invoke() callback is triggered immediately without involving
      the event loop. */
  void schedule(TrialBase& trial, Util::ErrorHandler& errhdlr);

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
  virtual void invoke(TrialBase& trial) = 0;

private:
  static void dummyInvoke(ClientData clientData);
  void invokeTemplate();

  TrialEvent(const TrialEvent&);
  TrialEvent& operator=(const TrialEvent&);

  int itsRequestedDelay;
  Tcl_TimerToken itsToken;

  Util::ErrorHandler* itsErrorHandler;
  TrialBase* itsTrial;

  mutable bool itsIsPending;

  // Diagnostic stuff
  mutable StopWatch itsTimer;
  mutable int itsEstimatedOffset;
  mutable int itsActualRequest;
  mutable int itsTotalOffset;
  mutable int itsTotalError;
  mutable int itsInvokeCount;
};

///////////////////////////////////////////////////////////////////////
//
// TrialEvent derived classes defintions
//
///////////////////////////////////////////////////////////////////////

/// TrialEvent subclass to call TrialBase::trAbortTrial().
class AbortTrialEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  AbortTrialEvent(int msec = 0);
public:
  /// Default creator.
  static AbortTrialEvent* make() { return new AbortTrialEvent; }
  /// Virtual destructor.
  virtual ~AbortTrialEvent();
protected:
  virtual void invoke(TrialBase& trial);
};

/// TrialEvent subclass to call TrialBase::installSelf() and Widget::display().
class DrawEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  DrawEvent(int msec = 0);
public:
  /// Default creator.
  static DrawEvent* make() { return new DrawEvent; }
  /// Virtual destructor.
  virtual ~DrawEvent();
protected:
  virtual void invoke(TrialBase& trial);
};

/// TrialEvent subclass to call TrialBase::trEndTrial().
class EndTrialEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  EndTrialEvent(int msec = 0);
public:
  /// Default creator.
  static EndTrialEvent* make() { return new EndTrialEvent; }
  /// Virtual destructor.
  virtual ~EndTrialEvent();
protected:
  virtual void invoke(TrialBase& trial);
};

/// TrialEvent subclass to call TrialBase::trNextNode().
class NextNodeEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  NextNodeEvent(int msec = 0);
public:
  /// Default creator.
  static NextNodeEvent* make() { return new NextNodeEvent; }
  /// Virtual destructor.
  virtual ~NextNodeEvent();
protected:
  virtual void invoke(TrialBase& trial);
};

/// TrialEvent subclass to call TrialBase::trAllowResponses().
class AllowResponsesEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  AllowResponsesEvent(int msec = 0);
public:
  /// Default creator.
  static AllowResponsesEvent* make() { return new AllowResponsesEvent; }
  /// Virtual destructor.
  virtual ~AllowResponsesEvent();
protected:
  virtual void invoke(TrialBase& trial);
};

/// TrialEvent subclass to call TrialBase::trDenyResponses().
class DenyResponsesEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  DenyResponsesEvent(int msec = 0);
public:
  /// Default creator.
  static DenyResponsesEvent* make() { return new DenyResponsesEvent; }
  /// Virtual destructor.
  virtual ~DenyResponsesEvent();
protected:
  virtual void invoke(TrialBase& trial);
};

/// TrialEvent subclass to call Widget::undraw().
class UndrawEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  UndrawEvent(int msec = 0);
public:
  /// Default creator.
  static UndrawEvent* make() { return new UndrawEvent; }
  /// Virtual destructor.
  virtual ~UndrawEvent();
protected:
  virtual void invoke(TrialBase& trial);
};

/// TrialEvent subclass to call Canvas::drawOnBackBuffer().
class RenderBackEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  RenderBackEvent(int msec = 0);
public:
  /// Default creator.
  static RenderBackEvent* make() { return new RenderBackEvent; }
  /// Virtual destructor.
  virtual ~RenderBackEvent();
protected:
  virtual void invoke(TrialBase& trial);
};

/// TrialEvent subclass to call Canvas::drawOnFrontBuffer().
class RenderFrontEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  RenderFrontEvent(int msec = 0);
public:
  /// Default creator.
  static RenderFrontEvent* make() { return new RenderFrontEvent; }
  /// Virtual destructor.
  virtual ~RenderFrontEvent();
protected:
  virtual void invoke(TrialBase& trial);
};

/// TrialEvent subclass to call Widget::swapBuffers().
class SwapBuffersEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  SwapBuffersEvent(int msec = 0);
public:
  /// Default creator.
  static SwapBuffersEvent* make() { return new SwapBuffersEvent; }
  /// Virtual destructor.
  virtual ~SwapBuffersEvent();
protected:
  virtual void invoke(TrialBase& trial);
};

/// TrialEvent subclass to call Canvas::clearColorBuffer().
class ClearBufferEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  ClearBufferEvent(int msec = 0);
public:
  /// Default creator.
  static ClearBufferEvent* make() { return new ClearBufferEvent; }
  /// Virtual destructor.
  virtual ~ClearBufferEvent();
protected:
  virtual void invoke(TrialBase& trial);
};

static const char vcid_trialevent_h[] = "$Header$";
#endif // !TRIALEVENT_H_DEFINED
