///////////////////////////////////////////////////////////////////////
//
// trialbase.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jan 25 18:41:19 2000
// written: Sat Jul 21 19:59:41 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALBASE_H_DEFINED
#define TRIALBASE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

namespace GWT
{
  class Canvas;
  class Widget;
}

namespace Util
{
  class ErrorHandler;
  template <class T> class WeakRef;
};

class Block;
class Response;

///////////////////////////////////////////////////////////////////////
//
// TrialBase class definition
//
///////////////////////////////////////////////////////////////////////

class TrialBase : public virtual IO::IoObject {
public:
  virtual ~TrialBase();

  // accessors
  virtual int trialType() const = 0;
  virtual int lastResponse() const = 0;
  virtual const char* description() const = 0;

  // manipulators
  virtual void undoLastResponse() = 0;

  // actions
  virtual void trDoTrial(Util::WeakRef<GWT::Widget> widget,
                         Util::ErrorHandler& errhdlr, Block& block) = 0;

  virtual int trElapsedMsec() = 0;

  virtual void trAbortTrial() = 0;
  virtual void trEndTrial() = 0;
  virtual void trNextNode() = 0;
  virtual void trNextTrial() = 0;
  virtual void trHaltExpt() = 0;
  virtual void trResponseSeen() = 0;
  virtual void trRecordResponse(Response& response) = 0;
  virtual void trAllowResponses() = 0;
  virtual void trDenyResponses() = 0;

  virtual void installSelf(Util::WeakRef<GWT::Widget> widget) const = 0;
};

static const char vcid_trialbase_h[] = "$Header$";
#endif // !TRIALBASE_H_DEFINED
