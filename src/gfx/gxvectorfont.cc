///////////////////////////////////////////////////////////////////////
//
// gxvectorfont.cc
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 13 20:05:41 2002
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

#ifndef GXVECTORFONT_CC_DEFINED
#define GXVECTORFONT_CC_DEFINED

#include "gxvectorfont.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/rect.h"

#include "util/strings.h"

#include <cstring>
#include <GL/gl.h>

#include "util/debug.h"
DBG_REGISTER;
#include "util/trace.h"

namespace
{

//---------------------------------------------------------------------
//
// pointtype struct
//
//---------------------------------------------------------------------

  enum pointtype {PT, STROKE, END};
  struct CP
  {
    GLfloat x,y;
    pointtype type;
  };

//---------------------------------------------------------------------
//
// uppercase stroke data
//
//---------------------------------------------------------------------

  CP Adata[] =
  {
    {0, 0, PT}, {0, 3, PT}, {2, 6, PT}, {4, 3, PT}, {4, 0, STROKE},
    {0, 3, PT}, {4, 3, END}
  };

  CP Bdata[] =
  {
    {0, 0, PT}, {0, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 4, PT}, {3, 3, PT}, {4, 2, PT}, {4, 1, PT}, {3, 0, PT}, {0, 0, STROKE},
    {0, 3, PT}, {3, 3, END}
  };

  CP Cdata[] =
  {
    {4, 1, PT}, {3, 0, PT}, {1, 0, PT}, {0, 1, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, END}
  };

  CP Ddata[] =
  {
    {0, 0, PT}, {0, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 1, PT}, {3, 0, PT}, {0, 0, END}
  };

  CP Edata[] =
  {
    {4, 0, PT}, {0, 0, PT}, {0, 6, PT}, {4, 6, STROKE},
    {0, 3, PT}, {3, 3, END}
  };

  CP Fdata[] =
  {
    {0, 0, PT}, {0, 6, PT}, {4, 6, STROKE},
    {0, 3, PT}, {3, 3, END}
  };

  CP Gdata[] =
  {
    {2, 3, PT}, {4, 3, PT}, {4, 1, PT}, {3, 0, PT}, {1, 0, PT}, {0, 1, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, END}
  };

  CP Hdata[] =
  {
    {0, 0, PT}, {0, 6, STROKE},
    {4, 0, PT}, {4, 6, STROKE},
    {0, 3, PT}, {4, 3, END}
  };

  CP Idata[] =
  {
    {2, 6, PT}, {2, 0, STROKE},
    {0, 0, PT}, {4, 0, STROKE},
    {0, 6, PT}, {4, 6, END},
  };

  CP Jdata[] =
  {
    {0, 1, PT}, {1, 0, PT}, {3, 0, PT}, {4, 1, PT}, {4, 6, END}
  };

  CP Kdata[] =
  {
    {0, 0, PT}, {0, 6, STROKE},
    {4, 6, PT}, {0, 3, PT}, {4, 0, END}
  };

  CP Ldata[] =
  {
    {4, 0, PT}, {0, 0, PT}, {0, 6, END}
  };

  CP Mdata[] =
  {
    {0, 0, PT}, {0, 6, PT}, {2, 3, PT}, {4, 6, PT}, {4, 0, END}
  };

  CP Ndata[] =
  {
    {0, 0, PT}, {0, 6, PT}, {4, 0, PT}, {4, 6, END}
  };

  CP Odata[] =
  {
    {1, 0, PT}, {0, 1, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 1, PT}, {3, 0, PT}, {1, 0, END}
  };

  CP Pdata[] =
  {
    {0, 0, PT}, {0, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 4, PT}, {3, 3, PT}, {0, 3, END}
  };

  CP Qdata[] =
  {
    {1, 0, PT}, {0, 1, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 1, PT}, {3, 0, PT}, {1, 0, STROKE},
    {3, 1, PT}, {4, 0, END}
  };

  CP Rdata[] =
  {
    {0, 0, PT}, {0, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 4, PT}, {3, 3, PT}, {0, 3, STROKE},
    {1, 3, PT}, {4, 0, END}
  };

  CP Sdata[] =
  {
    {0, 1, PT}, {1, 0, PT}, {3, 0, PT}, {4, 1, PT}, {4, 2, PT}, {3, 3, PT}, {1, 3, PT}, {0, 4, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, END}
  };

  CP Tdata[] =
  {
    {2, 0, PT}, {2, 6, STROKE},
    {0, 6, PT}, {4, 6, END},
  };

  CP Udata[] =
  {
    {0, 6, PT}, {0, 1, PT}, {1, 0, PT}, {3, 0, PT}, {4, 1, PT}, {4, 6, END}
  };

  CP Vdata[] =
  {
    {0, 6, PT}, {2, 0, PT}, {4, 6, END}
  };

  CP Wdata[] =
  {
    {0, 6, PT}, {0, 0, PT}, {2, 3, PT}, {4, 0, PT}, {4, 6, END}
  };

  CP Xdata[] =
  {
    {0, 0, PT}, {4, 6, STROKE},
    {0, 6, PT}, {4, 0, END}
  };

  CP Ydata[] =
  {
    {0, 6, PT}, {2, 3, PT}, {4, 6, STROKE},
    {2, 3, PT}, {2, 0, END}
  };

  CP Zdata[] =
  {
    {4, 0, PT}, {0, 0, PT}, {4, 6, PT}, {0, 6, STROKE},
    {1, 3, PT}, {3, 3, END}
  };

//---------------------------------------------------------------------
//
// lowercase stroke data
//
//---------------------------------------------------------------------

  CP adata[] =
  {
    {1, 4, PT}, {3, 4, PT}, {4, 3, PT}, {4, 0, STROKE},
    {4, 3, PT}, {1, 3, PT}, {0, 2, PT}, {0, 1, PT}, {1, 0, PT}, {3, 0, PT}, {4, 1, END}
  };

  CP bdata[] =
  {
    {0, 0, PT}, {0, 6, STROKE},
    {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, PT}, {4, 1, PT}, {3, 0, PT}, {1, 0, PT}, {0, 1, END}
  };

  CP cdata[] =
  {
    {4, 1, PT}, {3, 0, PT}, {1, 0, PT}, {0, 1, PT}, {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, END}
  };

  CP ddata[] =
  {
    {4, 0, PT}, {4, 6, STROKE},
    {4, 1, PT}, {3, 0, PT}, {1, 0, PT}, {0, 1, PT}, {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, END}
  };

  CP edata[] =
  {
    {3, 0, PT}, {1, 0, PT}, {0, 1, PT}, {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, PT}, {4, 2, PT}, {0, 2, END}
  };

  CP fdata[] =
  {
    {1, 0, PT}, {1, 5, PT}, {2, 6, PT}, {3, 6, PT}, {4, 5, STROKE},
    {0, 3, PT}, {3, 3, END}
  };

  CP gdata[] =
  {
    {4, 2, PT}, {3, 1, PT}, {1, 1, PT}, {0, 2, PT}, {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, PT}, {4, 0, PT}, {3, -1, PT}, {1, -1, PT}, {0, 0, END}
  };

  CP hdata[] =
  {
    {0, 0, PT}, {0, 6, STROKE},
    {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, PT}, {4, 0, END}
  };

  CP idata[] =
  {
    {0, 0, PT}, {4, 0, STROKE},
    {1, 4, PT}, {2, 4, PT}, {2, 0, STROKE},
    {2, 6, PT}, {2.25, 5.75, PT}, {2, 5.5, PT}, {1.75, 5.75, PT}, {2, 6, END}
  };

  CP jdata[] =
  {
    {3, 4, PT}, {4, 4, PT}, {4, 0, PT}, {3, -1, PT}, {1, -1, PT}, {0, 0, STROKE},
    {4, 6, PT}, {4.25, 5.75, PT}, {4, 5.5, PT}, {3.75, 5.75, PT}, {4, 6, END}
  };

  CP kdata[] =
  {
    {0, 0, PT}, {0, 6, STROKE},
    {0, 2, PT}, {1, 2, PT}, {4, 4, STROKE},
    {1, 2, PT}, {4, 0, END}
  };

  CP ldata[] =
  {
    {0, 0, PT}, {4, 0, STROKE},
    {1, 6, PT}, {2, 6, PT}, {2, 0, END}
  };

  CP mdata[] =
  {
    {0, 0, PT}, {0, 4, PT}, {2, 4, PT}, {2, 1, STROKE},
    {2, 4, PT}, {3, 4, PT}, {4, 3, PT}, {4, 0, END}
  };

  CP ndata[] =
  {
    {0, 0, PT}, {0, 4, STROKE},
    {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, PT}, {4, 0, END}
  };

  CP odata[] =
  {
    {1.5, 0, PT}, {0.5, 0.5, PT}, {0, 1.5, PT}, {0, 2.5, PT}, {0.5, 3.5, PT},
    {1.5, 4, PT}, {2.5, 4, PT}, {3.5, 3.5, PT}, {4, 2.5, PT}, {4, 1.5, PT},
    {3.5, 0.5, PT}, {2.5, 0, PT}, {1.5, 0, END}
  };

  CP pdata[] =
  {
    {0, -1, PT}, {0, 4, STROKE},
    {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, PT}, {4, 2, PT}, {3, 1, PT}, {1, 1, PT}, {0, 2, END}
  };

  CP qdata[] =
  {
    {4, -1, PT}, {4, 4, STROKE},
    {4, 3, PT}, {3, 4, PT}, {1, 4, PT}, {0, 3, PT}, {0, 2, PT}, {1, 1, PT}, {3, 1, PT}, {4, 2, END}
  };

  CP rdata[] =
  {
    {0, 0, PT}, {0, 4, STROKE},
    {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, END}
  };

  CP sdata[] =
  {
    {0, 0.5, PT}, {1, 0, PT}, {3, 0, PT}, {4, 0.5, PT}, {4, 1.5, PT}, {3, 2, PT}, {1, 2, PT}, {0, 2.5, PT}, {0, 3.5, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3.5, END}
  };

  CP tdata[] =
  {
    {0, 4, PT}, {3.5, 4, STROKE},
    {1.5, 5, PT}, {1.5, 0.5, PT}, {2.0, 0, PT}, {3.5, 0, PT}, {4, 0.5, END}
  };

  CP udata[] =
  {
    {0, 4, PT}, {0, 1, PT}, {1, 0, PT}, {3, 0, PT}, {4, 1, STROKE},
    {4, 0, PT}, {4, 4, END}
  };

  CP vdata[] =
  {
    {0, 4, PT}, {2, 0, PT}, {4, 4, END}
  };

  CP wdata[] =
  {
    {0, 4, PT}, {0, 1, PT}, {1, 0, PT}, {2, 1, STROKE},
    {2, 3, PT}, {2, 1, PT}, {3, 0, PT}, {4, 1, PT}, {4, 4, END}
  };

  CP xdata[] =
  {
    {0, 4, PT}, {4, 0, STROKE},
    {0, 0, PT}, {4, 4, END}
  };

  CP ydata[] =
  {
    {0, 4, PT}, {0, 2, PT}, {1, 1, PT}, {3, 1, PT}, {4, 2, STROKE},
    {4, 4, PT}, {4, 0, PT}, {3, -1, PT}, {1, -1, PT}, {0, 0, END}
  };

  CP zdata[] =
  {
    {0, 4, PT}, {4, 4, PT}, {0, 0, PT}, {4, 0, STROKE},
    {1, 2, PT}, {3, 2, END}
  };

//---------------------------------------------------------------------
//
// numeric stroke data
//
//---------------------------------------------------------------------

  CP n0data[] =
  {
    {1, 0, PT}, {0, 1, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 1, PT}, {3, 0, PT}, {1, 0, STROKE},
    {0, 0, PT}, {4, 6, END}
  };

  CP n1data[] =
  {
    {0, 0, PT}, {4, 0, STROKE},
    {0, 4, PT}, {2, 6, PT}, {2, 0, END}
  };

  CP n2data[] =
  {
    {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 4, PT}, {0, 1, PT}, {0, 0, PT}, {4, 0, END}
  };

  CP n3data[] =
  {
    {0, 6, PT}, {4, 6, PT}, {2, 3, PT}, {3, 3, PT}, {4, 2, PT}, {4, 1, PT}, {3, 0, PT}, {1, 0, PT}, {0, 1, END}
  };

  CP n4data[] =
  {
    {3, 0, PT}, {3, 6, PT}, {0, 3, PT}, {0, 2, PT}, {4, 2, END}
  };

  CP n5data[] =
  {
    {4, 6, PT}, {0, 6, PT}, {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, PT}, {4, 1, PT}, {3, 0, PT}, {1, 0, PT}, {0, 1, END}
  };

  CP n6data[] =
  {
    {3, 6, PT}, {2, 6, PT}, {0, 4, PT}, {0, 1, PT}, {1, 0, PT}, {3, 0, PT}, {4, 1, PT}, {4, 2, PT}, {3, 3, PT}, {1, 3, PT}, {0, 2, END}
  };

  CP n7data[] =
  {
    {0, 6, PT}, {4, 6, PT}, {1, 0, STROKE},
    {1.5, 3, PT}, {3.5, 3, END}
  };

  CP n8data[] =
  {
    {1, 3, PT}, {0, 4, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 4, PT}, {3, 3, PT}, {1, 3, PT},
                {0, 2, PT}, {0, 1, PT}, {1, 0, PT}, {3, 0, PT}, {4, 1, PT}, {4, 2, PT}, {3, 3, END}
  };

  CP n9data[] =
  {
    {1, 0, PT}, {2, 0, PT}, {4, 2, PT}, {4, 5, PT}, {3, 6, PT}, {1, 6, PT}, {0, 5, PT}, {0, 4, PT}, {1, 3, PT}, {3, 3, PT}, {4, 4, END}
  };


//---------------------------------------------------------------------
//
// punctuation data
//
// !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
//
//---------------------------------------------------------------------

  CP space_data[] = { {0, 0, END} };

  CP exclamation_data[] =
  {
    {2, 6, PT}, {2, 2, STROKE},
    {2, 0, PT}, {2.25, 0.25, PT}, {2, 0.5, PT}, {1.75, 0.25, PT}, {2, 0, END}
  };

  CP double_quote_data[] =
  {
    {1, 6, PT}, {1, 4, STROKE},
    {3, 6, PT}, {3, 4, END}
  };

  CP pound_sign_data[] =
  {
    {0.5, 1, PT}, {1.5, 5, STROKE},
    {2.5, 1, PT}, {3.5, 5, STROKE},
    {0, 2, PT}, {4, 2, STROKE},
    {0, 4, PT}, {4, 4, END}
  };

  CP dollar_sign_data[] =
  {
    {4, 5, PT}, {1, 5, PT}, {0, 4, PT}, {1, 3, PT}, {3, 3, PT}, {4, 2, PT}, {3, 1, PT}, {0, 1, STROKE},
    {2, 6, PT}, {2, 0, END}
  };

  CP percent_data[] =
  {
    {0, 0, PT}, {4, 6, STROKE},
    {1, 5.5, PT}, {1.5, 5, PT}, {1, 4.5, PT}, {0.5, 5, PT}, {1, 5.5, STROKE},
    {3, 1.5, PT}, {3.5, 1, PT}, {3, 0.5, PT}, {2.5, 1, PT}, {3, 1.5, END}
  };

  CP ampersand_data[] =
  {
    {4, 2, PT}, {2, 0, PT}, {1, 0, PT}, {0, 1, PT}, {0, 2, PT}, {3, 4, PT}, {3, 5, PT}, {2, 6, PT}, {1, 6, PT}, {0, 5, PT}, {0, 4, PT}, {4, 0, END}
  };

  CP apostrophe_data[] =
  {
    {2, 6, PT}, {2, 5, PT}, {1, 4, END}
  };

  CP left_paren_data[] =
  {
    {3, 6, PT}, {1, 4, PT}, {1, 2, PT}, {3, 0, END}
  };

  CP right_paren_data[] =
  {
    {1, 6, PT}, {3, 4, PT}, {3, 2, PT}, {1, 0, END}
  };

  CP asterisk_data[] =
  {
    {0, 3, PT}, {4, 3, STROKE},
    {0.5, 1.5, PT}, {3.5, 4.5, STROKE},
    {0.5, 4.5, PT}, {3.5, 1.5, END}
  };

  CP plus_data[] =
  {
    {0, 3, PT}, {4, 3, STROKE},
    {2, 5, PT}, {2, 1, END}
  };

  CP comma_data[] =
  {
    {2, 1, PT}, {2, 0, PT}, {1, -1, END}
  };

  CP hyphen_data[] =
  {
    {0, 3, PT}, {4, 3, END}
  };

  CP period_data[] =
  {
    {2, 0.25, PT}, {2.25, 0, PT}, {2, -0.25, PT}, {1.75, 0, PT}, {2, 0.25, END}
  };

  CP slash_data[] =
  {
    {0, 0, PT}, {4, 6, END}
  };

  CP colon_data[] =
  {
    {2, 0.25, PT}, {2.25, 0, PT}, {2, -0.25, PT}, {1.75, 0, PT}, {2, 0.25, STROKE},
    {2, 4.25, PT}, {2.25, 4, PT}, {2, 3.75, PT}, {1.75, 4, PT}, {2, 4.25, END}
  };

  CP semicolon_data[] =
  {
    {2, 4.25, PT}, {2.25, 4, PT}, {2, 3.75, PT}, {1.75, 4, PT}, {2, 4.25, STROKE},
    {2, 1, PT}, {2, 0, PT}, {1, -1, END}
  };

  CP left_angle_data[] =
  {
    {4, 6, PT}, {0, 3, PT}, {4, 0, END}
  };

  CP equal_data_data[] =
  {
    {0, 2, PT}, {4, 2, STROKE},
    {0, 4, PT}, {4, 4, END}
  };

  CP right_angle_data[] =
  {
    {0, 6, PT}, {4, 3, PT}, {0, 0, END}
  };

  CP question_data[] =
  {
    {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 4, PT}, {2, 3, PT}, {2, 2, STROKE},
    {2, 0.25, PT}, {2.25, 0, PT}, {2, -0.25, PT}, {1.75, 0, PT}, {2, 0.25, END}
  };

  CP at_data[] =
  {
    {3, 0, PT}, {1, 0, PT}, {0, 1, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 2, PT}, {1.5, 2, PT}, {1, 2.5, PT}, {1, 3.5, PT},
                {1.5, 4, PT}, {3, 4, PT}, {3, 2, END}
  };

  CP left_square_data[] =
  {
    {4, 0, PT}, {1, 0, PT}, {1, 6, PT}, {4, 6, END}
  };

  CP backslash_data[] =
  {
    {0, 6, PT}, {4, 0, END}
  };

  CP right_square_data[] =
  {
    {0, 0, PT}, {3, 0, PT}, {3, 6, PT}, {0, 6, END}
  };

  CP caret_data[] =
  {
    {0.5, 4.5, PT}, {2, 6, PT}, {3.5, 4.5, END}
  };

  CP underscore_data[] =
  {
    {0, 0, PT}, {4, 0, END}
  };

  CP backquote_data[] =
  {
    {2, 6, PT}, {2, 5, PT}, {3, 4, END}
  };

  CP left_curly_data[] =
  {
    {4, 6, PT}, {2.5, 6, PT}, {2, 5.5, PT}, {2, 4, PT}, {2.5, 3.5, PT}, {1.5, 3, PT},
    {2.5, 2.5, PT}, {2, 2, PT}, {2, 0.5, PT}, {2.5, 0, PT}, {4, 0, END}
  };

  CP vertical_bar_data[] =
  {
    {2, 0, PT}, {2, 6, END}
  };

  CP right_curly_data[] =
  {
    {0, 6, PT}, {1.5, 6, PT}, {2, 5.5, PT}, {2, 4, PT}, {1.5, 3.5, PT}, {2.5, 3, PT},
    {1.5, 2.5, PT}, {2, 2, PT}, {2, 0.5, PT}, {1.5, 0, PT}, {0, 0, END}
  };

  CP tilde_data[] =
  {
    {0.5, 5, PT}, {1.5, 6, PT}, {2.5, 5, PT}, {3.5, 6, END}
  };

//---------------------------------------------------------------------
//
// drawLetter -- parses stroke data into OpenGL code
//
//---------------------------------------------------------------------

  void drawLetter(CP* cp)
  {
    glBegin(GL_LINE_STRIP);
    while (1)
    {
      glVertex2fv(&cp->x);

      switch (cp->type)
      {
      case PT:
        break;
      case STROKE:
        glEnd();
        glBegin(GL_LINE_STRIP);
        break;
      case END:
        glEnd();
        glTranslatef(5.0, 0.0, 0.0);
        return;
      }

      cp++;
    }
  }

  GLuint getStrokeFontListBase()
  {
    static GLuint listBase = 0;

    if (listBase != 0) return listBase;

    listBase = glGenLists(128);

    dbgEvalNL(3, listBase);

    glNewList(listBase + 'A', GL_COMPILE); drawLetter(Adata); glEndList();
    glNewList(listBase + 'B', GL_COMPILE); drawLetter(Bdata); glEndList();
    glNewList(listBase + 'C', GL_COMPILE); drawLetter(Cdata); glEndList();
    glNewList(listBase + 'D', GL_COMPILE); drawLetter(Ddata); glEndList();
    glNewList(listBase + 'E', GL_COMPILE); drawLetter(Edata); glEndList();
    glNewList(listBase + 'F', GL_COMPILE); drawLetter(Fdata); glEndList();
    glNewList(listBase + 'G', GL_COMPILE); drawLetter(Gdata); glEndList();
    glNewList(listBase + 'H', GL_COMPILE); drawLetter(Hdata); glEndList();
    glNewList(listBase + 'I', GL_COMPILE); drawLetter(Idata); glEndList();
    glNewList(listBase + 'J', GL_COMPILE); drawLetter(Jdata); glEndList();
    glNewList(listBase + 'K', GL_COMPILE); drawLetter(Kdata); glEndList();
    glNewList(listBase + 'L', GL_COMPILE); drawLetter(Ldata); glEndList();
    glNewList(listBase + 'M', GL_COMPILE); drawLetter(Mdata); glEndList();
    glNewList(listBase + 'N', GL_COMPILE); drawLetter(Ndata); glEndList();
    glNewList(listBase + 'O', GL_COMPILE); drawLetter(Odata); glEndList();
    glNewList(listBase + 'P', GL_COMPILE); drawLetter(Pdata); glEndList();
    glNewList(listBase + 'Q', GL_COMPILE); drawLetter(Qdata); glEndList();
    glNewList(listBase + 'R', GL_COMPILE); drawLetter(Rdata); glEndList();
    glNewList(listBase + 'S', GL_COMPILE); drawLetter(Sdata); glEndList();
    glNewList(listBase + 'T', GL_COMPILE); drawLetter(Tdata); glEndList();
    glNewList(listBase + 'U', GL_COMPILE); drawLetter(Udata); glEndList();
    glNewList(listBase + 'V', GL_COMPILE); drawLetter(Vdata); glEndList();
    glNewList(listBase + 'W', GL_COMPILE); drawLetter(Wdata); glEndList();
    glNewList(listBase + 'X', GL_COMPILE); drawLetter(Xdata); glEndList();
    glNewList(listBase + 'Y', GL_COMPILE); drawLetter(Ydata); glEndList();
    glNewList(listBase + 'Z', GL_COMPILE); drawLetter(Zdata); glEndList();

    glNewList(listBase + 'a', GL_COMPILE); drawLetter(adata); glEndList();
    glNewList(listBase + 'b', GL_COMPILE); drawLetter(bdata); glEndList();
    glNewList(listBase + 'c', GL_COMPILE); drawLetter(cdata); glEndList();
    glNewList(listBase + 'd', GL_COMPILE); drawLetter(ddata); glEndList();
    glNewList(listBase + 'e', GL_COMPILE); drawLetter(edata); glEndList();
    glNewList(listBase + 'f', GL_COMPILE); drawLetter(fdata); glEndList();
    glNewList(listBase + 'g', GL_COMPILE); drawLetter(gdata); glEndList();
    glNewList(listBase + 'h', GL_COMPILE); drawLetter(hdata); glEndList();
    glNewList(listBase + 'i', GL_COMPILE); drawLetter(idata); glEndList();
    glNewList(listBase + 'j', GL_COMPILE); drawLetter(jdata); glEndList();
    glNewList(listBase + 'k', GL_COMPILE); drawLetter(kdata); glEndList();
    glNewList(listBase + 'l', GL_COMPILE); drawLetter(ldata); glEndList();
    glNewList(listBase + 'm', GL_COMPILE); drawLetter(mdata); glEndList();
    glNewList(listBase + 'n', GL_COMPILE); drawLetter(ndata); glEndList();
    glNewList(listBase + 'o', GL_COMPILE); drawLetter(odata); glEndList();
    glNewList(listBase + 'p', GL_COMPILE); drawLetter(pdata); glEndList();
    glNewList(listBase + 'q', GL_COMPILE); drawLetter(qdata); glEndList();
    glNewList(listBase + 'r', GL_COMPILE); drawLetter(rdata); glEndList();
    glNewList(listBase + 's', GL_COMPILE); drawLetter(sdata); glEndList();
    glNewList(listBase + 't', GL_COMPILE); drawLetter(tdata); glEndList();
    glNewList(listBase + 'u', GL_COMPILE); drawLetter(udata); glEndList();
    glNewList(listBase + 'v', GL_COMPILE); drawLetter(vdata); glEndList();
    glNewList(listBase + 'w', GL_COMPILE); drawLetter(wdata); glEndList();
    glNewList(listBase + 'x', GL_COMPILE); drawLetter(xdata); glEndList();
    glNewList(listBase + 'y', GL_COMPILE); drawLetter(ydata); glEndList();
    glNewList(listBase + 'z', GL_COMPILE); drawLetter(zdata); glEndList();

    glNewList(listBase + '0', GL_COMPILE); drawLetter(n0data); glEndList();
    glNewList(listBase + '1', GL_COMPILE); drawLetter(n1data); glEndList();
    glNewList(listBase + '2', GL_COMPILE); drawLetter(n2data); glEndList();
    glNewList(listBase + '3', GL_COMPILE); drawLetter(n3data); glEndList();
    glNewList(listBase + '4', GL_COMPILE); drawLetter(n4data); glEndList();
    glNewList(listBase + '5', GL_COMPILE); drawLetter(n5data); glEndList();
    glNewList(listBase + '6', GL_COMPILE); drawLetter(n6data); glEndList();
    glNewList(listBase + '7', GL_COMPILE); drawLetter(n7data); glEndList();
    glNewList(listBase + '8', GL_COMPILE); drawLetter(n8data); glEndList();
    glNewList(listBase + '9', GL_COMPILE); drawLetter(n9data); glEndList();

    glNewList(listBase + ' ', GL_COMPILE); drawLetter(space_data); glEndList();
    glNewList(listBase + '!', GL_COMPILE); drawLetter(exclamation_data); glEndList();
    glNewList(listBase + '"', GL_COMPILE); drawLetter(double_quote_data); glEndList();
    glNewList(listBase + '#', GL_COMPILE); drawLetter(pound_sign_data); glEndList();
    glNewList(listBase + '$', GL_COMPILE); drawLetter(dollar_sign_data); glEndList();
    glNewList(listBase + '%', GL_COMPILE); drawLetter(percent_data); glEndList();
    glNewList(listBase + '&', GL_COMPILE); drawLetter(ampersand_data); glEndList();
    glNewList(listBase + '\'', GL_COMPILE); drawLetter(apostrophe_data); glEndList();
    glNewList(listBase + '(', GL_COMPILE); drawLetter(left_paren_data); glEndList();
    glNewList(listBase + ')', GL_COMPILE); drawLetter(right_paren_data); glEndList();
    glNewList(listBase + '*', GL_COMPILE); drawLetter(asterisk_data); glEndList();
    glNewList(listBase + '+', GL_COMPILE); drawLetter(plus_data); glEndList();
    glNewList(listBase + ',', GL_COMPILE); drawLetter(comma_data); glEndList();
    glNewList(listBase + '-', GL_COMPILE); drawLetter(hyphen_data); glEndList();
    glNewList(listBase + '.', GL_COMPILE); drawLetter(period_data); glEndList();
    glNewList(listBase + '/', GL_COMPILE); drawLetter(slash_data); glEndList();
    glNewList(listBase + ':', GL_COMPILE); drawLetter(colon_data); glEndList();
    glNewList(listBase + ';', GL_COMPILE); drawLetter(semicolon_data); glEndList();
    glNewList(listBase + '<', GL_COMPILE); drawLetter(left_angle_data); glEndList();
    glNewList(listBase + '=', GL_COMPILE); drawLetter(equal_data_data); glEndList();
    glNewList(listBase + '>', GL_COMPILE); drawLetter(right_angle_data); glEndList();
    glNewList(listBase + '?', GL_COMPILE); drawLetter(question_data); glEndList();
    glNewList(listBase + '@', GL_COMPILE); drawLetter(at_data); glEndList();
    glNewList(listBase + '[', GL_COMPILE); drawLetter(left_square_data); glEndList();
    glNewList(listBase + '\\', GL_COMPILE); drawLetter(backslash_data); glEndList();
    glNewList(listBase + ']', GL_COMPILE); drawLetter(right_square_data); glEndList();
    glNewList(listBase + '^', GL_COMPILE); drawLetter(caret_data); glEndList();
    glNewList(listBase + '_', GL_COMPILE); drawLetter(underscore_data); glEndList();
    glNewList(listBase + '`', GL_COMPILE); drawLetter(backquote_data); glEndList();
    glNewList(listBase + '{', GL_COMPILE); drawLetter(left_curly_data); glEndList();
    glNewList(listBase + '|', GL_COMPILE); drawLetter(vertical_bar_data); glEndList();
    glNewList(listBase + '}', GL_COMPILE); drawLetter(right_curly_data); glEndList();
    glNewList(listBase + '~', GL_COMPILE); drawLetter(tilde_data); glEndList();

    return listBase;
  }
}


///////////////////////////////////////////////////////////////////////
//
// GxVectorFont member definitions
//
///////////////////////////////////////////////////////////////////////


GxVectorFont::GxVectorFont() {}

GxVectorFont::~GxVectorFont() throw() {}

const char* GxVectorFont::fontName() const
{
  return "vector";
}

unsigned int GxVectorFont::listBase() const
{
  return getStrokeFontListBase();
}

void GxVectorFont::bboxOf(const char* text, Gfx::Bbox& bbox) const
{
  int longest = 0;
  int current = 0;
  int lines = 1;
  const char* p = text;

  while (*p != '\0')
    {
      if (*p == '\n')
        {
          if (current > longest)
            longest = current;
          current = 0;
          ++lines;
        }
      else
        {
          ++current;
        }
      ++p;
    }

  if (current > longest)
    longest = current;


  Gfx::Rect<double> rect;
  rect.left() = 0.0;
  unsigned int len = longest;
  rect.right() = len > 0 ? (5*len) - 1 : 1.0;
  rect.bottom() = -1.0 - (lines - 1) * 8.0;
  rect.top() = 6.0;

  bbox.drawRect(rect);
}

void GxVectorFont::drawText(const char* text, Gfx::Canvas& canvas) const
{
DOTRACE("GxVectorFont::drawText");

  canvas.drawVectorText(text, *this);
}

double GxVectorFont::vectorHeight() const
{
DOTRACE("GxVectorFont::vectorHeight");
  return 8.0;
}

static const char vcid_gxvectorfont_cc[] = "$Header$";
#endif // !GXVECTORFONT_CC_DEFINED
