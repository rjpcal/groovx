///////////////////////////////////////////////////////////////////////
//
// face.cc
// Rob Peters
// created: Dec-98
// written: Sat May 15 15:15:15 1999
// $Id$
//
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
#define LOCAL_INVARIANT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data 
//
///////////////////////////////////////////////////////////////////////

namespace {
  const float theirNose_x = 0.0;
  const float theirMouth_x[2] = {-0.2, 0.2};

  const string ioTag = "Face";
}

///////////////////////////////////////////////////////////////////////
//
// Face member functions
//
///////////////////////////////////////////////////////////////////////

Face::Face(float eh, float es, float nl, float mh, int categ) {
DOTRACE("Face::Face");
  itsCategory() = categ;
  setEyeHgt(eh);
  setEyeDist(es);
  setNoseLen(nl);
  setMouthHgt(mh);
  Invariant(check());
}

// read the object's state from an input stream. The input stream must
// already be open and connected to an appropriate file.
Face::Face(istream& is, IOFlag flag) {
DOTRACE("Face::Face(istream&, IOFlag)");
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

  vector<const IO *> ioList;
  makeIoList(ioList);
  for (vector<const IO *>::const_iterator ii = ioList.begin(); 
		 ii != ioList.end(); ii++) {
	 (*ii)->serialize(os, flag);
  }
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

  vector<IO *> ioList;
  makeIoList(ioList);
  for (vector<IO *>::iterator ii = ioList.begin(); ii != ioList.end(); ii++) {
	 (*ii)->deserialize(is, flag);
  }

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

int Face::charCount() const {
DOTRACE("Face::charCount");
  return (ioTag.size() + 1
			 + charCountInt(itsCategory()) + 1
			 + 4*(4 + 1)
			 + 5);//fudge factor
}

void Face::grRecompile() const {
DOTRACE("Face::grRecompile");
  Invariant(check());

  // Delete old display list and get new display list.
  grNewList();

  // Create a quadric obj to use for calling gluDisk(). This disk will
  // be used to render the eyeballs and the pupils.
  GLUquadricObj* qobj = gluNewQuadric();
  gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);
  static const int num_slices = 20;
  static const int num_loops = 1;
  static const GLdouble outer_radius = 0.5;

  // Calculate the x positions for the left and right eyes
  // left position always <= 0.0
  // right position always >= 0.0
  const float left_eye_x = -abs(itsEyeDistance())/2.0;
  const float right_eye_x = -left_eye_x;

  // Calculate the y positions for the top and bottom of the nose
  // bottom always <= 0.0
  // top always >= 0.0
  const float nose_bottom_y = -abs(itsNoseLength())/2.0;
  const float nose_top_y = -nose_bottom_y;

  // Generate the eyeball scales on the basis of the eye aspect. The
  // eye aspect should range from 0.0 to 1.0 to control the eyeball x
  // and y scales from 0.1 to 0.185. The x and y scales are always at
  // opposite points within this range.
  const float eye_aspect = getEyeAspect();
  const GLdouble eyeball_x_scale = 0.1*eye_aspect     + 0.185*(1-eye_aspect);
  const GLdouble eyeball_y_scale = 0.1*(1-eye_aspect) + 0.185*eye_aspect;

  // The absolute scale of the pupil.
  static const GLdouble pupil_x_scale_abs = 0.07;
  static const GLdouble pupil_y_scale_abs = 0.07;

  // The scale of the pupil relative to the eyeball scale. These
  // values are computed since it is more efficient in the drawing
  // routine to do an additional scale for the pupils after having
  // drawn the eyeballs, rather than to pop the modelview matrix after
  // the eyeballs and push a new matrix for the pupils. But, maybe
  // this is the sort of optimization that OpenGL would make on its
  // own in a display list.
  const GLdouble pupil_x_scale = pupil_x_scale_abs/eyeball_x_scale;
  const GLdouble pupil_y_scale = pupil_y_scale_abs/eyeball_y_scale;

  // These parameters control the generation of the Bezier curve for
  // the face outline.
  static const int num_subdivisions = 30;
  static const int nctrlsets = 2;
  const float* const ctrlpnts = getCtrlPnts();
  
  const bool have_antialiasing = GfxAttribs::usingRgba();

  // Generate the display list that will draw the face
  glNewList(grDisplayList(), GL_COMPILE);
    // Enable antialiasing, if it is available
    if (have_antialiasing) {
      glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
      glEnable(GL_BLEND); // blend incoming RGBA values with old RGBA values
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // use transparency 
      glEnable(GL_LINE_SMOOTH);   // use anti-aliasing 
    }

    // Prepare to push and pop modelview matrices.
    glMatrixMode(GL_MODELVIEW);

    // Draw left eye.
    glPushMatrix();
    glTranslatef(left_eye_x, itsEyeHeight(), 0.0); 
    glScalef(eyeball_x_scale, eyeball_y_scale, 1.0);
    gluDisk(qobj, 0.0, outer_radius, num_slices, num_loops);
    glScalef(pupil_x_scale, pupil_y_scale, 1.0);
    gluDisk(qobj, 0.0, outer_radius, num_slices, num_loops);
    glPopMatrix();

    // Draw right eye.
    glPushMatrix();
    glTranslatef(right_eye_x, itsEyeHeight(), 0.0);
    glScalef(eyeball_x_scale, eyeball_y_scale, 1.0);
    gluDisk(qobj, 0.0, outer_radius, num_slices, num_loops);
    glScalef(pupil_x_scale, pupil_y_scale, 1.0);
    gluDisk(qobj, 0.0, outer_radius, num_slices, num_loops);
    glPopMatrix();

    // Draw face outline.
    glEnable(GL_MAP1_VERTEX_3);
    for (int i = 0; i < nctrlsets; ++i) {
      glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpnts[i*12]);
      // Define a 1-d evaluator for the Bezier curves .
      glBegin(GL_LINE_STRIP);
      for (int j = 0; j <= num_subdivisions; ++j) {
        glEvalCoord1f((GLfloat) j/ (GLfloat) num_subdivisions);
      }
      glEnd();
    }
    
    // Draw nose and mouth.
    glBegin(GL_LINES);
    glVertex2f(theirMouth_x[0], itsMouthHeight());
    glVertex2f(theirMouth_x[1], itsMouthHeight());
    glVertex2f(theirNose_x, nose_bottom_y);
    glVertex2f(theirNose_x, nose_top_y);
    glEnd();

    if (have_antialiasing) {
      glPopAttrib();
    }
  glEndList();

  grPostCompiled();
}

///////////////
// accessors //
///////////////

const float* Face::getCtrlPnts() const {
DOTRACE("Face::getCtrlPnts");
  static const float ctrlpnts[] = {    -.7, 0.2, 0, // first 4 control points 
                                       -.7, 1.4, 0,
                                       .7, 1.4, 0,
                                       .7, 0.2, 0,
                                       .7, 0.2, 0, // second 4 control points 
                                       .6, -1.7, 0,
                                       -.6, -1.7, 0,
                                       -.7, 0.2, 0};
  return ctrlpnts;
}

float Face::getEyeAspect() const {
DOTRACE("Face::getEyeAspect");
  return 0.0;
}

float Face::getVertOffset() const {
DOTRACE("Face::getVertOffset");
  return 0.0;
}



bool Face::check() const {
DOTRACE("Face::check");
  // nothing to be checked in current implementation, so just...
  return true;
}

void Face::makeIoList(vector<IO *>& vec) {
DOTRACE("Face::makeIoList");
  vec.clear();

  vec.push_back(&itsCategory);
  vec.push_back(&itsEyeHeight);
  vec.push_back(&itsEyeDistance);
  vec.push_back(&itsNoseLength);
  vec.push_back(&itsMouthHeight);
}

void Face::makeIoList(vector<const IO *>& vec) const {
DOTRACE("Face::makeIoList const");
  vec.clear();

  vec.push_back(&itsCategory);
  vec.push_back(&itsEyeHeight);
  vec.push_back(&itsEyeDistance);
  vec.push_back(&itsNoseLength);
  vec.push_back(&itsMouthHeight);
}

static const char vcid_face_cc[] = "$Header$";
#endif // !FACE_CC_DEFINED
