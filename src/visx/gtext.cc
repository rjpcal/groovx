///////////////////////////////////////////////////////////////////////
//
// gtext.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  1 11:54:48 1999
// written: Thu Jul  1 13:57:07 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXT_CC_DEFINED
#define GTEXT_CC_DEFINED

#include "gtext.h"

#include <GL/gl.h>
#include <togl.h>

#include "objtogl.h"

#define LOCAL_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  GLuint fontListBase = -1;

  const string ioTag = "Gtext";
}

Gtext::Gtext(const char* text) :
  itsText(text ? text : "")
{
DOTRACE("Gtext::Gtext(const char*)");
}

Gtext::Gtext(istream& is, IOFlag flag) {
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
  return (ioTag.size() + 1
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

void Gtext::grRender() const {
DOTRACE("Gtext::grRender");
  glPushAttrib(GL_LIST_BIT);
	 glRasterPos2d(0, 0);
    glListBase( fontListBase );
	 glCallLists( itsText.length(), GL_BYTE, itsText.c_str() );
  glPopAttrib();
}

void Gtext::loadFont(const char* fontname) {
DOTRACE("Gtext::loadFont");
  Togl* togl = ObjTogl::theWidget(); 


  GLuint newListBase = Togl_LoadBitmapFont(togl, fontname);

  // Check if font loading succeeded...
  if (newListBase == 0) {
	 throw GtextError(string("unable to load font ") + fontname);
  }

  // ... otherwise unload the current font
  Togl_UnloadBitmapFont(togl, fontListBase);

  // ... and point to the new font
  fontListBase = newListBase;
}


static const char vcid_gtext_cc[] = "$Header$";
#endif // !GTEXT_CC_DEFINED
