///////////////////////////////////////////////////////////////////////
//
// application.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  7 11:05:52 1999
// written: Wed Sep 11 14:37:14 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_CC_DEFINED
#define APPLICATION_CC_DEFINED

#include "visx/application.h"

#include "system/system.h"

#include "util/error.h"
#include "util/strings.h"

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  Application* theSingleton = 0;
  const char* GRSH_LIB_DIR = "GRSH_LIB_DIR";
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
                  0 : System::theSystem().getenv(library_env_var)),
    itsCanvas(0)
  {
    DebugEvalNL((void*)library_env_var);
    DebugEvalNL(library_env_var);
    DebugEvalNL(itsLibraryDir);
    DebugEvalNL(itsLibraryDir.c_str());
  }

  int itsArgc;
  char** itsArgv;
  fstring itsLibraryDir;
  Gfx::Canvas* itsCanvas;
};

Application::Application(int argc, char** argv) :
  itsImpl(new Impl(argc, argv, GRSH_LIB_DIR))
{
DOTRACE("Application::Application");
  installApp(this);
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

void Application::installCanvas(Gfx::Canvas& canvas)
{
DOTRACE("Application::installCanvas");
  itsImpl->itsCanvas = &canvas;
  Assert(itsImpl->itsCanvas != 0);
}

Gfx::Canvas& Application::getCanvas()
{
DOTRACE("Application::getCanvas");
  if (itsImpl->itsCanvas == 0)
    {
      throw Util::Error("no canvas has yet been installed");
    }
  return *(itsImpl->itsCanvas);
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
