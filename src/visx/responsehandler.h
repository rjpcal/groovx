///////////////////////////////////////////////////////////////////////
//
// responsehandler.h
// Rob Peters
// created: Tue May 18 16:21:09 1999
// written: Wed Dec  1 14:36:21 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSEHANDLER_H_DEFINED
#define RESPONSEHANDLER_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

struct Tcl_Interp;

class Experiment;

///////////////////////////////////////////////////////////////////////
/**
 *
 * ResponseHandler provides an abstract interface for collecting
 * responses from a user during an experiment. However, it says
 * nothing about which modality will provide those responses. The key
 * virtual functions are the actions rhXxx(); these are called by
 * ExptDriver at appropriate points in the trial sequence.
 *
 * @memo Defines the interface for collecting responses from a user
 * during an experiment.
 **/

class ResponseHandler : public virtual IO {
public:
  /// Default constructor.
  ResponseHandler();
  ///
  virtual ~ResponseHandler();

  ///
  virtual void serialize(ostream& os, IOFlag flag) const = 0;
  ///
  virtual void deserialize(istream& is, IOFlag flag) = 0;

  ///
  virtual int charCount() const = 0;

  ///
  virtual void readFrom(Reader* reader) = 0;
  ///
  virtual void writeTo(Writer* writer) const = 0;

  ///
  static const int INVALID_RESPONSE = -1;

  ///
  virtual void setInterp(Tcl_Interp* interp) = 0;

  /** @name  Experiment event sequence actions
	*
	* These functions will be called by ExptDriver at appropriate
	* points in the experiment sequence.
	*
	**/
  //@{
  ///
  virtual void rhBeginTrial(Experiment* expt) const = 0;
  ///
  virtual void rhAbortTrial(Experiment* expt) const = 0;
  ///
  virtual void rhEndTrial(Experiment* expt) const = 0;
  ///
  virtual void rhHaltExpt(Experiment* expt) const = 0;
  //@}
};

static const char vcid_responsehandler_h[] = "$Header$";
#endif // !RESPONSEHANDLER_H_DEFINED
