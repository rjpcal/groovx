///////////////////////////////////////////////////////////////////////
//
// trialevent.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 25 12:45:05 1999
// written: Wed Mar  8 16:51:43 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALEVENT_H_DEFINED
#define TRIALEVENT_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef STOPWATCH_H_DEFINED
#include "stopwatch.h"
#endif

struct Tcl_TimerToken_;
typedef struct Tcl_TimerToken_* Tcl_TimerToken;
typedef void* ClientData;

class Experiment;

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

class TrialEvent : public virtual IO {
public:
  /// Construct with a requested delay of \a msec.
  TrialEvent(int msec);

  /// Destructor cancels any pending callback to \c invoke().
  virtual ~TrialEvent();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

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
  void schedule(Experiment* expt);

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
  virtual void invoke() = 0;

  /** This returns the \c Experiment which was passed to the most
      recent call to schedule. \c TrialEventError is thrown if \c
      schedule() has never been called, or if the pointer passed most
      recently to \c schedule() was null. */
  Experiment& getExperiment();

private:
  static void dummyInvoke(ClientData clientData);

  int itsRequestedDelay;
  Tcl_TimerToken itsToken;

  Experiment* itsExperiment;

  mutable bool itsIsPending;

  // Diagnostic stuff
  mutable StopWatch itsTimer;
  mutable int itsTotalError;
  mutable int itsTotalAbsError;
  mutable int itsInvokeCount;
};

///////////////////////////////////////////////////////////////////////
//
// TrialEvent derived classes defintions
//
///////////////////////////////////////////////////////////////////////

/// TrialEvent subclass to call Experiment::edAbortTrial().
class AbortTrialEvent : public TrialEvent {
public:
  /// Construct with a requested delay of \a msec milliseconds.
  AbortTrialEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

/// TrialEvent subclass to call Experiment::edDraw().
class DrawEvent : public TrialEvent {
public:
  /// Construct with a requested delay of \a msec milliseconds.
  DrawEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

/// TrialEvent subclass to call Experiment::edEndTrial().
class EndTrialEvent : public TrialEvent {
public:
  /// Construct with a requested delay of \a msec milliseconds.
  EndTrialEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

/// TrialEvent subclass to call Experiment::edUndraw().
class UndrawEvent : public TrialEvent {
public:
  /// Construct with a requested delay of \a msec milliseconds.
  UndrawEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

/// TrialEvent subclass to call Canvas::drawOnBackBuffer().
class RenderBackEvent : public TrialEvent {
public:
  /// Construct with a requested delay of \a msec milliseconds.
  RenderBackEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

/// TrialEvent subclass to call Canvas::drawOnFrontBuffer().
class RenderFrontEvent : public TrialEvent {
public:
  /// Construct with a requested delay of \a msec milliseconds.
  RenderFrontEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

/// TrialEvent subclass to call Experiment::edSwapBuffers().
class SwapBuffersEvent : public TrialEvent {
public:
  /// Construct with a requested delay of \a msec milliseconds.
  SwapBuffersEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

/// TrialEvent subclass to call Canvas::clearColorBuffer().
class ClearBufferEvent : public TrialEvent {
public:
  /// Construct with a requested delay of \a msec milliseconds.
  ClearBufferEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

static const char vcid_trialevent_h[] = "$Header$";
#endif // !TRIALEVENT_H_DEFINED
