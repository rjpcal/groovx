///////////////////////////////////////////////////////////////////////
//
// gtext.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  1 11:54:48 1999
// written: Mon Sep 27 14:33:58 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXT_CC_DEFINED
#define GTEXT_CC_DEFINED

#include "gtext.h"

#include <GL/gl.h>
#include <togl.h>

#include "objtogl.h"
#include "toglconfig.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_DEBUG

#define LOCAL_ASSERT
#include "debug.h"

namespace {
  const string ioTag = "Gtext";

  enum pointtype {PT, STROKE, END};
  struct CP {
	 GLint x,y;
	 pointtype type;
  };

  CP Adata[] = {
	 {0, 0, PT}, {0, 3, PT}, {2, 6, PT}, {4, 3, PT}, {4, 0, STROKE},
	 {0, 3, PT}, {4, 3, END}
  };

  CP Bdata[] = {
	 {0, 0, PT}, {0, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 4, PT}, {3, 3, PT}, {4, 2, PT}, {4, 1, PT}, {3, 0, PT}, {0, 0, STROKE},
	 {0, 3, PT}, {3, 3, END}
  };

  CP Cdata[] = {
	 {4, 1, PT}, {3, 0, PT}, {1, 0, PT}, {0, 1, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, END}
  };

  CP Ddata[] = {
	 {0, 0, PT}, {0, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 1, PT}, {3, 0, PT}, {0, 0, END}
  };

  CP Edata[] = {
	 {4, 0, PT}, {0, 0, PT}, {0, 6, PT}, {4, 6, STROKE},
	 {0, 3, PT}, {3, 3, END}
  };

  CP Fdata[] = {
	 {0, 0, PT}, {0, 6, PT}, {4, 6, STROKE},
	 {0, 3, PT}, {3, 3, END}
  };

  CP Gdata[] = {
	 {2, 3, PT}, {4, 3, PT}, {4, 1, PT}, {3, 0, PT}, {1, 0, PT}, {0, 1, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, END}
  };

  CP Hdata[] = {
	 {0, 0, PT}, {0, 6, STROKE},
	 {4, 0, PT}, {4, 6, STROKE},
	 {0, 3, PT}, {4, 3, END}
  };

  CP Idata[] = {
	 {2, 6, PT}, {2, 0, STROKE},
	 {0, 0, PT}, {4, 0, STROKE},
	 {0, 6, PT}, {4, 6, END},
  };

  CP Jdata[] = {
	 {0, 1, PT}, {1, 0, PT}, {3, 0, PT}, {4, 1, PT}, {4, 6, END}
  };

  CP Kdata[] = {
	 {0, 0, PT}, {0, 6, STROKE},
	 {4, 6, PT}, {0, 3, PT}, {4, 0, END}
  };

  CP Ldata[] = {
	 {4, 0, PT}, {0, 0, PT}, {0, 6, END}
  };

  CP Mdata[] = {
	 {0, 0, PT}, {0, 6, PT}, {2, 3, PT}, {4, 6, PT}, {4, 0, END}
  };

  CP Ndata[] = {
	 {0, 0, PT}, {0, 6, PT}, {4, 0, PT}, {4, 6, END}
  };

  CP Odata[] = {
	 {1, 0, PT}, {0, 1, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 1, PT}, {3, 0, PT}, {1, 0, END}
  };

  CP Pdata[] = {
	 {0, 0, PT}, {0, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 4, PT}, {3, 3, PT}, {0, 3, END}
  };

  CP Qdata[] = {
	 {1, 0, PT}, {0, 1, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 1, PT}, {3, 0, PT}, {1, 0, STROKE},
	 {3, 1, PT}, {4, 0, END}
  };

  CP Rdata[] = {
	 {0, 0, PT}, {0, 6, PT}, {3, 6, PT}, {4, 5, PT}, {4, 4, PT}, {3, 3, PT}, {0, 3, STROKE},
	 {1, 3, PT}, {4, 0, END}
  };

  CP Sdata[] = {
	 {0, 1, PT}, {1, 0, PT}, {3, 0, PT}, {4, 1, PT}, {4, 2, PT}, {3, 3, PT}, {1, 3, PT}, {0, 4, PT}, {0, 5, PT}, {1, 6, PT}, {3, 6, PT}, {4, 5, END}
  };

  CP Tdata[] = {
	 {2, 0, PT}, {2, 6, STROKE},
	 {0, 6, PT}, {4, 6, END},
  };

  CP Udata[] = {
	 {0, 6, PT}, {0, 1, PT}, {1, 0, PT}, {3, 0, PT}, {4, 1, PT}, {4, 6, END}
  };

  CP Vdata[] = {
	 {0, 6, PT}, {2, 0, PT}, {4, 6, END}
  };

  CP Wdata[] = {
	 {0, 6, PT}, {0, 0, PT}, {2, 3, PT}, {4, 0, PT}, {4, 6, END}
  };

  CP Xdata[] = {
	 {0, 0, PT}, {4, 6, STROKE},
	 {0, 6, PT}, {4, 0, END}
  };

  CP Ydata[] = {
	 {0, 6, PT}, {2, 3, PT}, {4, 6, STROKE},
	 {2, 3, PT}, {2, 0, END}
  };

  CP Zdata[] = {
	 {4, 0, PT}, {0, 0, PT}, {4, 6, PT}, {0, 6, STROKE},
	 {1, 3, PT}, {3, 3, END}
  };

  CP adata[] = {
	 {1, 4, PT}, {3, 4, PT}, {4, 3, PT}, {4, 0, STROKE},
	 {4, 3, PT}, {1, 3, PT}, {0, 2, PT}, {0, 1, PT}, {1, 0, PT}, {3, 0, PT}, {4, 1, END}
  };
  
  CP bdata[] = {
	 {0, 0, PT}, {0, 6, STROKE},
	 {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, PT}, {4, 1, PT}, {3, 0, PT}, {1, 0, PT}, {0, 1, END}
  };

  CP cdata[] = {
	 {4, 1, PT}, {3, 0, PT}, {1, 0, PT}, {0, 1, PT}, {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, END}
  };

  CP ddata[] = {
	 {4, 0, PT}, {4, 6, STROKE},
	 {4, 1, PT}, {3, 0, PT}, {1, 0, PT}, {0, 1, PT}, {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, END}
  };

  CP edata[] = {
	 {3, 0, PT}, {1, 0, PT}, {0, 1, PT}, {0, 3, PT}, {1, 4, PT}, {3, 4, PT}, {4, 3, PT}, {4, 2, PT}, {0, 2, END}
  };

  void drawLetter(CP* cp) {
	 glBegin(GL_LINE_STRIP);
	 while (1) {
		glVertex2iv(&cp->x);

		switch (cp->type) {
		case PT:
		  break;
		case STROKE:
		  glEnd();
		  glBegin(GL_LINE_STRIP);
		  break;
		case END:
		  glEnd();
		  glTranslatef(6.0, 0.0, 0.0);
		  return;
		}
		
		cp++;
	 }
  }
  
  GLuint getStrokeFontListBase() {
	 static GLuint listBase = 0;

	 if (listBase != 0) return listBase;

	 listBase = glGenLists(128);

	 DebugEvalNL(listBase);

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

	 glNewList(listBase + ' ', GL_COMPILE); glTranslatef(6.0, 0.0, 0.0); glEndList();

	 return listBase;
  }

}

Gtext::Gtext(const char* text) :
  GrObj(GROBJ_GL_COMPILE, GROBJ_SWAP_FORE_BACK),
  itsText(text ? text : ""),
  itsStrokeWidth(2),
  itsListBase(0)
{
DOTRACE("Gtext::Gtext(const char*)");
  itsListBase = getStrokeFontListBase();
}

Gtext::Gtext(istream& is, IOFlag flag) :
  itsListBase(0)
{
DOTRACE("Gtext::Gtext(istream&, IOFlag)");
  deserialize(is, flag);
}
 
Gtext::~Gtext() {
DOTRACE("Gtext::~Gtext");
}

void Gtext::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Gtext::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsText << endl;

  if (flag & BASES) { GrObj::serialize(os, flag); }
}

void Gtext::deserialize(istream &is, IOFlag flag) {
DOTRACE("Gtext::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); } 

  getline(is, itsText, '\n');

  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { GrObj::deserialize(is, flag); }
}

int Gtext::charCount() const {
DOTRACE("Gtext::charCount");
  return (ioTag.length() + 1
			 + itsText.length() + 1
			 + 1); // fudge factor
}

void Gtext::setText(const char* text) {
DOTRACE("Gtext::setText");
  itsText = text;
  sendStateChangeMsg();
}

const char* Gtext::getText() const {
DOTRACE("Gtext::getText");
  return itsText.c_str();
}

void Gtext::setStrokeWidth(int width) {
DOTRACE("Gtext::setStrokeWidth");
  if (width > 0) {
	 itsStrokeWidth = width;
	 sendStateChangeMsg();
  }
}

int Gtext::getStrokeWidth() const {
DOTRACE("Gtext::getStrokeWidth");
  return itsStrokeWidth; 
}

bool Gtext::grGetBoundingBox(double& left, double& top,
									  double& right, double& bottom,
									  int& border_pixels) const {
DOTRACE("Gtext::grGetBoundingBox");

  left = 0.0;
  right = (6*itsText.length()) - 2;
  bottom = -1.0;
  top = 6.0;
  border_pixels = 4;

  return true;
}

void Gtext::grRender() const {
DOTRACE("Gtext::grRender");
//   unsigned int fontListBase = ObjTogl::theToglConfig()->getFontListBase();
//   if (itsListBase != fontListBase) { 
// 	 itsListBase = fontListBase;
// 	 sendStateChangeMsg();
//   }
  glPushAttrib(GL_LIST_BIT|GL_LINE_BIT);
    glListBase( itsListBase );
	 glLineWidth(itsStrokeWidth);
	 glCallLists( itsText.length(), GL_BYTE, itsText.c_str() );
  glPopAttrib();
}

static const char vcid_gtext_cc[] = "$Header$";
#endif // !GTEXT_CC_DEFINED
