///////////////////////////////////////////////////////////////////////
//
// gxvectorfont.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 13 20:05:23 2002
// written: Wed Mar 19 17:55:59 2003
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

#ifndef GXVECTORFONT_H_DEFINED
#define GXVECTORFONT_H_DEFINED

#include "gfx/gxfont.h"

class GxVectorFont : public GxFont
{
public:
  GxVectorFont();

  virtual ~GxVectorFont();

  virtual const char* fontName() const;

  virtual unsigned int listBase() const;

  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const;

private:
  GxVectorFont(const GxVectorFont&);
  GxVectorFont& operator=(const GxVectorFont&);
};

static const char vcid_gxvectorfont_h[] = "$Header$";
#endif // !GXVECTORFONT_H_DEFINED
