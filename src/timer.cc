///////////////////////////////////////////////////////////////////////
// timer.cc
// Rob Peters
// created: Thu Feb  4 23:09:21 1999
// written: Mon Mar  8 17:21:16 1999
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

#endif // !TIMER_CC_DEFINED
