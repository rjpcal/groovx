///////////////////////////////////////////////////////////////////////
//
// tracer.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Feb  8 17:48:17 2000
// written: Tue Jul  2 13:05:32 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACER_H_DEFINED
#define TRACER_H_DEFINED

namespace Util
{
  class Tracer;
}

/// A class for dynamic toggling sets of trace statements.
class Util::Tracer
{
public:
  Tracer() : itsStatus(false) {}

  void on()           { itsStatus = true; }
  void off()          { itsStatus = false; }
  void toggle()       { itsStatus = !itsStatus; }
  bool status() const { return itsStatus; }

private:
  bool itsStatus;
};

static const char vcid_tracer_h[] = "$Header$";
#endif // !TRACER_H_DEFINED
