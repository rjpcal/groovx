///////////////////////////////////////////////////////////////////////
//
// experiment.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec  1 10:52:17 1999
// written: Wed Dec  1 11:14:42 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPERIMENT_H_DEFINED
#define EXPERIMENT_H_DEFINED

class Experiment {
public:
  static void installExperiment(Experiment* expt);
  static Experiment& getExperiment();

  virtual void draw() = 0;
  virtual void undraw() = 0;
  virtual void edSwapBuffers() = 0;

  virtual void edBeginExpt() = 0;
  virtual void edBeginTrial() = 0;
  virtual void edResponseSeen() = 0;
  virtual void edProcessResponse(int response) = 0;
  virtual void edAbortTrial() = 0;
  virtual void edEndTrial() = 0;
  virtual void edHaltExpt() const = 0;
  virtual void edResetExpt() = 0;
};

static const char vcid_experiment_h[] = "$Header$";
#endif // !EXPERIMENT_H_DEFINED
