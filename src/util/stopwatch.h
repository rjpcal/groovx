///////////////////////////////////////////////////////////////////////
//
// stopwatch.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 18 10:24:59 1999
// written: Mon Jun 11 15:08:14 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STOPWATCH_H_DEFINED
#define STOPWATCH_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TIME_H_DEFINED)
#include <sys/time.h>
#define TIME_H_DEFINED
#endif

class StopWatch {
public:
  StopWatch() : itsStartTime() { restart(); }

  void restart()
	 { gettimeofday(&itsStartTime, NULL); }

  int elapsedMsec()
	 {
		timeval endTime, elapsedTime;

		// Get ending time from previous trial
		gettimeofday(&endTime, NULL);

		// Compute elapsed time for previous trial
		elapsedTime.tv_sec = endTime.tv_sec - itsStartTime.tv_sec;
		elapsedTime.tv_usec = endTime.tv_usec - itsStartTime.tv_usec;

		return int(double(elapsedTime.tv_sec)*1000.0 +
					  double(elapsedTime.tv_usec)/1000.0);
	 }

private:
  timeval itsStartTime;
};

static const char vcid_stopwatch_h[] = "$Header$";
#endif // !STOPWATCH_H_DEFINED
