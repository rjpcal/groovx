///////////////////////////////////////////////////////////////////////
//
// pipe.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jan 14 17:33:24 2000
// written: Fri Nov 10 17:03:47 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PIPE_H_DEFINED
#define PIPE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CSTDIO_DEFINED)
#include <cstdio>
#define CSTDIO_DEFINED
#endif

#ifdef PRESTANDARD_IOSTREAMS
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FSTREAM_H_DEFINED)
#    include <fstream.h>
#    define FSTREAM_H_DEFINED
#  endif
#else
#  include <fstream>
#endif

namespace Util {
  class Pipe;
}

class Util::Pipe {
public:
  Pipe(const char* command, const char* mode) :
	 itsFile(popen(command, mode)),
#ifdef PRESTANDARD_IOSTREAMS
	 itsStream(),
#else
	 itsStream(&itsFilebuf),
#endif
	 itsClosed(false),
	 itsExitStatus(0)
	 {
		if ( itsFile != 0 ) {
#ifdef PRESTANDARD_IOSTREAMS
		  itsStream.attach(filedes());
#else
		  itsFilebuf.open(filedes());
#endif
		}
		else {
		  itsClosed = true;
		}
	 }

  ~Pipe()
	 { close(); }

  STD_IO::iostream& stream() { return itsStream; }

  int close()
	 {
		if ( !itsClosed ) {
#ifdef PRESTANDARD_IOSTREAMS
		  itsStream.close();
#endif
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
#ifdef PRESTANDARD_IOSTREAMS
  fstream itsStream;
#else
  STD_IO::iostream itsStream;
  std::filebuf itsFilebuf;
#endif
  bool itsClosed;
  int itsExitStatus;
};


static const char vcid_pipe_h[] = "$Header$";
#endif // !PIPE_H_DEFINED
