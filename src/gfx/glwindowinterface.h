///////////////////////////////////////////////////////////////////////
//
// glwindowinterface.h
//
// Copyright (c) 2004-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jul 19 18:42:10 2004
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

#ifndef GLWINDOWINTERFACE_H_DEFINED
#define GLWINDOWINTERFACE_H_DEFINED

class GlWindowInterface
{
public:
  /// Virtual destructor.
  virtual ~GlWindowInterface();

  /// Query whether rendering context has direct access to the hardware.
  virtual bool isDirect() const = 0;

  /// Query whether the rendering context is double-buffered.
  virtual bool isDoubleBuffered() const = 0;

  /// Get the bit depth of the draw buffer(s).
  virtual unsigned int bitsPerPixel() const = 0;

  /// Swaps buffers if in double-buffering mode.
  virtual void swapBuffers() const = 0;
};

static const char vcid_glwindowinterface_h[] = "$Header$";
#endif // !GLWINDOWINTERFACE_H_DEFINED
