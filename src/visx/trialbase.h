///////////////////////////////////////////////////////////////////////
//
// trialbase.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jan 25 18:41:19 2000
// written: Wed Sep 25 19:03:47 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALBASE_H_DEFINED
#define TRIALBASE_H_DEFINED

#include "io/io.h"

namespace Gfx
{
  class Canvas;
}

namespace GWT
{
  class Widget;
}

namespace Util
{
  class ErrorHandler;
  template <class T> class SoftRef;
};

class Block;
class Response;

///////////////////////////////////////////////////////////////////////
//
// TrialBase class definition
//
///////////////////////////////////////////////////////////////////////

class TrialBase : public IO::IoObject
{
public:
  virtual ~TrialBase();

  // accessors
  virtual int trialType() const = 0;
  virtual int lastResponse() const = 0;
  virtual const char* description() const = 0;
  virtual Util::SoftRef<GWT::Widget> getWidget() const = 0;

  // manipulators
  virtual void undoLastResponse() = 0;

  // actions
  virtual void trDoTrial(Util::SoftRef<GWT::Widget> widget,
                         Util::ErrorHandler& errhdlr, Block& block) = 0;

  virtual double trElapsedMsec() = 0;

  virtual void trAbortTrial() = 0;
  virtual void trEndTrial() = 0;
  virtual void trNextNode() = 0;
  virtual void trNextTrial() = 0;
  virtual void trHaltExpt() = 0;
  virtual void trResponseSeen() = 0;
  virtual void trRecordResponse(Response& response) = 0;
  virtual void trAllowResponses() = 0;
  virtual void trDenyResponses() = 0;

  virtual void installSelf(Util::SoftRef<GWT::Widget> widget) const = 0;
};

static const char vcid_trialbase_h[] = "$Header$";
#endif // !TRIALBASE_H_DEFINED
