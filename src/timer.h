///////////////////////////////////////////////////////////////////////
// timer.h
// Rob Peters
// created: Thu Feb  4 23:09:20 1999
// written: Mon Mar  8 17:21:32 1999
///////////////////////////////////////////////////////////////////////

#ifndef TIMER_H_DEFINED
#define TIMER_H_DEFINED

#ifndef GROBJ_H_INCLUDED
#include "grobj.h"
#endif

class Timer : public GrObj {
public:
  Timer(int t=0);
  virtual ~Timer();

  virtual void grRecompile() const;

  virtual void grAction() const;

  void setTime(int msec);

private:
  int itsTime;						  // time in msec
};

#endif // !TIMER_H_DEFINED
