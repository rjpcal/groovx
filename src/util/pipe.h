///////////////////////////////////////////////////////////////////////
//
// pipe.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jan 14 17:33:24 2000
// written: Wed Mar 29 14:07:33 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PIPE_H_DEFINED
#define PIPE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CSTDIO_DEFINED)
#include <cstdio>
#define CSTDIO_DEFINED
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FSTREAM_H_DEFINED)
#include <fstream.h>
#define FSTREAM_H_DEFINED
#endif

namespace Util {
  class Pipe;
}

class Util::Pipe {
public:
  Pipe(const char* command, const char* mode) :
	 itsFile(popen(command, mode)),
	 itsStream(),
	 itsClosed(false),
	 itsExitStatus(0)
	 {
		if ( itsFile != 0 ) {
		  itsStream.attach(filedes());
		}
		else {
		  itsClosed = true;
		}
	 }

  ~Pipe()
	 { close(); }

  fstream& stream() { return itsStream; }

  int close()
	 {
		if ( !itsClosed ) {
		  itsStream.close();
		  itsExitStatus = pclose(itsFile);
		  itsClosed = true;
		}
		return itsExitStatus;
	 }

  bool isClosed() const { return itsClosed; }

  int exitStatus() const { return itsExitStatus; }

private:
  FILE* file() const { return itsFile; }
  int filedes() const { return fileno(itsFile); }

  Pipe(const Pipe&);
  Pipe& operator=(const Pipe&);

  FILE* itsFile;
  fstream itsStream;
  bool itsClosed;
  int itsExitStatus;
};


static const char vcid_pipe_h[] = "$Header$";
#endif // !PIPE_H_DEFINED
