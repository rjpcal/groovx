///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:09:11 1999
// written: Fri Sep 29 14:49:22 2000
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
public:
  // creators
  KbdResponseHdlr();
  KbdResponseHdlr(const char* key_resp_pairs);
  virtual ~KbdResponseHdlr();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

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

private:
  void legacySrlz(IO::LegacyWriter* writer) const;
  void legacyDesrlz(IO::LegacyReader* reader);
};

static const char vcid_kbdresponsehdlr_h[] = "$Header$";
#endif // !KBDRESPONSEHDLR_H_DEFINED
