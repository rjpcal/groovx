///////////////////////////////////////////////////////////////////////
//
// trialevent.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 25 12:45:05 1999
// written: Thu Dec 19 18:11:09 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALEVENT_H_DEFINED
#define TRIALEVENT_H_DEFINED

#include "io/io.h"

#include "tcl/tclprocwrapper.h"
#include "tcl/tcltimer.h"

#include "util/stopwatch.h"
#include "util/ref.h"

class Trial;

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
  virtual ~TrialEvent();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

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

///////////////////////////////////////////////////////////////////////
//
// TrialEvent derived classes defintions
//
///////////////////////////////////////////////////////////////////////

/// TrialEvent subclass to call Trial::trAbort().
class AbortTrialEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  AbortTrialEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static AbortTrialEvent* make() { return new AbortTrialEvent; }
  /// Virtual destructor.
  virtual ~AbortTrialEvent();
protected:
  virtual void invoke(Trial& trial);
};

/** TrialEvent subclass to call Trial::installSelf() and
    Widget::fullRender(). */
class DrawEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  DrawEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static DrawEvent* make() { return new DrawEvent; }
  /// Virtual destructor.
  virtual ~DrawEvent();
protected:
  virtual void invoke(Trial& trial);
};

/// TrialEvent subclass to call Widget::render().
class RenderEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  RenderEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static RenderEvent* make() { return new RenderEvent; }
  /// Virtual destructor.
  virtual ~RenderEvent();
protected:
  virtual void invoke(Trial& trial);
};

/// TrialEvent subclass to call Trial::trEndTrial().
class EndTrialEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  EndTrialEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static EndTrialEvent* make() { return new EndTrialEvent; }
  /// Virtual destructor.
  virtual ~EndTrialEvent();
protected:
  virtual void invoke(Trial& trial);
};

/// TrialEvent subclass to call Trial::trNextNode().
class NextNodeEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  NextNodeEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static NextNodeEvent* make() { return new NextNodeEvent; }
  /// Virtual destructor.
  virtual ~NextNodeEvent();
protected:
  virtual void invoke(Trial& trial);
};

/// TrialEvent subclass to call Trial::trAllowResponses().
class AllowResponsesEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  AllowResponsesEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static AllowResponsesEvent* make() { return new AllowResponsesEvent; }
  /// Virtual destructor.
  virtual ~AllowResponsesEvent();
protected:
  virtual void invoke(Trial& trial);
};

/// TrialEvent subclass to call Trial::trDenyResponses().
class DenyResponsesEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  DenyResponsesEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static DenyResponsesEvent* make() { return new DenyResponsesEvent; }
  /// Virtual destructor.
  virtual ~DenyResponsesEvent();
protected:
  virtual void invoke(Trial& trial);
};

/// TrialEvent subclass to call Widget::undraw().
class UndrawEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  UndrawEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static UndrawEvent* make() { return new UndrawEvent; }
  /// Virtual destructor.
  virtual ~UndrawEvent();
protected:
  virtual void invoke(Trial& trial);
};

/// TrialEvent subclass to call Canvas::drawOnBackBuffer().
class RenderBackEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  RenderBackEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static RenderBackEvent* make() { return new RenderBackEvent; }
  /// Virtual destructor.
  virtual ~RenderBackEvent();
protected:
  virtual void invoke(Trial& trial);
};

/// TrialEvent subclass to call Canvas::drawOnFrontBuffer().
class RenderFrontEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  RenderFrontEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static RenderFrontEvent* make() { return new RenderFrontEvent; }
  /// Virtual destructor.
  virtual ~RenderFrontEvent();
protected:
  virtual void invoke(Trial& trial);
};

/// TrialEvent subclass to call Widget::swapBuffers().
class SwapBuffersEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  SwapBuffersEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static SwapBuffersEvent* make() { return new SwapBuffersEvent; }
  /// Virtual destructor.
  virtual ~SwapBuffersEvent();
protected:
  virtual void invoke(Trial& trial);
};

/// TrialEvent subclass to call Canvas::clearColorBuffer().
class ClearBufferEvent : public TrialEvent
{
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  ClearBufferEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static ClearBufferEvent* make() { return new ClearBufferEvent; }
  /// Virtual destructor.
  virtual ~ClearBufferEvent();
protected:
  virtual void invoke(Trial& trial);
};

/// TrialEvent subclass to call Canvas::clearColorBuffer().
class GenericEvent : public TrialEvent
{
private:
  Util::Ref<Tcl::ProcWrapper> itsCallback;
protected:
  /// Construct with a requested delay of \a msec milliseconds.
  GenericEvent(unsigned int msec = 0);
public:
  /// Default creator.
  static GenericEvent* make() { return new GenericEvent; }
  /// Virtual destructor.
  virtual ~GenericEvent();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  fstring getCallback() const;
  void setCallback(const fstring& script);

protected:
  virtual void invoke(Trial& trial);
};

static const char vcid_trialevent_h[] = "$Header$";
#endif // !TRIALEVENT_H_DEFINED
