///////////////////////////////////////////////////////////////////////
//
// tracer.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Feb  8 17:48:17 2000
// written: Wed Mar 19 12:45:34 2003
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
