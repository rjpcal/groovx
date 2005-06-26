///////////////////////////////////////////////////////////////////////
//
// glwindowinterface.cc
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jul 19 18:42:28 2004
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GLWINDOWINTERFACE_CC_DEFINED
#define GLWINDOWINTERFACE_CC_DEFINED

#include "glwindowinterface.h"

#include "rutz/sharedptr.h"

#include "rutz/trace.h"

GlWindowInterface::~GlWindowInterface()
{
DOTRACE("GlWindowInterface::~GlWindowInterface");
}

#if defined(GL_PLATFORM_GLX)
#  include "gfx/glxwrapper.h"
rutz::shared_ptr<GlWindowInterface>
GlWindowInterface::make(Display* dpy, GlxOpts& opts)
{
DOTRACE("GlWindowInterface::make[glx]");
  return rutz::make_shared
    (GlxWrapper::make(dpy, opts, (GlxWrapper*)0 /*shared context*/));
}
#elif defined(GL_PLATFORM_AGL)
#  include "gfx/aglwrapper.h"
rutz::shared_ptr<GlWindowInterface>
GlWindowInterface::make(Display* dpy, GlxOpts& opts)
{
DOTRACE("GlWindowInterface::make[agl]");
  return rutz::make_shared(AglWrapper::make(dpy, opts));
}
#else
#  error no GL_PLATFORM macro defined!
#endif

static const char vcid_glwindowinterface_cc[] = "$Id$ $URL$";
#endif // !GLWINDOWINTERFACE_CC_DEFINED
