///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 18:09:11 1999
// written: Fri Nov 10 17:03:52 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KBDRESPONSEHDLR_H_DEFINED
#define KBDRESPONSEHDLR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(EVENTRESPONSEHDLR_H_DEFINED)
#include "eventresponsehdlr.h"
#endif


///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr class defintion
//
///////////////////////////////////////////////////////////////////////

class KbdResponseHdlr : public EventResponseHdlr {
  // creators
protected:
  KbdResponseHdlr();
  KbdResponseHdlr(const char* key_resp_pairs);
public:
  static KbdResponseHdlr* make();

  virtual ~KbdResponseHdlr();

  const fixed_string& getKeyRespPairs() const
	 { return EventResponseHdlr::getInputResponseMap(); }
  void setKeyRespPairs(const fixed_string& s)
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
