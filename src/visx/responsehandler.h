///////////////////////////////////////////////////////////////////////
//
// responsehandler.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 18 16:21:09 1999
// written: Wed Sep 25 19:03:15 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSEHANDLER_H_DEFINED
#define RESPONSEHANDLER_H_DEFINED

#include "io/io.h"

namespace GWT
{
  class Widget;
}

namespace Util
{
  class ErrorHandler;
  template <class T> class SoftRef;
};

class Block;
class TrialBase;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c ResponseHandler defines an abstract interface for collecting
 * responses from a user during an experiment. However, it says
 * nothing about which modality will provide those responses. The key
 * virtual functions are the actions \c rhXxx(); these are called by
 * \c ExptDriver at appropriate points in the trial sequence.
 *
 **/
///////////////////////////////////////////////////////////////////////

class ResponseHandler : public IO::IoObject
{
public:
  /// Default constructor.
  ResponseHandler();

  /// Virtual destructor.
  virtual ~ResponseHandler();

  virtual void readFrom(IO::Reader* reader) = 0;
  virtual void writeTo(IO::Writer* writer) const = 0;

  /** Will be called by a \c TrialBase at the beginning of a trial. \c
      ResponseHandler subclasses implement this function to prepare
      listening for responses, etc. */
  virtual void rhBeginTrial(Util::SoftRef<GWT::Widget> widget,
                            TrialBase& trial) const = 0;

  /** Will by called by a \c TrialBase if a trial is aborted. \c
      ResponseHandler subclasses implement this function to quit
      listening for responses, etc. */
  virtual void rhAbortTrial() const = 0;

  /** Will by called by a \c TrialBase when a trial is complete. \c
      ResponseHandler subclasses implement this function to quit
      listening for responses, etc. */
  virtual void rhEndTrial() const = 0;

  /** Will by called by a \c TrialBase when the sequence of trials is
      halted suddenly. \c ResponseHandler subclasses implement this
      function to quit listening for responses, etc. */
  virtual void rhHaltExpt() const = 0;

  /// Subclasses implement this function to attend to responses.
  virtual void rhAllowResponses(Util::SoftRef<GWT::Widget> widget,
                                TrialBase& trial) const = 0;

  /// Subclasses implement this function to stop attending to responses.
  virtual void rhDenyResponses() const = 0;

private:
  ResponseHandler(const ResponseHandler&);
  ResponseHandler& operator=(const ResponseHandler&);
};

static const char vcid_responsehandler_h[] = "$Header$";
#endif // !RESPONSEHANDLER_H_DEFINED
