///////////////////////////////////////////////////////////////////////
//
// log.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 20 17:47:13 2001
// written: Mon Jan 13 11:08:25 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LOG_H_DEFINED
#define LOG_H_DEFINED

class fstring;

namespace Util
{
  namespace Log
  {
    void reset();

    void addScope(const fstring& name);
    void removeScope(const fstring& name);
  }

  void log(const char* msg);
  void log(const fstring& msg);
}

static const char vcid_log_h[] = "$Header$";
#endif // !LOG_H_DEFINED
