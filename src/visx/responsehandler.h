///////////////////////////////////////////////////////////////////////
//
// responsehandler.h
// Rob Peters
// created: Tue May 18 16:21:09 1999
// written: Thu Mar 30 09:50:01 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSEHANDLER_H_DEFINED
#define RESPONSEHANDLER_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

struct Tcl_Interp;

class Experiment;

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

class ResponseHandler : public virtual IO::IoObject {
public:
  /// Default constructor.
  ResponseHandler();

  /// Virtual destructor.
  virtual ~ResponseHandler();

  virtual void serialize(ostream& os, IO::IOFlag flag) const = 0;
  virtual void deserialize(istream& is, IO::IOFlag flag) = 0;
  virtual int charCount() const = 0;

  virtual void readFrom(IO::Reader* reader) = 0;
  virtual void writeTo(IO::Writer* writer) const = 0;

  /** This symbolic is used in place of an actual response value to
      indicate that the response was invalid or could not be
      determined. */
  static const int INVALID_RESPONSE = -1;

  /** Use \a interp as the Tcl interpreter for any Tcl-related
      processing that may be required in handling responses. */
  virtual void setInterp(Tcl_Interp* interp) = 0;

  /** Will by called by an \c Experiment at the beginning of a
      trial. \c ResponseHandler subclasses implement this function to
      prepare listening for responses, etc. */
  virtual void rhBeginTrial(Experiment* expt) const = 0;

  /** Will by called by an \c Experiment if a trial is aborted. \c
      ResponseHandler subclasses implement this function to quit
      listening for responses, etc. */
  virtual void rhAbortTrial(Experiment* expt) const = 0;

  /** Will by called by an \c Experiment when a trial is complete. \c
      ResponseHandler subclasses implement this function to quit
      listening for responses, etc. */
  virtual void rhEndTrial(Experiment* expt) const = 0;

  /** Will by called by an \c Experiment when the sequence of trials
      is halted suddenly. \c ResponseHandler subclasses implement this
      function to quit listening for responses, etc. */
  virtual void rhHaltExpt(Experiment* expt) const = 0;

private:
  ResponseHandler(const ResponseHandler&);
  ResponseHandler& operator=(const ResponseHandler&);
};

static const char vcid_responsehandler_h[] = "$Header$";
#endif // !RESPONSEHANDLER_H_DEFINED
