///////////////////////////////////////////////////////////////////////
//
// application.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 11:05:52 1999
// written: Tue Sep 19 17:14:23 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_CC_DEFINED
#define APPLICATION_CC_DEFINED

#include "application.h"

#include "system/system.h"
#include "util/strings.h"

#include "util/trace.h"

namespace {
  Application* theSingleton = 0;
}

NoAppError::NoAppError() : ErrorWithMsg() {}

NoAppError::NoAppError(const char* msg) : ErrorWithMsg(msg) {}

NoAppError::~NoAppError() {}

class Application::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(const char* library_dir) : itsLibraryDir(library_dir) {}

  fixed_string itsLibraryDir;
};

Application::Application(const char* library_env_var) :
  itsImpl(new Impl(System::theSystem().getenv(library_env_var)))
{
DOTRACE("Application::Application");
}

Application::~Application() {
DOTRACE("Application::~Application");
  delete itsImpl; 
}

void Application::installApp(Application* theApp) {
DOTRACE("Application::installApp");
  if (theSingleton == 0 && theApp != 0) {
	 theSingleton = theApp;
  }
}

Application& Application::theApp() {
DOTRACE("Application::theApp");
  if (theSingleton == 0) {
	 throw NoAppError("the application has not yet been installed");
  }
  return *theSingleton;
}

const char* Application::getLibraryDirectory() const {
DOTRACE("Application::getLibraryDirectory");
  return itsImpl->itsLibraryDir.c_str();
}

static const char vcid_application_cc[] = "$Header$";
#endif // !APPLICATION_CC_DEFINED
