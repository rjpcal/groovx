///////////////////////////////////////////////////////////////////////
//
// trialbase.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jan 25 18:41:19 2000
// written: Wed Dec  4 17:02:47 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALBASE_H_DEFINED
#define TRIALBASE_H_DEFINED

#include "visx/element.h"

namespace Gfx
{
  class Canvas;
}

namespace Util
{
  class ErrorHandler;
  template <class T> class SoftRef;
};

class Block;
class Response;
class Toglet;

///////////////////////////////////////////////////////////////////////
//
// TrialBase class definition
//
///////////////////////////////////////////////////////////////////////

class TrialBase : public Element
{
public:
  virtual ~TrialBase();

  // accessors
  virtual int lastResponse() const = 0;
  virtual Util::SoftRef<Toglet> getWidget() const = 0;

  // actions
  virtual double trElapsedMsec() = 0;

  virtual void trAbortTrial() = 0;
  virtual void trEndTrial() = 0;
  virtual void trNextNode() = 0;
  virtual void trNextTrial() = 0;
  virtual void trResponseSeen() = 0;
  virtual void trRecordResponse(Response& response) = 0;
  virtual void trAllowResponses() = 0;
  virtual void trDenyResponses() = 0;

  virtual void installSelf(Util::SoftRef<Toglet> widget) const = 0;
};

static const char vcid_trialbase_h[] = "$Header$";
#endif // !TRIALBASE_H_DEFINED
