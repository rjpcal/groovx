///////////////////////////////////////////////////////////////////////
//
// widget.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Dec  4 12:52:59 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_CC_DEFINED
#define WIDGET_CC_DEFINED

#include "gwt/widget.h"

#include "util/trace.h"

GWT::Widget::Widget()
{
DOTRACE("GWT::Widget::Widget");
}

GWT::Widget::~Widget() throw()
{
DOTRACE("GWT::Widget::~Widget");
}

static const char vcid_widget_cc[] = "$Header$";
#endif // !WIDGET_CC_DEFINED
