///////////////////////////////////////////////////////////////////////
//
// log.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 20 17:47:13 2001
// written: Fri Jan 25 10:48:19 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LOG_H_DEFINED
#define LOG_H_DEFINED

class fstring;

namespace Util
{
  void log(const char* msg);
  void log(const fstring& msg);
}

static const char vcid_log_h[] = "$Header$";
#endif // !LOG_H_DEFINED
