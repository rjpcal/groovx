///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:09:11 1999
// written: Tue Nov  9 16:44:19 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_H_DEFINED
#define KBDRESPONSEHDLR_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef EVENTRESPONSEHDLR_H_DEFINED
#include "eventresponsehdlr.h"
#endif


///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr class defintion
//
///////////////////////////////////////////////////////////////////////

class KbdResponseHdlr : public EventResponseHdlr {
public:
  // creators
  KbdResponseHdlr(const string& s="");

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  const string& getKeyRespPairs() const
	 { return EventResponseHdlr::getInputResponseMap(); }
  void setKeyRespPairs(const string& s)
	 { EventResponseHdlr::setInputResponseMap(s); }

  bool getUseFeedback() const
	 { return EventResponseHdlr::getUseFeedback(); }
  void setUseFeedback(bool val)
	 { EventResponseHdlr::setUseFeedback(val); }

  const char* getFeedbackPairs() const
	 { return EventResponseHdlr::getFeedbackMap(); }
  void setFeedbackPairs(const char* feedback_string)
	 { EventResponseHdlr::setFeedbackMap(feedback_string); }
};

static const char vcid_kbdresponsehdlr_h[] = "$Header$";
#endif // !KBDRESPONSEHDLR_H_DEFINED
