///////////////////////////////////////////////////////////////////////
//
// face.cc
// Rob Peters
// created: Dec-98
// written: Sat Jul  3 16:34:47 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACE_CC_DEFINED
#define FACE_CC_DEFINED

#include "face.h"

#include <iostream.h>           // for serialize
#include <iomanip.h>				  // for setw in serialize
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cctype>

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
  const double theirNose_x = 0.0;
  const double theirMouth_x[2] = {-0.2, 0.2};

  const string ioTag = "Face";

  void eatWhitespace(istream& is) {
	 while ( isspace(is.peek()) ) {
		is.get();
	 }
  }
}

///////////////////////////////////////////////////////////////////////
//
// Face member functions
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Creators
///////////////////////////////////////////////////////////////////////

Face::Face(double eh, double es, double nl, double mh, int categ) {
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
  // nothing to do
}

// Writes the object's state to an output stream. The output stream
// must already be open and connected to an appropriate file.
void Face::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Face::serialize");
  Invariant(check());

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  // version
  os << "@1" << sep;

  os << '{' << sep;

  vector<const IO *> ioList;
  makeIoList(ioList);
  for (vector<const IO *>::const_iterator ii = ioList.begin(); 
		 ii != ioList.end(); ii++) {
	 (*ii)->serialize(os, flag);
  }
  os << '}' << endl;
  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { GrObj::serialize(os, flag); }
}

void Face::deserialize(istream &is, IOFlag flag) {
DOTRACE("Face::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  eatWhitespace(is);
  int version = 0;

  if ( is.peek() == '@' ) {
	 int c = is.get();
	 Assert(c == '@');

	 is >> version;
	 DebugEvalNL(version);
  }

  if (version == 0) {
	 // Format is:
	 // Face $category $eyeheight $eyedistance $noselength $mouthheight
	 vector<IO *> ioList;
	 makeIoList(ioList);
	 for (vector<IO *>::iterator ii = ioList.begin(); ii != ioList.end(); ii++) {
		(*ii)->deserialize(is, flag);
	 }
  }
  else if (version == 1) {
	 // Format is:
	 // Face { $category $eyeheight $eyedistance $noselength $mouthheight }
	 char brace;
	 is >> brace;
	 if (brace != '{') { throw IoLogicError(ioTag + " missing left-brace"); }
	 vector<IO *> ioList;
	 makeIoList(ioList);
	 for (vector<IO *>::iterator ii = ioList.begin(); ii != ioList.end(); ii++) {
		(*ii)->deserialize(is, flag);
	 }
	 is >> brace;
	 if (brace != '}') { throw IoLogicError(ioTag + " missing right-brace"); }
  }
  else {
	 throw IoLogicError(ioTag + " unknown version");
  }

  // Mysterious bug in HP compiler (aCC) requires the following
  // contorted code; if the exception is not first caught in this
  // function, then re-thrown, it causes a bus error when compiled
  // with optimization on, although there is no problem with
  // optimization off. Somehow adding the catch and re-throw avoids
  // the problem, without changing the program's behavior.
  try {
	 if (is.fail()) throw InputError(ioTag);
  }
  catch (IoError&) { 
	 throw;
  }
  Invariant(check());

  if (flag & BASES) { GrObj::deserialize(is, flag); }
}

int Face::charCount() const {
DOTRACE("Face::charCount");
  return (ioTag.length() + 1
			 + 3 // version
			 + 2 // brace
			 + itsCategory.charCount() + 1
			 + itsEyeDistance.charCount() + 1
			 + itsNoseLength.charCount() + 1
			 + itsMouthHeight.charCount() + 1
			 + itsEyeHeight.charCount() + 1
			 + 2 // brace
			 + 1);//fudge factor
}

///////////////////////////////////////////////////////////////////////
// Actions
///////////////////////////////////////////////////////////////////////

void Face::grRender() const {
DOTRACE("Face::grRender");
  Invariant(check());

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
  const double left_eye_x = -abs(itsEyeDistance())/2.0;
  const double right_eye_x = -left_eye_x;

  // Calculate the y positions for the top and bottom of the nose
  // bottom always <= 0.0
  // top always >= 0.0
  const double nose_bottom_y = -abs(itsNoseLength())/2.0;
  const double nose_top_y = -nose_bottom_y;

  // Generate the eyeball scales on the basis of the eye aspect. The
  // eye aspect should range from 0.0 to 1.0 to control the eyeball x
  // and y scales from 0.1 to 0.185. The x and y scales are always at
  // opposite points within this range.
  const double eye_aspect = getEyeAspect();
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
  const double* const ctrlpnts = getCtrlPnts();
  
  const bool have_antialiasing = GfxAttribs::usingRgba();

  //
  // Drawing commands begin here...
  //

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
	 glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpnts[i*12]);
	 // Define a 1-d evaluator for the Bezier curves .
	 glBegin(GL_LINE_STRIP);
	 for (int j = 0; j <= num_subdivisions; ++j) {
		glEvalCoord1f((GLdouble) j/ (GLdouble) num_subdivisions);
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
}

///////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////

const double* Face::getCtrlPnts() const {
DOTRACE("Face::getCtrlPnts");
  static const double ctrlpnts[] = {    -.7, 0.2, 0, // first 4 control points 
                                       -.7, 1.4, 0,
                                       .7, 1.4, 0,
                                       .7, 0.2, 0,
                                       .7, 0.2, 0, // second 4 control points 
                                       .6, -1.7, 0,
                                       -.6, -1.7, 0,
                                       -.7, 0.2, 0};
  return ctrlpnts;
}

double Face::getEyeAspect() const {
DOTRACE("Face::getEyeAspect");
  return 0.0;
}

double Face::getVertOffset() const {
DOTRACE("Face::getVertOffset");
  return 0.0;
}

bool Face::check() const {
DOTRACE("Face::check");
  return (itsEyeDistance() >= 0.0 && itsNoseLength() >= 0.0);
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
