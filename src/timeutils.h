///////////////////////////////////////////////////////////////////////
//
// timeutils.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 13:31:07 1999
// written: Sat Jun 26 13:39:58 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMEUTILS_H_DEFINED
#define TIMEUTILS_H_DEFINED

#ifndef TIME_H_DEFINED
#include <sys/time.h>
#define TIME_H_DEFINED
#endif

namespace {
  int elapsedMsecSince(timeval beginTime) {
	 timeval endTime, elapsedTime;
	 
	 // Get ending time from previous trial
	 gettimeofday(&endTime, NULL);
	 
	 // Compute elapsed time for previous trial
	 elapsedTime.tv_sec = endTime.tv_sec - beginTime.tv_sec;
	 elapsedTime.tv_usec = endTime.tv_usec - beginTime.tv_usec;
	 
	 // XXX Need sanity check on overwrapped times

	 return int(double(elapsedTime.tv_sec)*1000.0 +
					double(elapsedTime.tv_usec)/1000.0);
  }
}

static const char vcid_timeutils_h[] = "$Header$";
#endif // !TIMEUTILS_H_DEFINED
