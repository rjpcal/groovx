///////////////////////////////////////////////////////////////////////
//
// morphyface.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 15:38:42 1999
// written: Mon Jun 11 15:08:17 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACE_CC_DEFINED
#define MORPHYFACE_CC_DEFINED

#ifdef ACC_COMPILER
#pragma OPT_LEVEL 1
#endif

#include "morphyface.h"

#include "bezier.h"
#include "rect.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "gwt/canvas.h"

#include <cstring>
#include <GL/gl.h>
#include <GL/glu.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope definitions
//
///////////////////////////////////////////////////////////////////////

namespace {
  template <class T>
  inline T abs(T val) { return (val < 0) ? -val : val; }

  const IO::VersionId MFACE_SERIAL_VERSION_ID = 1;

  const char* ioTag = "MorphyFace";

  typedef MorphyFace MF;

  const FieldInfo FINFOS[] = {
	 FieldInfo("category", &MF::mfaceCategory, 0, 0, 10, 1, true),

	 FieldInfo("faceWidth", &MF::faceWidth, 2.75, 1.5, 3.5, 0.1),
	 FieldInfo("topWidth", &MF::topWidth, 1.15, 0.05, 2.0, 0.05),
	 FieldInfo("bottomWidth", &MF::bottomWidth, 1.0, 0.05, 2.0, 0.05),
	 FieldInfo("topHeight", &MF::topHeight, 3.8, 0.5, 5.0, 0.25),
	 FieldInfo("bottomHeight", &MF::bottomHeight, -3.0, -5.0, -0.5, 0.25),

	 FieldInfo("hairWidth", &MF::hairWidth, 0.20, 0.00, 0.5, 0.02, true),
	 FieldInfo("hairStyle", &MF::hairStyle, 0, 0, 1, 1),

	 FieldInfo("eyeYpos", &MF::eyeYpos, 0.375, -2.0, 2.0, 0.1, true),
	 FieldInfo("eyeDistance", &MF::eyeDistance, 2.25, 0.0, 5.0, 0.25),
	 FieldInfo("eyeHeight", &MF::eyeHeight, 0.9, 0.0, 2.0, 0.1),
	 FieldInfo("eyeAspectRatio", &MF::eyeAspectRatio, 1.555556, 0.1, 5.0, 0.1),

	 FieldInfo("pupilXpos", &MF::pupilXpos, 0.0, -0.5, 0.5, 0.05, true),
	 FieldInfo("pupilYpos", &MF::pupilYpos, 0.0, -0.5, 0.5, 0.05),
	 FieldInfo("pupilSize", &MF::pupilSize, 0.6, 0.0, 1.0, 0.05),
	 FieldInfo("pupilDilation", &MF::pupilDilation, 0.5, 0.0, 1.0, 0.05),

	 FieldInfo("eyebrowXpos", &MF::eyebrowXpos, 0.0, -0.5, 0.5, 0.02, true),
	 FieldInfo("eyebrowYpos", &MF::eyebrowYpos, 0.5, 0.0, 1.5, 0.05),
	 FieldInfo("eyebrowCurvature", &MF::eyebrowCurvature, 0.8, -2.0, 2.0, 0.1),
	 FieldInfo("eyebrowAngle", &MF::eyebrowAngle, -5, -50, 50, 1),
	 FieldInfo("eyebrowThickness", &MF::eyebrowThickness, 2.0, 0.1, 4.0, 0.1),

	 FieldInfo("noseXpos", &MF::noseXpos, 0.0, -1.0, 1.0, 0.05, true),
	 FieldInfo("noseYpos", &MF::noseYpos, -0.825, -2.0, 2.0, 0.1),
	 FieldInfo("noseLength", &MF::noseLength, 0.75, 0.0, 2.0, 0.1),
	 FieldInfo("noseWidth", &MF::noseWidth, 1.5, 0.0, 3.0, 0.1),

	 FieldInfo("mouthXpos", &MF::mouthXpos, 0.0, -2.0, 2.0, 0.1, true),
	 FieldInfo("mouthYpos", &MF::mouthYpos, -2.0, -3.0, 1.0, 0.1),
	 FieldInfo("mouthWidth", &MF::mouthWidth, 2.5, 0.0, 5.0, 0.25),
	 FieldInfo("mouthCurvature", &MF::mouthCurvature, 0.6, -2.0, 2.0, 0.1)
  };

  const unsigned int NUM_FINFOS = sizeof(FINFOS)/sizeof(FieldInfo);

  const FieldMap MFACE_FIELDS(FINFOS, FINFOS+NUM_FINFOS);

  void draw_hairstyle1(GWT::Canvas& canvas) {
	 const GLdouble hair_vertices[] = {
		-0.955, 0.293,
		-1.000, 0.000,
		-1.273, 0.206,
		-1.273, 0.382,
		-0.550, 1.018,
		-0.118, 0.618,
		0.364, 1.117,
		0.455, 0.764,
		0.682, 0.945,
		0.591, 0.587,
		1.182, 0.529,
		0.955, 0.294,
		1.210, 0.265,
		1.000, 0.059,
// 	 // Individual hair line 1
		-0.455, 0.824,				  //	 -1.25 3.5
		-0.182, 0.921,				  // 	 -0.5 4.125
		0.273, 0.950,				  // 	 0.75 4.25
		0.545, 0.900,				  // 	 1.5 3.825
// 	 // Individual hair line 2
		0.455, 0.765,				  // 	 1.25 3.25
		0.300, 0.588,				  // 	 0.825 2.5
		0.091, 0.500,				  // 	 0.25 2.125
		-0.300, 0.429,				  // 	 -0.825 1.825
	 };

	 glEnableClientState(GL_VERTEX_ARRAY);
	 {
		glVertexPointer(2, GL_DOUBLE, 0, hair_vertices);

		glPushAttrib(GL_POLYGON_BIT);
		{
		  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		  glBegin(GL_TRIANGLE_FAN);
		  {
			 glArrayElement(0);
			 glArrayElement(1);
			 glArrayElement(2);
			 glArrayElement(3);
			 glArrayElement(4);
			 glArrayElement(5);
		  }
		  glEnd();

		  glBegin(GL_TRIANGLE_STRIP);
		  {
			 glArrayElement(4);
			 glArrayElement(5);
			 glArrayElement(6);
			 glArrayElement(7);
			 glArrayElement(8);
			 glArrayElement(9);
			 glArrayElement(10);
			 glArrayElement(11);
			 glArrayElement(12);
			 glArrayElement(13);
		  }
		  glEnd();

		  // Second hair line
		  glBegin(GL_LINE_STRIP);
		  {
			 glArrayElement(18);
			 glArrayElement(19);
			 glArrayElement(20);
			 glArrayElement(21);
		  }
		  glEnd();

		  // First hair line
		  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
		  {
			 canvas.swapForeBack();

			 glBegin(GL_LINE_STRIP);
			 {
				glArrayElement(14);
				glArrayElement(15);
				glArrayElement(16);
				glArrayElement(17);
			 }
			 glEnd();
		  }
		  glPopAttrib();

		}
		glPopAttrib();

	 }
	 glDisableClientState(GL_VERTEX_ARRAY);
  }
}

///////////////////////////////////////////////////////////////////////
//
// MorphyFace member functions
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Creators
///////////////////////////////////////////////////////////////////////

const FieldMap& MorphyFace::classFields() { return MFACE_FIELDS; }

MorphyFace* MorphyFace::make() {
DOTRACE("MorphyFace::make");
  return new MorphyFace;
}

MorphyFace::MorphyFace() :
  GrObj(GLCOMPILE, CLEAR_BOUNDING_BOX),
  FieldContainer(),

  mfaceCategory(this, 0),

  faceWidth(this, 2.75),
  topWidth(this, 1.15),
  bottomWidth(this, 1.0),
  topHeight(this, 3.8),
  bottomHeight(this, -3.0),

  hairWidth(this, 0.20),
  hairStyle(this, 0),

  eyeYpos(this, 0.375),
  eyeDistance(this, 2.25),
  eyeHeight(this, 0.9),
  eyeAspectRatio(this, 1.555556),

  pupilXpos(this, 0.0),
  pupilYpos(this, 0.0),
  pupilSize(this, 0.6),
  pupilDilation(this, 0.5, 0.0, 0.999),

  eyebrowXpos(this, 0.0),
  eyebrowYpos(this, 0.5),
  eyebrowCurvature(this, 0.8),
  eyebrowAngle(this, -5),
  eyebrowThickness(this, 2.0),

  noseXpos(this, 0.0),
  noseYpos(this, -0.825),
  noseLength(this, 0.75),
  noseWidth(this, 1.5),

  mouthXpos(this, 0.0),
  mouthYpos(this, -2.0),
  mouthWidth(this, 2.5),
  mouthCurvature(this, 0.6)
{
DOTRACE("MorphyFace::MorphyFace");
  Invariant(check());

  setFieldMap(MFACE_FIELDS);
}

MorphyFace::~MorphyFace() {
DOTRACE("MorphyFace::~MorphyFace");
  // nothing to do
}

IO::VersionId MorphyFace::serialVersionId() const {
DOTRACE("MorphyFace::serialVersionId");
  return MFACE_SERIAL_VERSION_ID;
}

void MorphyFace::readFrom(IO::Reader* reader) {
DOTRACE("MorphyFace::readFrom");

  reader->ensureReadVersionId("MorphyFace", 1, "Try grsh0.8a4");

  readFieldsFrom(reader, classFields());

  reader->readBaseClass("GrObj", IO::makeProxy<GrObj>(this));
}

void MorphyFace::writeTo(IO::Writer* writer) const {
DOTRACE("MorphyFace::writeTo");

  writer->ensureWriteVersionId("MorphyFace", MFACE_SERIAL_VERSION_ID, 1,
										 "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());

  writer->writeBaseClass("GrObj", IO::makeConstProxy<GrObj>(this));
}


///////////////////////////////////////////////////////////////////////
// Actions
///////////////////////////////////////////////////////////////////////

void MorphyFace::grRender(GWT::Canvas& canvas, DrawMode mode) const {
DOTRACE("MorphyFace::grRender");
  Invariant(check());

  // Enable antialiasing, if it is available
  const bool have_antialiasing = canvas.isRgba();

  if (have_antialiasing) {
	 glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
	 glEnable(GL_BLEND); // blend incoming RGBA values with old RGBA values

	 if (mode == DRAW)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // use transparency
	 else
		glBlendFunc(GL_ONE, GL_ZERO); // overwrite source into destination

	 glEnable(GL_LINE_SMOOTH);   // use anti-aliasing 
  }

  // Prepare to work with modelview matrices.
  glMatrixMode(GL_MODELVIEW);  


  //
  // Draw eyes
  //

  // Create a quadric obj to use for calling gluDisk(). This disk will
  // be used to render the eyeballs and the pupils.
  GLUquadricObj* qobj = gluNewQuadric();
  gluQuadricDrawStyle(qobj, GLU_FILL);

  const double eye_ctrlpnts[] = {
	 -3.0/7.0, 0.0,     0.0,
	 -2.0/7.0, 2.0/3.0, 0.0,
	  2.0/7.0, 2.0/3.0, 0.0,
	  4.0/7.0, 0.0,     0.0,
  };
  const int eye_subdivisions = 10;

  for (int left_right = -1; left_right < 2; left_right += 2) {
	 glPushMatrix();
	   // Do appropriate reflection
	   glScalef(left_right*1.0, 1.0, 1.0);

		// Move to the eye position
		glTranslatef(abs(eyeDistance())/2.0, eyeYpos(), 0.0);

		// Draw eye outline
		for (int top_bottom = -1; top_bottom < 2; top_bottom += 2) {
		  glPushMatrix();
		    glScalef(eyeHeight()*eyeAspectRatio(),
						 eyeHeight()*top_bottom,
						 1.0);
			 glEnable(GL_MAP1_VERTEX_3);
			 glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, eye_ctrlpnts);
			 // Evaluate the 1-d Bezier curve
			 glMapGrid1d(eye_subdivisions, 0.0, 1.0);
			 glEvalMesh1(GL_LINE, 0, eye_subdivisions);
		  glPopMatrix();
		}

		// Draw eyebrow
		glPushMatrix();
		  glTranslatef(eyebrowXpos(), eyebrowYpos(), 0.0);
		  glRotatef(eyebrowAngle(), 0.0, 0.0, 1.0);
		  glScalef(eyeHeight()*eyeAspectRatio(),
					  eyeHeight()*eyebrowCurvature(),
					  1.0);
		  glPushAttrib(GL_LINE_BIT);
		  {
			 GLdouble line_width;
			 glGetDoublev(GL_LINE_WIDTH, &line_width);
			 glLineWidth(eyebrowThickness()*line_width);
			 glEnable(GL_MAP1_VERTEX_3);
			 glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, eye_ctrlpnts);
			 // Evaluate the 1-d Bezier curve
			 glMapGrid1d(eye_subdivisions, 0.0, 1.0);
			 glEvalMesh1(GL_LINE, 0, eye_subdivisions);
		  }
		  glPopAttrib();
		glPopMatrix();

		// Draw pupil
		glPushMatrix();
		  glTranslatef(left_right*pupilXpos(), pupilYpos(), 0.0);
		  glScalef(pupilSize()*eyeHeight(),
					  pupilSize()*eyeHeight(),
					  1.0);
		  static const int num_slices = 20;
		  static const int num_loops = 1;
		  gluDisk(qobj, 0.5*abs(pupilDilation()), 0.5, num_slices, num_loops);
		glPopMatrix();

	 glPopMatrix();
  }

  gluDeleteQuadric(qobj);

  //
  // Draw face outline.
  //

  // These parameters control the generation of the Bezier curve for
  // the face outline.
  static const int num_subdivisions = 30;
  static const int nctrlsets = 2;
  const double ctrlpnts[] = {
    -faceWidth(), 0.0,        0,        // first 4 control points 
	 -topWidth()*faceWidth(),  topHeight()*4.0/3.0, 0,
	 topWidth()*faceWidth(),   topHeight()*4.0/3.0, 0,
	 faceWidth(),  0.0,        0,
	 faceWidth(), 0.0, 0, // second 4 control points 
	 bottomWidth()*faceWidth(), bottomHeight()*4.0/3.0, 0,
	 -bottomWidth()*faceWidth(), bottomHeight()*4.0/3.0, 0,
	 -faceWidth(), 0.0, 0};

  glEnable(GL_MAP1_VERTEX_3);
  for (int i = 1; i < nctrlsets; ++i) {
	 glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpnts[i*12]);
	 // Evaluate the 1-d Bezier curve
	 glMapGrid1d(num_subdivisions, 0.0, 1.0);
	 glEvalMesh1(GL_LINE, 0, num_subdivisions);
  }

  //
  // Draw nose.
  //

  glPushMatrix();
    glTranslatef(noseXpos(), noseYpos(), 0.0);
	 glScalef(abs(noseWidth())/2.0, abs(noseLength()), 1.0);
	 glBegin(GL_LINE_STRIP);
	   glVertex2d(-0.75, 0.5);
		glVertex2d(-1.0,  0.0);
		glVertex2d(-0.75, -0.333333);
		glVertex2d(-0.25, -0.333333);
		glVertex2d( 0.0, -0.5);	  // CENTER
		glVertex2d( 0.25, -0.333333);
		glVertex2d( 0.75, -0.333333);
		glVertex2d( 1.0,  0.0);
	   glVertex2d( 0.75, 0.5);
	 glEnd();
  glPopMatrix();

  //
  // Draw mouth.
  //

  const double mouth_ctrlpnts[] = {
    -0.5,  0.5,      0.0,
	 -0.2, -0.833333, 0.0,
	  0.2, -0.833333, 0.0,
	  0.5,  0.5,      0.0
  };

  glPushMatrix();
    glTranslatef(mouthXpos(), mouthYpos(), 0.0);
	 glScalef(mouthWidth(), mouthCurvature(), 1.0);
	 glEnable(GL_MAP1_VERTEX_3);
	 glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, mouth_ctrlpnts);
	 // Evaluate the 1-d Bezier curve
	 glMapGrid1d(num_subdivisions, 0.0, 1.0);
	 glEvalMesh1(GL_LINE, 0, num_subdivisions);
  glPopMatrix();

  //
  // Draw hair.
  //

  Bezier4 xbezier(-1.0, -topWidth(), topWidth(), 1.0);
  Bezier4 ybezier( 0.0,     4.0/3.0   ,   4.0/3.0  , 0.0);

  const int num_hair_points = 15;
  double hair_widths[num_hair_points]   = { 
	 0.40, 0.90, 0.95,
	 1.00, 0.97, 0.94,
	 0.91, 0.88, 0.85,
	 0.81, 0.78, 0.75,
	 0.7, 0.6, 0.0 };
  double hair_x_normals[num_hair_points];
  double hair_y_normals[num_hair_points];

  double hair_vertices[num_hair_points*4];

  {
	 for (int i = 0; i < num_hair_points; ++i) {
		double u = 0.5 + double(i)/double(2*num_hair_points-2);

		double x = xbezier.eval(u);
		double y = ybezier.eval(u);

		double tang_x = xbezier.evalDeriv(u);
		double tang_y = ybezier.evalDeriv(u);

		// tangent vector = (tang_x, tang_y)
		// ==> normal vector = (-tang_y, tang_x)
		double norm_x = -tang_y;
		double norm_y = tang_x;

		// compute the factor needed to make a unit normal vector
		double norm_factor = 1.0 / sqrt(norm_x*norm_x + norm_y*norm_y);

		hair_x_normals[i] = norm_x * norm_factor;
		hair_y_normals[i] = norm_y * norm_factor;

		hair_vertices[4*i]   = // inner x value
		  x - hair_x_normals[i]*hair_widths[i]*hairWidth();
		hair_vertices[4*i+1] = // inner y value
		  y - hair_y_normals[i]*hair_widths[i]*hairWidth();
		hair_vertices[4*i+2] = // outer x value
		  x + hair_x_normals[i]*hair_widths[i]*hairWidth();
		hair_vertices[4*i+3] = // outer y value
		  y + hair_y_normals[i]*hair_widths[i]*hairWidth();
	 }
  }

  glPushAttrib(GL_POLYGON_BIT);
  {
	 if (hairStyle() == 0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	 }
	 else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	 }

	 // Draw right side
	 glPushMatrix();
	 {
		glScalef(faceWidth(), topHeight(), 1.0);

		glBegin(GL_QUAD_STRIP);
		{
		  for (int i = 0; i < num_hair_points; ++i) {
			 glVertex2d(hair_vertices[4*i  ], hair_vertices[4*i+1]);
			 glVertex2d(hair_vertices[4*i+2], hair_vertices[4*i+3]);
		  }
		}
		glEnd();

	 }
	 glPopMatrix();

	 // Draw left side
	 glPushMatrix();
	 {
		glScalef(-faceWidth(), topHeight(), 1.0);

		glBegin(GL_QUAD_STRIP);
		{
		  for (int i = 0; i < num_hair_points; ++i) {
			 glVertex2d(hair_vertices[4*i  ], hair_vertices[4*i+1]);
			 glVertex2d(hair_vertices[4*i+2], hair_vertices[4*i+3]);
		  }
		}
		glEnd();

	 }
	 glPopMatrix();

  }
  glPopAttrib();

  // Undo pushes
  if (have_antialiasing) {
	 glPopAttrib();
  }
}

///////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////

void MorphyFace::grGetBoundingBox(Rect<double>& bbox,
											 int& border_pixels) const {
DOTRACE("MorphyFace::grGetBoundingBox");
  Bezier4 xbezier_top(-1.0, -topWidth(), topWidth(), 1.0);
  Bezier4 xbezier_bottom(1.0, bottomWidth(), -bottomWidth(), -1.0);

  double top_width = xbezier_top.evalMax();
  double bottom_width = xbezier_bottom.evalMax();

  DebugEval(top_width);   DebugEvalNL(bottom_width);

  double max_width = max(1.0, max(top_width, bottom_width)); 

  DebugEvalNL(max_width);

  bbox.left()   = -max_width      * faceWidth() * (1 + hairWidth());
  bbox.right()  =  max_width      * faceWidth() * (1 + hairWidth());
  bbox.top()    =  topHeight() * (1 + hairWidth());
  bbox.bottom() =  bottomHeight();

  border_pixels = 4;
}

bool MorphyFace::grHasBoundingBox() const {
DOTRACE("MorphyFace::grHasBoundingBox");
  return true;
}

bool MorphyFace::check() const {
DOTRACE("MorphyFace::check");
  return true;
}


static const char vcid_morphyface_cc[] = "$Header$";
#endif // !MORPHYFACE_CC_DEFINED
