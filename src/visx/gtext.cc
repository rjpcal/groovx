///////////////////////////////////////////////////////////////////////
//
// gtext.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  1 11:54:48 1999
// written: Tue Jul  6 13:16:06 1999
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
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  const string ioTag = "Gtext";
}

Gtext::Gtext(const char* text) :
  itsText(text ? text : ""),
  itsListBase(0)
{
DOTRACE("Gtext::Gtext(const char*)");
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

void Gtext::draw() const {
DOTRACE("Gtext::draw");
  unsigned int fontListBase = ObjTogl::theToglConfig()->getFontListBase();
  if (itsListBase != fontListBase) { 
	 itsListBase = fontListBase;
	 sendStateChangeMsg();
  }
  GrObj::draw();
}

void Gtext::grRender() const {
DOTRACE("Gtext::grRender");
  glPushAttrib(GL_LIST_BIT);
    glRasterPos2d(0, 0);
    glListBase( itsListBase );
	 glCallLists( itsText.length(), GL_BYTE, itsText.c_str() );
  glPopAttrib();
}

static const char vcid_gtext_cc[] = "$Header$";
#endif // !GTEXT_CC_DEFINED
