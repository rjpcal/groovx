///////////////////////////////////////////////////////////////////////
//
// application.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 10:55:51 1999
// written: Tue Dec  7 11:09:11 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_H_DEFINED
#define APPLICATION_H_DEFINED

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

class Experiment;


/** Exception class that is thrown from Application::theApp() if
    nobody has yet called Application::installApp(). */
class NoAppError : public ErrorWithMsg {
public:
  NoAppError(const string& msg = "") : ErrorWithMsg(msg) {}
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * Abstract base class that models an application invocation.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Application {
protected:
  /** When the concrete Application is created in a program, the
      Application should be installed with this function so that it is
      globally accessible via theApp(). */
  static void installApp(Application* theApp);

public:
  virtual ~Application();

  /** Provides global access to the singleton Application.
		@exception NoAppError installApp() has not yet been called.
		@return the Application passed to installApp. */
  static Application& theApp();

  /// Retrieves the experiment that is running in the application.
  virtual Experiment* getExperiment() = 0;

private:
  Application(const Application&);
  Application& operator=(const Application&);
};

static const char vcid_application_h[] = "$Header$";
#endif // !APPLICATION_H_DEFINED
