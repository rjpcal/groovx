///////////////////////////////////////////////////////////////////////
// face.cc
// Rob Peters
// created: Dec-98
// written: Fri Mar 12 10:50:46 1999
///////////////////////////////////////////////////////////////////////

#ifndef FACE_CC_DEFINED
#define FACE_CC_DEFINED

#include "face.h"

#include <iostream.h>			  // for serialize
#include <string>
#include <typeinfo>
#include <GL/gl.h>
#include <GL/glu.h>

#include "gfxattribs.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// File scope data 
///////////////////////////////////////////////////////////////////////

namespace {
  const float theirNose_x = 0.0;
  const float theirMouth_x[2] = {-0.2, 0.2};
}

///////////////////////////////////////////////////////////////////////
// Face member functions
///////////////////////////////////////////////////////////////////////

Face::Face(float eh, float es, float nl, float mh, int categ) : 
  itsCategory(categ)
{
DOTRACE("Face::Face");
  // Default to white 
  itsColor[0] = itsColor[1] = itsColor[2] = 1.0;

  setEyeHgt(eh);
  setEyeDist(es);
  setNoseLen(nl);
  setMouthHgt(mh);
}

// read the object's state from an input stream. The input stream must
// already be open and connected to an appropriate file.
Face::Face(istream &is) {
DOTRACE("Face::Face");
  deserialize(is);
}

Face::~Face() {
}

// write the object's state to an output stream. The output stream must
// already be open and connected to an appropriate file.
IOResult Face::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Face::serialize");
  if (flag & IO::BASES) { GrObj::serialize(os, flag); }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << typeid(this).name() << sep; }

  os << itsCategory << sep;
  os << getEyeHgt() << sep;
  os << getEyeDist() << sep;
  os << getNoseLen() << sep;
  os << getMouthHgt();
  os << endl;
  return checkStream(os);
}

IOResult Face::deserialize(istream &is, IOFlag flag) {
DOTRACE("Face::deserialize");
  if (flag & IO::BASES) { GrObj::deserialize(is, flag); }
  if (flag & IO::TYPENAME) {
	 string name;
	 is >> name;
	 if (name != string(typeid(this).name())) { return IO_ERROR; }
  }

  int cat;
  is >> cat; itsCategory = cat;
  float val;
  is >> val; setEyeHgt(val);
  is >> val; setEyeDist(val);
  is >> val; setNoseLen(val);
  is >> val; setMouthHgt(val);
}

void Face::grRecompile() const {
DOTRACE("Face::grRecompile");
  // dump old display list and get new one
  grNewList();

  // use qobj for calling gluDisk()
  GLUquadricObj *qobj = gluNewQuadric();
  gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);
  static const int num_slices = 20;
  static const int num_loops = 1;
  static const GLdouble outer_radius = 0.5;
  static const GLdouble eyeball_x_scale = 0.185;
  static const GLdouble eyeball_y_scale = 0.1;
  static const GLdouble pupil_x_scale = 0.3784;	// rel to eyeball scale
  static const GLdouble pupil_y_scale = 0.7;	// rel to eyeball scale

  // use these params for generating the face outline
  static const int num_subdivisions = 30;
  static const int nctrlsets = 2;
  static const float ctrlpnts[] = {    -.7, 0.2, 0, // first 4 control points 
													-.7, 1.4, 0,
													.7, 1.4, 0,
													.7, 0.2, 0,
													.7, 0.2, 0, // second 4 control points 
													.6, -1.7, 0,
													-.6, -1.7, 0,
													-.7, 0.2, 0};
  
  const int use_antialiasing = GfxAttribs::usingRgba();

  // make the big display list
  glNewList(grDisplayList(), GL_COMPILE);
    if (use_antialiasing) {
		glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
		glEnable(GL_BLEND); // blend incoming RGBA values with old RGBA values
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // use transparency 
		glEnable(GL_LINE_SMOOTH);   // use anti-aliasing 
	 }

    // prepare to push and pop modelview matrices
    glMatrixMode(GL_MODELVIEW);

    // draw left eye
    glPushMatrix();
    glTranslatef(itsEye[0], itsEyeheight, 0.0); 
    glScalef(eyeball_x_scale, eyeball_y_scale, 1.0);
    gluDisk(qobj, 0.0, outer_radius, num_slices, num_loops);
    glScalef(pupil_x_scale, pupil_y_scale, 1.0);
    gluDisk(qobj, 0.0, outer_radius, num_slices, num_loops);
    glPopMatrix();

    // draw right eye
    glPushMatrix();
    glTranslatef(itsEye[1], itsEyeheight, 0.0);
    glScalef(eyeball_x_scale, eyeball_y_scale, 1.0);
    gluDisk(qobj, 0.0, outer_radius, num_slices, num_loops);
    glScalef(pupil_x_scale, pupil_y_scale, 1.0);
    gluDisk(qobj, 0.0, outer_radius, num_slices, num_loops);
    glPopMatrix();

    // draw outline
    glEnable(GL_MAP1_VERTEX_3);
    for (int i = 0; i < nctrlsets; i++) {
      glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpnts[i*12]);
      // define a 1-d evaluator for the Bezier curves 
      glBegin(GL_LINE_STRIP);
      for (int j = 0; j <= num_subdivisions; j++) {
        glEvalCoord1f((GLfloat) j/ (GLfloat) num_subdivisions);
      }
      glEnd();
    }
    
    // draw nose and mouth
    glBegin(GL_LINES);
    glVertex2f(theirMouth_x[0], itsMouthHeight);
    glVertex2f(theirMouth_x[1], itsMouthHeight);
    glVertex2f(theirNose_x, itsNose[0]);
    glVertex2f(theirNose_x, itsNose[1]);
    glEnd();

	 if (use_antialiasing) {
		glPopAttrib();
	 }
  glEndList();

  grPostCompiled();
}

#endif // !FACE_CC_DEFINED
