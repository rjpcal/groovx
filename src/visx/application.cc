///////////////////////////////////////////////////////////////////////
//
// application.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 11:05:52 1999
// written: Tue Dec  7 11:54:06 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_CC_DEFINED
#define APPLICATION_CC_DEFINED

#include "application.h"

#include "trace.h"

namespace {
  Application* theSingleton = 0;
}

Application::Application() 
{
DOTRACE("Application::Application");
}

void Application::installApp(Application* theApp) {
DOTRACE("Application::installApp");
  if (theSingleton == 0 && theApp != 0) {
	 theSingleton = theApp;
  }
}

Application::~Application() {}

Application& Application::theApp() {
DOTRACE("Application::theApp");
  if (theSingleton == 0) {
	 throw NoAppError("the application has not yet been installed");
  }
  return *theSingleton;
}

static const char vcid_application_cc[] = "$Header$";
#endif // !APPLICATION_CC_DEFINED
