///////////////////////////////////////////////////////////////////////
//
// experiment.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec  1 10:52:17 1999
// written: Wed May 10 14:37:50 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPERIMENT_H_DEFINED
#define EXPERIMENT_H_DEFINED

namespace IO { class IoObject; }

class Response;

namespace GWT {
  class Widget;
  class Canvas;
}

/// Protocol class that represents psychophysical experiments.
class Experiment {
public:
  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~Experiment();

  virtual GWT::Widget* getWidget() = 0;
  ///< Return the Widget in which the experiment is running.

  virtual GWT::Canvas* getCanvas() = 0;
  ///< Return the Canvas for the Widget in which the experiment is running.

  virtual void manageObject(const char* name, IO::IoObject* object) = 0;
  ///< Registers an object for the Experiment to handle for serialization.

  virtual void edDraw() = 0;
  ///< Draws the current trial onto the screen.

  virtual void edUndraw() = 0;
  ///< Undraws the current trial from the screen.

  virtual void edSwapBuffers() = 0;
  ///< Swaps the buffers in the Experiment's Widget.

  virtual void edBeginExpt() = 0;
  ///< Begin the experiment.

  virtual void edBeginTrial() = 0;
  ///< Begin the next trial.

  virtual void edProcessResponse(const Response& response) = 0;
  /**< Tell the Experiment to process a response.
	    @param response An experiment-specific value representing the response. */

  virtual void edAbortTrial() = 0;
  ///< Abort the current trial, and move on to the next trial.

  virtual void edEndTrial() = 0;
  ///< End the current trial normally, and move on to the next trial.

  virtual void edHaltExpt() const = 0;
  ///< Halt the experiment. No more trials will be begun.

  virtual void edResetExpt() = 0;
  /**< Reset the Experiment, restoring it to a state in which no
	    trials have been run. */

  virtual void edSetCurrentTrial(int trial) = 0;
  /**< Changes the current trial that should be redrawn in the case
       that a redraw or refresh event is sent to its Widget. */

  virtual void edRaiseBackgroundError(const char* msg) const = 0;
  ///< Generates a background error with the message \a msg.
};

static const char vcid_experiment_h[] = "$Header$";
#endif // !EXPERIMENT_H_DEFINED
