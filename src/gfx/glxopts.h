/** @file gfx/glxopts.h options for controlling the
    OpenGL-windowsystem interface */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Sun Aug  4 16:28:37 2002
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_GFX_GLXOPTS_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GLXOPTS_H_UTC20050626084024_DEFINED

/// Setup options for controlling the OpenGL-windowsystem interface.
struct GlxOpts
{
  /// Construct with default values.
  GlxOpts();

  bool rgbaFlag;         ///< whether to use RGBA-mode
   int rgbaRed;          ///< red bit-depth in RGBA-mode
   int rgbaGreen;        ///< green bit-depth in RGBA-mode
   int rgbaBlue;         ///< blue bit-depth in RGBA-mode
  bool alphaFlag;        ///< whether to request an alpha layer
   int alphaSize;        ///< alpha bit-depth in RGBA-mode
   int colorIndexSize;   ///< bit-depth in color-index mode
  bool doubleFlag;       ///< whether to request double-buffering
  bool depthFlag;        ///< whether to request a depth buffer
   int depthSize;        ///< bit-depth of depth buffer
  bool accumFlag;        ///< whether to request an accum buffer
   int accumRed;         ///< red bit-depth of accum buffer
   int accumGreen;       ///< green bit-depth of accum buffer
   int accumBlue;        ///< blue bit-depth of accum buffer
   int accumAlpha;       ///< alpha bit-depth of accum buffer
  bool stencilFlag;      ///< whether to request a stencil buffer
   int stencilSize;      ///< bit-depth of stencil buffer
   int auxNumber;        ///< number of aux buffers to request
  bool indirect;         ///< whether to request an indirect rendering context
   int level;            ///< stacking level of GL context
  bool transparent;      ///< whether to request transparency
};

#endif // !GROOVX_GFX_GLXOPTS_H_UTC20050626084024_DEFINED
