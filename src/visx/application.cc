///////////////////////////////////////////////////////////////////////
//
// application.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  7 11:05:52 1999
// written: Sun Aug 26 08:35:17 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_CC_DEFINED
#define APPLICATION_CC_DEFINED

#include "application.h"

#include "system/system.h"

#include "util/error.h"
#include "util/strings.h"

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

namespace
{
  Application* theSingleton = 0;
}

class Application::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(int argc, char** argv, const char* library_env_var) :
    itsArgc(argc),
    itsArgv(argv),
    itsLibraryDir(library_env_var == 0 ?
                  0 : System::theSystem().getenv(library_env_var))
  {
    DebugEvalNL((void*)library_env_var);
    DebugEvalNL(library_env_var);
    DebugEvalNL(itsLibraryDir);
    DebugEvalNL(itsLibraryDir.c_str());
  }

  int itsArgc;
  char** itsArgv;
  fstring itsLibraryDir;
};

Application::Application(int argc, char** argv, const char* library_env_var) :
  itsImpl(new Impl(argc, argv, library_env_var))
{
DOTRACE("Application::Application");
}

Application::~Application()
{
DOTRACE("Application::~Application");
  delete itsImpl;
}

void Application::installApp(Application* theApp)
{
DOTRACE("Application::installApp");
  if (theSingleton == 0 && theApp != 0)
    {
      theSingleton = theApp;
    }
}

Application& Application::theApp()
{
DOTRACE("Application::theApp");
  if (theSingleton == 0)
    {
      throw Util::Error("the application has not yet been installed");
    }
  return *theSingleton;
}

int Application::argc() const
{
DOTRACE("Application::argv");
  return itsImpl->itsArgc;
}

char** Application::argv() const
{
DOTRACE("Application::argv");
  return itsImpl->itsArgv;
}

const char* Application::getLibraryDirectory() const
{
DOTRACE("Application::getLibraryDirectory");
  return itsImpl->itsLibraryDir.c_str();
}

static const char vcid_application_cc[] = "$Header$";
#endif // !APPLICATION_CC_DEFINED
