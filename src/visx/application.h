///////////////////////////////////////////////////////////////////////
//
// application.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  7 10:55:51 1999
// written: Fri Jan 18 16:06:54 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_H_DEFINED
#define APPLICATION_H_DEFINED

namespace Gfx
{
  class Canvas;
}


///////////////////////////////////////////////////////////////////////
/**
 *
 * Abstract base class that models an application invocation.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Application
{
protected:
  /// Default no-op constructor.
  Application(int argc, char** argv, const char* library_env_var = 0);

  /** When the concrete Application is created in a program, the
      Application should be installed with this function so that it is
      globally accessible via theApp(). */
  static void installApp(Application* theApp);

public:
  /// Virtual destructor ensures correct destruction of subclasses.
  virtual ~Application();

  /** Provides global access to the singleton Application.
      @exception Util::Error installApp() has not yet been called.
      @return the Application passed to installApp. */
  static Application& theApp();

  /// Retrieves the experiment that is running in the application.
  virtual Gfx::Canvas& getCanvas() = 0;

  int argc() const;

  char** argv() const;

  const char* getLibraryDirectory() const;

private:
  Application(const Application&);
  Application& operator=(const Application&);

  class Impl;
  friend class Impl;
  Impl* const itsImpl;
};

static const char vcid_application_h[] = "$Header$";
#endif // !APPLICATION_H_DEFINED
