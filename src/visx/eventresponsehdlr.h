///////////////////////////////////////////////////////////////////////
//
// eventresponsehdlr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov  9 15:30:54 1999
// written: Tue Nov  9 15:57:25 1999
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
//
// EventResponseHdlr class definition
//
///////////////////////////////////////////////////////////////////////

class EventResponseHdlr : public ResponseHandler {
public:
  // creators
  EventResponseHdlr(const string& input_response_map="");
  virtual ~EventResponseHdlr();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  // manipulators/accessors
  virtual void setInterp(Tcl_Interp* interp);

  const string& getInputResponseMap() const;
  void setInputResponseMap(const string& s);

  bool getUseFeedback() const;
  void setUseFeedback(bool val);

  const char* getFeedbackMap() const;
  void setFeedbackMap(const char* feedback_string);

  const string& getEventSequence() const;
  void setEventSequence(const string& seq);

  const string& getBindingSubstitution() const;
  void setBindingSubstitution(const string& sub);

  // actions
  virtual void rhBeginTrial() const;
  virtual void rhAbortTrial() const;
  virtual void rhHaltExpt() const;

private:
  class Impl;
  friend class EventResponseHdlr::Impl;
  Impl* const itsImpl;
};


static const char vcid_eventresponsehdlr_h[] = "$Header$";
#endif // !EVENTRESPONSEHDLR_H_DEFINED
