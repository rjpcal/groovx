///////////////////////////////////////////////////////////////////////
//
// morphyface.cc
// Rob Peters
// created: Wed Sep  8 15:38:42 1999
// written: Wed Dec  1 11:53:12 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACE_CC_DEFINED
#define MORPHYFACE_CC_DEFINED

#ifdef ACC_COMPILER
#pragma OPT_LEVEL 1
#endif

#include "morphyface.h"

#include <iostream.h>           // for serialize
#include <iomanip.h>				  // for setw in serialize
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cctype>
#include <cmath>

#include "bezier.h"
#include "gfxattribs.h"
#include "reader.h"
#include "rect.h"
#include "writer.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#define LOCAL_INVARIANT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope definitions
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string ioTag = "MorphyFace";

  void draw_hairstyle1() {
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
			 GrObj::swapForeBack();
		  
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

MorphyFace::MorphyFace() :
  GrObj(GROBJ_GL_COMPILE, GROBJ_CLEAR_BOUNDING_BOX)
{
DOTRACE("MorphyFace::MorphyFace");

  faceWidth() = 2.75; 
  topHeight() = 3.8;
  bottomHeight() = -3.0;
  topWidth() = 1.15;
  bottomWidth() = 1.0;

  hairWidth() = 0.20;
  hairStyle() = 0;

  eyeYpos() = 0.375;
  eyeDistance() = 2.25;
  eyeHeight() = 0.9;
  eyeAspectRatio() = 1.555556;

  pupilXpos() = 0.0;
  pupilYpos() = 0.0;
  pupilSize() = 0.6;
  pupilDilation() = 0.5;

  eyebrowXpos() = 0.0;
  eyebrowYpos() = 0.5;
  eyebrowCurvature() = 0.8;
  eyebrowAngle() = -5;
  eyebrowThickness() = 2.0;

  noseXpos() = 0.0;
  noseYpos() = -0.825;
  noseLength() = 0.75;
  noseWidth() = 1.5;

  mouthXpos() = 0.0;
  mouthYpos() = -2.0;
  mouthWidth() = 2.5;
  mouthCurvature() = 0.6;

  Invariant(check());
}

// read the object's state from an input stream. The input stream must
// already be open and connected to an appropriate file.
MorphyFace::MorphyFace(istream& is, IOFlag flag) :
  GrObj(GROBJ_GL_COMPILE, GROBJ_CLEAR_BOUNDING_BOX)
{
DOTRACE("MorphyFace::MorphyFace(istream&, IOFlag)");
  deserialize(is, flag);
  Invariant(check());
}

MorphyFace::~MorphyFace() {
DOTRACE("MorphyFace::~MorphyFace");
  // nothing to do
}

// Writes the object's state to an output stream. The output stream
// must already be open and connected to an appropriate file.
void MorphyFace::serialize(ostream &os, IOFlag flag) const {
DOTRACE("MorphyFace::serialize");
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

  if (flag & BASES) { GrObj::serialize(os, flag | TYPENAME); }
}

void MorphyFace::deserialize(istream &is, IOFlag flag) {
DOTRACE("MorphyFace::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  IO::eatWhitespace(is);
  int version = 0;

  if ( is.peek() == '@' ) {
	 int c = is.get();
	 Assert(c == '@');

	 is >> version;
	 DebugEvalNL(version);
  }

  if (version == 0) {
	 vector<IO *> ioList;
	 makeIoList(ioList);
	 for (vector<IO *>::iterator ii = ioList.begin(); ii != ioList.end(); ii++) {
		(*ii)->deserialize(is, flag);
	 }
  }
  else if (version == 1) {
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

  if (flag & BASES) { GrObj::deserialize(is, flag | TYPENAME); }

  sendStateChangeMsg();
}

int MorphyFace::charCount() const {
DOTRACE("MorphyFace::charCount");
  return (ioTag.length() + 1
			 + 3 // version
			 + 2 // brace
			 + 128 // params
			 + 2 // brace
			 + GrObj::charCount()
			 + 1);//fudge factor
}

void MorphyFace::readFrom(Reader* reader) {
DOTRACE("MorphyFace::readFrom");
  const vector<PInfo>& infos = getPropertyInfos();
  for (size_t i = 0; i < infos.size(); ++i) {
	 reader->readValueObj(infos[i].name, const_cast<Value&>(get(infos[i].property)));
  }

  GrObj::readFrom(reader);
}

void MorphyFace::writeTo(Writer* writer) const {
DOTRACE("MorphyFace::writeTo");
  const vector<PInfo>& infos = getPropertyInfos();
  for (size_t i = 0; i < infos.size(); ++i) {
	 writer->writeValueObj(infos[i].name, get(infos[i].property));
  }

  GrObj::writeTo(writer);
}

///////////////////////////////////////////////////////////////////////
//
// Properties
//
///////////////////////////////////////////////////////////////////////

const vector<MorphyFace::PInfo>& MorphyFace::getPropertyInfos() {
DOTRACE("MorphyFace::getPropertyInfos");
  static vector<PInfo> p;

  // just to shorten up the line lengths below
  typedef MorphyFace MF;

  if (p.size() == 0) {
	 p.push_back(PInfo("category", &MF::category, 0, 10, 1, true));

	 p.push_back(PInfo("faceWidth", &MF::faceWidth, 1.5, 3.5, 0.1));
	 p.push_back(PInfo("topWidth", &MF::topWidth, 0.05, 2.0, 0.05));
	 p.push_back(PInfo("bottomWidth", &MF::bottomWidth, 0.05, 2.0, 0.05));
	 p.push_back(PInfo("topHeight", &MF::topHeight, 0.5, 5.0, 0.25));
	 p.push_back(PInfo("bottomHeight", &MF::bottomHeight, -5.0, -0.5, 0.25));
	 
	 p.push_back(PInfo("hairWidth", &MF::hairWidth, 0.00, 0.5, 0.02, true));
	 p.push_back(PInfo("hairStyle", &MF::hairStyle, 0, 1, 1));
	 
	 p.push_back(PInfo("eyeYpos", &MF::eyeYpos, -2.0, 2.0, 0.1, true));
	 p.push_back(PInfo("eyeDistance", &MF::eyeDistance, 0.0, 5.0, 0.25));
	 p.push_back(PInfo("eyeHeight", &MF::eyeHeight, 0.0, 2.0, 0.1));
	 p.push_back(PInfo("eyeAspectRatio", &MF::eyeAspectRatio, 0.1, 5.0, 0.1));
	 
	 p.push_back(PInfo("pupilXpos", &MF::pupilXpos, -0.5, 0.5, 0.05, true));
	 p.push_back(PInfo("pupilYpos", &MF::pupilYpos, -0.5, 0.5, 0.05));
	 p.push_back(PInfo("pupilSize", &MF::pupilSize, 0.0, 1.0, 0.05));
	 p.push_back(PInfo("pupilDilation", &MF::pupilDilation, 0.0, 1.0, 0.05));
	 
	 p.push_back(PInfo("eyebrowXpos", &MF::eyebrowXpos, -0.5, 0.5, 0.02, true));
	 p.push_back(PInfo("eyebrowYpos", &MF::eyebrowYpos, 0.0, 1.5, 0.05));
	 p.push_back(PInfo("eyebrowCurvature", &MF::eyebrowCurvature, -2.0, 2.0, 0.1));
	 p.push_back(PInfo("eyebrowAngle", &MF::eyebrowAngle, -50, 50, 1));
	 p.push_back(PInfo("eyebrowThickness", &MF::eyebrowThickness, 0.1, 4.0, 0.1));
	
	 p.push_back(PInfo("noseXpos", &MF::noseXpos, -1.0, 1.0, 0.05, true));
	 p.push_back(PInfo("noseYpos", &MF::noseYpos, -2.0, 2.0, 0.1));
	 p.push_back(PInfo("noseLength", &MF::noseLength, 0.0, 2.0, 0.1));
	 p.push_back(PInfo("noseWidth", &MF::noseWidth, 0.0, 3.0, 0.1));
	 
	 p.push_back(PInfo("mouthXpos", &MF::mouthXpos, -2.0, 2.0, 0.1, true));
	 p.push_back(PInfo("mouthYpos", &MF::mouthYpos, -3.0, 1.0, 0.1));
	 p.push_back(PInfo("mouthWidth", &MF::mouthWidth, 0.0, 5.0, 0.25));
	 p.push_back(PInfo("mouthCurvature", &MF::mouthCurvature, -2.0, 2.0, 0.1));

  }
  return p;
}


///////////////////////////////////////////////////////////////////////
// Actions
///////////////////////////////////////////////////////////////////////

void MorphyFace::grRender() const {
DOTRACE("MorphyFace::grRender");
  Invariant(check());

  // Enable antialiasing, if it is available
  const bool have_antialiasing = GfxAttribs::usingRgba();

  if (have_antialiasing) {
	 glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
	 glEnable(GL_BLEND); // blend incoming RGBA values with old RGBA values
	 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // use transparency 
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
		  gluDisk(qobj, 0.5*pupilDilation(), 0.5, num_slices, num_loops);
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

bool MorphyFace::grGetBoundingBox(Rect<double>& bbox,
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

  return true;
}

bool MorphyFace::check() const {
DOTRACE("MorphyFace::check");
  return (eyeDistance() >= 0.0 && noseLength() >= 0.0);
}

void MorphyFace::makeIoList(vector<IO *>& vec) {
DOTRACE("MorphyFace::makeIoList");
  makeIoList(reinterpret_cast<vector<const IO *> &>(vec));
}

void MorphyFace::makeIoList(vector<const IO *>& vec) const {
DOTRACE("MorphyFace::makeIoList const");
  vec.clear();

  vec.push_back(&category);

  vec.push_back(&faceWidth);
  vec.push_back(&topWidth);
  vec.push_back(&bottomWidth);
  vec.push_back(&topHeight);
  vec.push_back(&bottomHeight);

  vec.push_back(&hairWidth);
  vec.push_back(&hairStyle);

  vec.push_back(&eyeYpos);
  vec.push_back(&eyeDistance);
  vec.push_back(&eyeHeight);
  vec.push_back(&eyeAspectRatio);

  vec.push_back(&pupilXpos);
  vec.push_back(&pupilYpos);
  vec.push_back(&pupilSize);
  vec.push_back(&pupilDilation);

  vec.push_back(&eyebrowXpos);
  vec.push_back(&eyebrowYpos);
  vec.push_back(&eyebrowCurvature);
  vec.push_back(&eyebrowAngle);
  vec.push_back(&eyebrowThickness);

  vec.push_back(&noseXpos);
  vec.push_back(&noseYpos);
  vec.push_back(&noseLength);
  vec.push_back(&noseWidth);

  vec.push_back(&mouthXpos);
  vec.push_back(&mouthYpos);
  vec.push_back(&mouthWidth);
  vec.push_back(&mouthCurvature);
}

static const char vcid_morphyface_cc[] = "$Header$";
#endif // !MORPHYFACE_CC_DEFINED
