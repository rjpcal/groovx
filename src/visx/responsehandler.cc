/** @file visx/responsehandler.cc interface for getting user responses
    during a psychophysics experiment */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue May 18 16:21:09 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_RESPONSEHANDLER_CC_UTC20050626084017_DEFINED
#define GROOVX_VISX_RESPONSEHANDLER_CC_UTC20050626084017_DEFINED

#include "visx/responsehandler.h"

#include "rutz/trace.h"

///////////////////////////////////////////////////////////////////////
//
// ResponseHandler member definitions
//
///////////////////////////////////////////////////////////////////////

ResponseHandler::ResponseHandler()
{
GVX_TRACE("ResponseHandler::ResponseHandler");
}

ResponseHandler::~ResponseHandler() throw()
{
GVX_TRACE("ResponseHandler::~ResponseHandler");
}

static const char __attribute__((used)) vcid_groovx_visx_responsehandler_cc_utc20050626084017[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_RESPONSEHANDLER_CC_UTC20050626084017_DEFINED
