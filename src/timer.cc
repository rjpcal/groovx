///////////////////////////////////////////////////////////////////////
// timer.cc
// Rob Peters
// created: Thu Feb  4 23:09:21 1999
// written: Tue Mar 16 19:26:01 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TIMER_CC_DEFINED
#define TIMER_CC_DEFINED

#include "timer.h"

#include <GL/gl.h>
#include <unistd.h>

Timer::Timer(int t) : itsTime(t) {
  grPostCompiled();
}

Timer::~Timer() {}

void Timer::grRecompile() const {}

void Timer::grAction() const {
  glFlush();
  usleep(itsTime * 1000);
}

void Timer::setTime(int msec) {
  itsTime = msec;
}

static const char vcid_timer_cc[] = "$Headf: timer.cc,v 1.3 1999/03/12 20:53:38 rjpeters Exp rjpeters $";
#endif // !TIMER_CC_DEFINED
