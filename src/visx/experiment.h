///////////////////////////////////////////////////////////////////////
//
// experiment.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 10:52:17 1999
// written: Wed Dec  4 18:09:41 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPERIMENT_H_DEFINED
#define EXPERIMENT_H_DEFINED

#include "visx/element.h"

namespace Gfx
{
  class Canvas;
}

namespace Util
{
  class ErrorHandler;
  template <class T> class SoftRef;
};

class Response;
class Toglet;

/// Protocol class that represents psychophysical experiments.
class Experiment : public Element
{
public:
  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~Experiment();

  virtual Util::ErrorHandler& getErrorHandler() const = 0;
  ///< Return an \c ErrorHandler that can deal with error messages.

  virtual const Util::SoftRef<Toglet>& getWidget() const = 0;
  ///< Return the Widget in which the experiment is running.

  virtual Gfx::Canvas& getCanvas() const = 0;
  ///< Return the Canvas for the Widget in which the experiment is running.

  virtual void edBeginExpt() = 0;
  ///< Begin the experiment.

  virtual void edResumeExpt() = 0;
  ///< Resume the experiment after it has been halted.

  virtual void edClearExpt() = 0;
  ///< Reset the experiment to an empty state.

  virtual void edResetExpt() = 0;
  /**< Reset the Experiment, restoring it to a state in which no
       trials have been run. */
};

static const char vcid_experiment_h[] = "$Header$";
#endif // !EXPERIMENT_H_DEFINED
