///////////////////////////////////////////////////////////////////////
//
// application.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 11:05:52 1999
// written: Thu Jun  1 13:41:51 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_CC_DEFINED
#define APPLICATION_CC_DEFINED

#include "application.h"

#include "util/trace.h"

namespace {
  Application* theSingleton = 0;
}

NoAppError::NoAppError() : ErrorWithMsg() {}

NoAppError::NoAppError(const char* msg) : ErrorWithMsg(msg) {}

NoAppError::~NoAppError() {}

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
