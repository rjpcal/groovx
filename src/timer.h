///////////////////////////////////////////////////////////////////////
// timer.h
// Rob Peters
// created: Thu Feb  4 23:09:20 1999
// written: Thu May 20 09:46:59 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TIMER_H_DEFINED
#define TIMER_H_DEFINED

#ifndef GROBJ_H_DEFINED
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
  int itsTime;                  // time in msec
};

static const char vcid_timer_h[] = "$Id$";
#endif // !TIMER_H_DEFINED
