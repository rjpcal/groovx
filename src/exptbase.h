///////////////////////////////////////////////////////////////////////
// expt.h
// Rob Peters
// created: Jan-99
// written: Fri Mar 12 12:58:28 1999
static const char vcid_expt_h[] = "$Id$";
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

  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const = 0;

  // accessors
  virtual int numTrials() const = 0;
  virtual int numCompleted() const = 0;
  virtual int currentTrial() const = 0;
  virtual int currentStimClass() const = 0;
  virtual int prevResponse() const = 0;
  virtual bool isComplete() const = 0;
  virtual const char* trialDescription() const = 0;

  // control functions
  virtual void beginTrial() = 0;
  virtual void recordResponse(int resp) = 0;
};

#endif // !EXPT_H_DEFINED
