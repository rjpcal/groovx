///////////////////////////////////////////////////////////////////////
//
// trialbase.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jan 25 18:41:19 2000
// written: Wed Dec  4 18:36:28 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALBASE_H_DEFINED
#define TRIALBASE_H_DEFINED

#include "visx/element.h"

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

  // actions
  virtual double trElapsedMsec() = 0;

  virtual void trNextNode() = 0;
  virtual void trNextTrial() = 0;
  virtual void trResponseSeen() = 0;
  virtual void trAllowResponses() = 0;
  virtual void trDenyResponses() = 0;

  virtual void installSelf(Util::SoftRef<Toglet> widget) const = 0;
};

static const char vcid_trialbase_h[] = "$Header$";
#endif // !TRIALBASE_H_DEFINED
