///////////////////////////////////////////////////////////////////////
//
// face.cc
// Rob Peters
// created: Dec-98
// written: Fri Nov  3 14:56:29 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACE_CC_DEFINED
#define FACE_CC_DEFINED

#include "face.h"

#include "rect.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "gwt/canvas.h"

#include "util/strings.h"

#include <cstring>
#include <GL/gl.h>
#include <GL/glu.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

#ifdef MIPSPRO_COMPILER
#  define SGI_IDIOT_CAST(to, from) reinterpret_cast<to>(from)
#else
#  define SGI_IDIOT_CAST(to, from) (from)
#endif

///////////////////////////////////////////////////////////////////////
//
// File scope data 
//
///////////////////////////////////////////////////////////////////////

namespace {
  template <class T>
  inline T abs(T val) { return (val < 0) ? -val : val; }

  const IO::VersionId FACE_SERIAL_VERSION_ID = 1;

  const double theirNose_x = 0.0;
  const double theirMouth_x[2] = {-0.2, 0.2};

  const string_literal ioTag("Face");

  const Face::PInfo PINFOS[] = {
	 Face::PInfo("category", SGI_IDIOT_CAST(Property Face::*, &Face::faceCategory)
					 , 0, 10, 1, true),
	 Face::PInfo("eyeHeight", SGI_IDIOT_CAST(Property Face::*, &Face::eyeHeight),
					 -1.2, 1.2, 0.1),
	 Face::PInfo("eyeDistance", SGI_IDIOT_CAST(Property Face::*, &Face::eyeDistance),
					 0.0, 1.8, 0.1),
	 Face::PInfo("noseLength", SGI_IDIOT_CAST(Property Face::*, &Face::noseLength),
					 -0.0, 3.0, 0.1),
	 Face::PInfo("mouthHeight", SGI_IDIOT_CAST(Property Face::*, &Face::mouthHeight),
					 -1.2, 1.2, 0.1)
  };

  const unsigned int NUM_PINFOS = sizeof(PINFOS)/sizeof(Face::PInfo);
}

///////////////////////////////////////////////////////////////////////
//
// Face member functions
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Creators
///////////////////////////////////////////////////////////////////////

Face* Face::make() {
DOTRACE("Face::make");
  return new Face;
}

Face::Face(double eh, double es, double nl, double mh, int categ) :
  faceCategory(categ),
  eyeHeight(eh),
  eyeDistance(es),
  noseLength(nl),
  mouthHeight(mh)
{
DOTRACE("Face::Face");
  Invariant(check());
}

Face::~Face() {
DOTRACE("Face::~Face");
  // nothing to do
}

IO::VersionId Face::serialVersionId() const {
DOTRACE("Face::serialVersionId");
  return FACE_SERIAL_VERSION_ID;
}

void Face::readFrom(IO::Reader* reader) {
DOTRACE("Face::readFrom");

  for (size_t i = 0; i < NUM_PINFOS; ++i) {
	 reader->readValueObj(PINFOS[i].name(),
								 const_cast<Value&>(get(PINFOS[i].property())));
  }

  IO::VersionId svid = reader->readSerialVersionId();
  if (svid == 0)
	 GrObj::readFrom(reader);
  else if (svid == 1)
	 {
		IO::IoProxy<GrObj> baseclass(this);
		reader->readBaseClass("GrObj", &baseclass);
	 }
}

void Face::writeTo(IO::Writer* writer) const {
DOTRACE("Face::writeTo");

  for (size_t i = 0; i < NUM_PINFOS; ++i) {
	 writer->writeValueObj(PINFOS[i].name_cstr(), get(PINFOS[i].property()));
  }

  if (FACE_SERIAL_VERSION_ID == 0)
	 GrObj::writeTo(writer);
  else if (FACE_SERIAL_VERSION_ID == 1)
	 {
		IO::ConstIoProxy<GrObj> baseclass(this);
		writer->writeBaseClass("GrObj", &baseclass);
	 }
}

///////////////////////////////////////////////////////////////////////
//
// Properties
//
///////////////////////////////////////////////////////////////////////

unsigned int Face::numPropertyInfos() {
DOTRACE("Face::numPropertyInfos");
  return NUM_PINFOS;
}

const Face::PInfo& Face::getPropertyInfo(unsigned int i) {
DOTRACE("Face::getPropertyInfo");
  return PINFOS[i];
}

///////////////////////////////////////////////////////////////////////
// Actions
///////////////////////////////////////////////////////////////////////

void Face::grRender(GWT::Canvas& canvas) const {
DOTRACE("Face::grRender");
  Invariant(check());

  const bool have_antialiasing = canvas.isRgba();

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
  
  //
  // Set up for drawing eyes.
  //

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
  const double left_eye_x = -abs(eyeDistance())/2.0;
  const double right_eye_x = -left_eye_x;

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

  // Draw left eye.
  glPushMatrix();
  glTranslatef(left_eye_x, eyeHeight(), 0.0); 
  glScalef(eyeball_x_scale, eyeball_y_scale, 1.0);
  gluDisk(qobj, 0.0, outer_radius, num_slices, num_loops);
  glScalef(pupil_x_scale, pupil_y_scale, 1.0);
  gluDisk(qobj, 0.0, outer_radius, num_slices, num_loops);
  glPopMatrix();
  
  // Draw right eye.
  glPushMatrix();
  glTranslatef(right_eye_x, eyeHeight(), 0.0);
  glScalef(eyeball_x_scale, eyeball_y_scale, 1.0);
  gluDisk(qobj, 0.0, outer_radius, num_slices, num_loops);
  glScalef(pupil_x_scale, pupil_y_scale, 1.0);
  gluDisk(qobj, 0.0, outer_radius, num_slices, num_loops);
  glPopMatrix();
  
  gluDeleteQuadric(qobj);

  //
  // Draw face outline.
  //

  // These parameters control the generation of the Bezier curve for
  // the face outline.
  static const int num_subdivisions = 30;
  static const int nctrlsets = 2;
  const double* const ctrlpnts = getCtrlPnts();
  
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
  
  //
  // Draw nose and mouth.
  //

  // Calculate the y positions for the top and bottom of the nose
  // bottom always <= 0.0
  // top always >= 0.0
  const double nose_bottom_y = -abs(noseLength())/2.0;
  const double nose_top_y = -nose_bottom_y;

  glBegin(GL_LINES);
  glVertex2f(theirMouth_x[0], mouthHeight());
  glVertex2f(theirMouth_x[1], mouthHeight());
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

void Face::grGetBoundingBox(Rect<double>& bounding_box,
									 int& border_pixels) const {
DOTRACE("Face::grGetBoundingBox");
  bounding_box.left() = -0.7;
  bounding_box.right() = 0.7;
  bounding_box.bottom() = 0.2 + 0.75*(-1.7-0.2);
  bounding_box.top() = 0.2 + 0.75*(1.4-0.2);
  border_pixels = 4;
}

bool Face::grHasBoundingBox() const {
DOTRACE("Face::grHasBoundingBox");  
  return true;
}

int Face::category() const {
DOTRACE("Face::category");
  return faceCategory.getNative();
}

void Face::setCategory(int val) {
DOTRACE("Face::setCategory");
  faceCategory.setNative(val);
}

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
//   return (eyeDistance() >= 0.0 && noseLength() >= 0.0);
  return true; 
}

static const char vcid_face_cc[] = "$Header$";
#endif // !FACE_CC_DEFINED
