///////////////////////////////////////////////////////////////////////
//
// trialevent.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 25 12:45:05 1999
// written: Thu Oct 21 18:14:51 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALEVENT_H_DEFINED
#define TRIALEVENT_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef TCL_H_DEFINED
#include <tcl.h>
#define TCL_H_DEFINED
#endif

#ifndef TIME_H_DEFINED
#include <sys/time.h>
#define TIME_H_DEFINED
#endif

class ExptDriver;

///////////////////////////////////////////////////////////////////////
//
// TrialEvent abstract class defintion
//
///////////////////////////////////////////////////////////////////////

class TrialEvent : public virtual IO {
public:
  TrialEvent(int msec);
  virtual ~TrialEvent();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  int getDelay() const { return itsRequestedDelay; }
  void setDelay(int msec) { itsRequestedDelay = msec; }

  void schedule();
  void cancel();

protected:
  virtual void invoke() = 0;

  ExptDriver& getExptDriver();
  
private:
  static void dummyInvoke(ClientData clientData);

  int itsRequestedDelay;
  Tcl_TimerToken itsToken;

  // Diagnostic stuff
  mutable timeval itsBeginTime;
  mutable int itsTotalError;
  mutable int itsTotalAbsError;
  mutable int itsInvokeCount;
};

///////////////////////////////////////////////////////////////////////
//
// TrialEvent derived classes defintions
//
///////////////////////////////////////////////////////////////////////

class AbortTrialEvent : public TrialEvent {
public:
  AbortTrialEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

class DrawEvent : public TrialEvent {
public:
  DrawEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

class EndTrialEvent : public TrialEvent {
public:
  EndTrialEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

class UndrawEvent : public TrialEvent {
public:
  UndrawEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

class RenderBackEvent : public TrialEvent {
public:
  RenderBackEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

class RenderFrontEvent : public TrialEvent {
public:
  RenderFrontEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

class SwapBuffersEvent : public TrialEvent {
public:
  SwapBuffersEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

class ClearBufferEvent : public TrialEvent {
public:
  ClearBufferEvent(int msec = 0) : TrialEvent(msec) {}
protected:
  virtual void invoke();
};

static const char vcid_trialevent_h[] = "$Header$";
#endif // !TRIALEVENT_H_DEFINED
