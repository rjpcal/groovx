///////////////////////////////////////////////////////////////////////
// trialtiming.h
// Rob Peters
// created: Wed May  5 19:37:14 1999
// written: Sun Jun  6 19:43:07 1999
//
// This is right now a scratch file containing ideas for implementing
// a new event-based experiment design. The basic idea is that each
// Trial contains a pointer to a TrialTiming object. (This saves much
// space, since each TrialTiming object will likely be shared by many
// different Trial's). Each TrialTiming object contains a set of
// TrialEvent's. When it is a particular Trial's turn to go, the Expt
// requests a description of how the Trial would like to be timed, in
// a form of a set/list/vector of TrialEvent's. TrialEvent's may come
// in a variety of derived flavors, which make no difference to Expt,
// but which are process()'ed differently by Trial. The Expt's job is
// to facilitate the proper timing callback's etc with the OS, and
// when each event comes due, it passes the TrialEvent back to the
// Trial, which takes the appropriate action.
//
// Since both Trial's and TrialEvent's are polymorphic, proper
// communication between them requires double dispatch.
///////////////////////////////////////////////////////////////////////

#error // this file is not to be compiled

class TrialTiming {
public:
  TrialTiming();
  virtual ~TrialTiming();

  void addEvent(Event* e) { itsEvents.push_back(e); }
  void getEvents(list<TrialEvent>& events) { /* copy itsEvents into
																/* events */ }

private:
  list<TrialEvent> itsEvents;
};

// Example of building a TrialTiming object
{
  TrialTiming* example = new TrialTiming();
  example->addEvent(DrawTrialEvent(FROM_START, 0));
  example->addEvent(UndrawEvent(FROM_START, 2000));
  example->addEvent(AbortTrialEvent(FROM_START, 4000));
}

class Trial {
  void getEvents(list<TrialEvent>& events) {
	 itsTrialTiming->getEvents(events); 
  }
  
  virtual void process(TrialEvent* e) { 
	 // note the value of e->actual_time to see how it compares to
	 // e->requested_time
	 e->action(this);
  }
  virtual void draw();
  virtual void undraw();
  virtual void redraw();
  virtual void abort();
  virtual void recordResponse(int val, int msec /* ? */);
private:
  TrialTiming* itsTrialTiming;
};

enum TimeBase { IMMEDIATE, FROM_START, FROM_RESPONSE };

// TrialEvent's are probably small enough (four int's and a vptr) to
// pass around by value ???
class TrialEvent {
  TrialEvent(TimeBase tb = FROM_START, int req) : 
	 timeBase(tb),
	 requested_time(req) {}
  const TimeBase timeBase;
  const int requested_time;
  int actual_time;
  virtual void action(Trial* t) = 0;
};

// Derived classes of TrialEvent mainly work to dispatch messages back
// to the Trial in a way that depends on their (dynamic) type.

class DrawTrialEvent : public TrialEvent {
  virtual void action(Trial* t) { t->draw(); }
};

class UndrawEvent : public TrialEvent {
  virtual void action(Trial* t) { t->undraw(); }
};

class RedrawTrialEvent : public TrialEvent {
  virtual void action(Trial* t) { t->redraw(); }
};

class TrialResponseEvent : public TrialEvent {
  virtual void action(Trial* t) { t->recordResponse(response, response_time); }
  int response;
  int response_time; // do we need this, or should Trial calculate
							// this itself?
};

class AbortTrialEvent : public TrialEvent {
  virtual void action(Trial* t) { t->abort(); }
};

// This struct is passed as the client data to the callbacks that will
// be registered by an experiment. It must contain an Expt*, since the
// callbacks must be to free-standing/static functions, not member
// functions, and so they will need to know on which Expt to operate.
struct ExptEvent {
public:
  ExptEvent(Expt* expt, TrialEvent* event) : 
	 theExpt(expt), theTrialEvent(event) {}
  Expt* theExpt;
  TrialEvent* theTrialEvent;
};

void Expt::doTrial() {
  Trial* t = currentTrial();
  list<TrialEvent> events;
  t->getEvents(events);

  // Do all IMMEDIATE events
  for (list<TrialEvent>::iterator ii = events.begin(); 
		 ii != events.end(); ++ii) {
	 if (ii->timeBase == IMMEDIATE) {
		ii->action();
		/* pseudocode? */ events.remove(ii);
	 }
  }

  // Register all FROM_START events
  for (list<TrialEvent>::iterator ii = events.begin(); 
		 ii != events.end(); ++ii) {
	 if (ii->timeBase == FROM_START) {
		// Who is supposed to memory-manage the ExptEvent's???
		registerCallback(processEvent, 
							  static_cast<ClientData>(new ExptEvent(this, *ii)));
	 }
  }
}

void Expt::processResponse(ClientData clientData) {
  // cancel all FROM_START events
  Trial* t = currentTrial();
  // figure out response val, time, etc.
  t->recordResponse(val, time);
  // Register all/any FROM_RESPONSE events
}

// This **static** function processes ExptEvent's
void Expt::processEvent(ClientData clientData) {
  ExptEvent* eEvent = static_cast<ExptEvent *>(clientData);
  Expt* expt = eEvent->theExpt;
  Trial* t = expt->currentTrial();
  TrialEvent* tEvent = eEvent->theTrialEvent;
  // Get the actual time
  tEvent.actual_time = actual_time;
  t->process(tEvent);
}

