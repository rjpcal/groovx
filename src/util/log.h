///////////////////////////////////////////////////////////////////////
//
// log.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 20 17:47:13 2001
// written: Wed Jun 20 18:24:47 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LOG_H_DEFINED
#define LOG_H_DEFINED

#ifdef PRESTANDARD_IOSTREAMS
class ostream;
#else
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#    include <iosfwd>
#    define IOSFWD_DEFINED
#  endif
#endif

namespace Util
{
  class Log {
    Log(const Log&);
    Log& operator=(const Log&);

    STD_IO::ostream& itsOs;
  public:
    Log(STD_IO::ostream& os) : itsOs(os) {}

    Log& operator<<(char n);
    Log& operator<<(const char* n);
    Log& operator<<(int n);
    Log& operator<<(unsigned int n);
    Log& operator<<(long n);
    Log& operator<<(unsigned long n);
    Log& operator<<(double d);
  };

  Log& log();
}

static const char vcid_log_h[] = "$Header$";
#endif // !LOG_H_DEFINED
