///////////////////////////////////////////////////////////////////////
// face.cc
// Rob Peters
// created: Dec-98
// written: Mon Apr 26 19:20:41 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef FACE_CC_DEFINED
#define FACE_CC_DEFINED

#include "face.h"

#include <iostream.h>           // for serialize
#include <iomanip.h>				  // for setw in serialize
#include <string>
#include <typeinfo>
#include <GL/gl.h>
#include <GL/glu.h>

#include "gfxattribs.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#define INVARIANT
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
  setEyeHgt(eh);
  setEyeDist(es);
  setNoseLen(nl);
  setMouthHgt(mh);
  Invariant(check());
}

// read the object's state from an input stream. The input stream must
// already be open and connected to an appropriate file.
Face::Face(istream &is, IOFlag flag) {
DOTRACE("Face::Face");
  deserialize(is, flag);
  Invariant(check());
}

Face::~Face() {
DOTRACE("Face::~Face");
}

// write the object's state to an output stream. The output stream must
// already be open and connected to an appropriate file.
void Face::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Face::serialize");
  Invariant(check());
  if (flag & BASES) { GrObj::serialize(os, flag); }

  char sep = ' ';
  if (flag & TYPENAME) { os << typeid(Face).name() << sep; }

  os << itsCategory << sep;
  os.setf(ios::fixed);
  os.precision(2);
  os << setw(4) << getEyeHgt() << sep;
  os << setw(4) << getEyeDist() << sep;
  os << setw(4) << getNoseLen() << sep;
  os << setw(4) << getMouthHgt();
  os << endl;
  if (os.fail()) throw OutputError(typeid(Face));
}

void Face::deserialize(istream &is, IOFlag flag) {
DOTRACE("Face::deserialize");
  if (flag & BASES) { GrObj::deserialize(is, flag); }
  if (flag & TYPENAME) {
    string name;
    is >> name;
    if (name != typeid(Face).name()) { 
		throw InputError(typeid(Face));
	 }
  }

  int cat;
  is >> cat; itsCategory = cat;
  float val;
  is >> val; setEyeHgt(val);
  is >> val; setEyeDist(val);
  is >> val; setNoseLen(val);
  is >> val; setMouthHgt(val);

  // Mysterious bug in HP compiler (aCC) requires the following
  // contorted code; if the exception is not first caught in this
  // function, then re-thrown, it causes a bus error when compiled
  // with optimization on, although there is no problem with
  // optimization off. Somehow adding the catch and re-throw avoids
  // the problem, without changing the program's behavior.
  try {
	 if (is.fail()) throw InputError(typeid(Face));
  }
  catch (IoError& err) {
 	 throw;
  }
  Invariant(check());
}

void Face::grRecompile() const {
DOTRACE("Face::grRecompile");
  Invariant(check());

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
  static const GLdouble pupil_x_scale = 0.3784; // rel to eyeball scale
  static const GLdouble pupil_y_scale = 0.7; // rel to eyeball scale

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
  
  const bool use_antialiasing = GfxAttribs::usingRgba();

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

bool Face::check() const {
DOTRACE("Face::check");
  return ( (itsNose[0]<=0.0) &&
			  (itsNose[1]>=0.0) &&
			  (itsEye[0]<=0.0) &&
			  (itsEye[1]>=0.0) );
}

static const char vcid_face_cc[] = "$Header$";
#endif // !FACE_CC_DEFINED
