///////////////////////////////////////////////////////////////////////
//
// glxopts.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sun Aug  4 16:28:37 2002
// written: Sat Mar 29 12:46:03 2003
// $Id$
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

#ifndef GLXOPTS_H_DEFINED
#define GLXOPTS_H_DEFINED

class GlxAttribs;

template <class T> class shared_ptr;

/// Setup options for controlling the OpenGL-X11 interface.
struct GlxOpts
{
  GlxOpts() :
#ifndef NO_RGBA
    rgbaFlag( 1 ),
#else
    rgbaFlag( 0 ),
#endif
    rgbaRed( 1 ),
    rgbaGreen( 1 ),
    rgbaBlue( 1 ),
    colorIndexSize( 8 ),
#ifndef NO_DOUBLE_BUFFER
    doubleFlag( 1 ),
#else
    doubleFlag( 0 ),
#endif
    depthFlag( 1 ),
    depthSize( 8 ),
    accumFlag( 0 ),
    accumRed( 1 ),
    accumGreen( 1 ),
    accumBlue( 1 ),
    accumAlpha( 1 ),
    alphaFlag( 0 ),
    alphaSize( 1 ),
    stencilFlag( 0 ),
    stencilSize( 1 ),
    auxNumber( 0 ),
    indirect( 0 ),
    level( 0 ),
    transparent( 0 )
  {}

  int rgbaFlag;
  int rgbaRed;
  int rgbaGreen;
  int rgbaBlue;
  int colorIndexSize;
  int doubleFlag;
  int depthFlag;
  int depthSize;
  int accumFlag;
  int accumRed;
  int accumGreen;
  int accumBlue;
  int accumAlpha;
  int alphaFlag;
  int alphaSize;
  int stencilFlag;
  int stencilSize;
  int auxNumber;
  int indirect;
  int level;
  int transparent;

  shared_ptr<GlxAttribs> buildAttribList();
};

static const char vcid_glxopts_h[] = "$Header$";
#endif // !GLXOPTS_H_DEFINED
