///////////////////////////////////////////////////////////////////////
// expt.h
// Rob Peters
// created: Jan-99
// written: Thu Apr 15 17:29:33 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef EXPT_H_DEFINED
#define EXPT_H_DEFINED

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

class Expt : public virtual IO {
public:
  Expt () {}
  virtual ~Expt() {}

  virtual void serialize(ostream &os, IOFlag flag = NO_FLAGS) const = 0;
  virtual void deserialize(istream &is, IOFlag flag = NO_FLAGS) = 0;

  // accessors
  virtual int numTrials() const = 0;
  virtual int numCompleted() const = 0;
  virtual int currentTrial() const = 0;
  virtual int currentStimClass() const = 0;
  virtual int prevResponse() const = 0;
  virtual bool isComplete() const = 0;
  virtual const char* trialDescription() const = 0;

  // actions
  virtual void beginTrial() = 0;
  virtual void abortTrial() = 0;
  virtual void recordResponse(int resp) = 0;
};

static const char vcid_expt_h[] = "$Header$";
#endif // !EXPT_H_DEFINED
