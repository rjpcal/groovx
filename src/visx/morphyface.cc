///////////////////////////////////////////////////////////////////////
//
// morphyface.cc
// Rob Peters
// created: Wed Sep  8 15:38:42 1999
// written: Thu Sep  9 19:06:43 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACE_CC_DEFINED
#define MORPHYFACE_CC_DEFINED

#include "morphyface.h"

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
  const string ioTag = "MorphyFace";

  void eatWhitespace(istream& is) {
	 while ( isspace(is.peek()) ) {
		is.get();
	 }
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

MorphyFace::MorphyFace(double, double, double, double, int categ) {
DOTRACE("MorphyFace::MorphyFace");
  itsCategory() = categ;

  itsFaceWidth() = 2.75; 
  itsTopHeight() = 4.1;
  itsBottomHeight() = -3.0;
  itsTopWidth() = 2.75;
  itsBottomWidth() = 2.75;

  itsEyeYpos() = 0.375;
  itsEyeDistance() = 2.25;
  itsEyeHeight() = 0.9;
  itsEyeAspectRatio() = 1.555556;

  itsPupilXpos() = 0.0;
  itsPupilYpos() = 0.0;
  itsPupilSize() = 0.6;
  itsPupilDilation() = 0.5;

  itsEyebrowXpos() = 0.0;
  itsEyebrowYpos() = 0.5;
  itsEyebrowCurvature() = 0.8;
  itsEyebrowAngle() = -5;
  itsEyebrowThickness() = 2.0;

  itsNoseXpos() = 0.0;
  itsNoseYpos() = -0.825;
  itsNoseLength() = 0.75;
  itsNoseWidth() = 1.5;

  itsMouthXpos() = 0.0;
  itsMouthYpos() = -2.0;
  itsMouthWidth() = 2.5;
  itsMouthCurvature() = 0.6;

  Invariant(check());
}

// read the object's state from an input stream. The input stream must
// already be open and connected to an appropriate file.
MorphyFace::MorphyFace(istream& is, IOFlag flag) {
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

  if (flag & BASES) { GrObj::serialize(os, flag); }
}

void MorphyFace::deserialize(istream &is, IOFlag flag) {
DOTRACE("MorphyFace::deserialize");
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
	 // MorphyFace $category $eyeheight $eyedistance $noselength $mouthheight
	 vector<IO *> ioList;
	 makeIoList(ioList);
	 for (vector<IO *>::iterator ii = ioList.begin(); ii != ioList.end(); ii++) {
		(*ii)->deserialize(is, flag);
	 }
  }
  else if (version == 1) {
	 // Format is:
	 // MorphyFace { $category $eyeheight $eyedistance $noselength $mouthheight }
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

int MorphyFace::charCount() const {
DOTRACE("MorphyFace::charCount");
  return (ioTag.length() + 1
			 + 3 // version
			 + 2 // brace
			 + 128 // params
			 + 2 // brace
			 + 1);//fudge factor
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
		glTranslatef(abs(itsEyeDistance())/2.0, itsEyeYpos(), 0.0);

		// Draw eye outline
		for (int top_bottom = -1; top_bottom < 2; top_bottom += 2) {
		  glPushMatrix();
		    glScalef(itsEyeHeight()*itsEyeAspectRatio(),
						 itsEyeHeight()*top_bottom,
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
		  glTranslatef(itsEyebrowXpos(), itsEyebrowYpos(), 0.0);
		  glRotatef(itsEyebrowAngle(), 0.0, 0.0, 1.0);
		  glScalef(itsEyeHeight()*itsEyeAspectRatio(),
					  itsEyeHeight()*itsEyebrowCurvature(),
					  1.0);
		  glPushAttrib(GL_LINE_BIT);
		  {
			 GLdouble line_width;
			 glGetDoublev(GL_LINE_WIDTH, &line_width);
			 glLineWidth(itsEyebrowThickness()*line_width);
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
		  glTranslatef(left_right*itsPupilXpos(), itsPupilYpos(), 0.0);
		  glScalef(itsPupilSize()*itsEyeHeight(),
					  itsPupilSize()*itsEyeHeight(),
					  1.0);
		  static const int num_slices = 20;
		  static const int num_loops = 1;
		  gluDisk(qobj, 0.5*itsPupilDilation(), 0.5, num_slices, num_loops);
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
    -itsFaceWidth(), 0.0,        0,        // first 4 control points 
	 -itsTopWidth(),  itsTopHeight()*4.0/3.0, 0,
	 itsTopWidth(),   itsTopHeight()*4.0/3.0, 0,
	 itsFaceWidth(),  0.0,        0,
	 itsFaceWidth(), 0.0, 0, // second 4 control points 
	 itsBottomWidth(), itsBottomHeight()*4.0/3.0, 0,
	 -itsBottomWidth(), itsBottomHeight()*4.0/3.0, 0,
	 -itsFaceWidth(), 0.0, 0};
  
  glEnable(GL_MAP1_VERTEX_3);
  for (int i = 0; i < nctrlsets; ++i) {
	 glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpnts[i*12]);
	 // Evaluate the 1-d Bezier curve
	 glMapGrid1d(num_subdivisions, 0.0, 1.0);
	 glEvalMesh1(GL_LINE, 0, num_subdivisions);
  }
  
  //
  // Draw nose.
  //

  glPushMatrix();
    glTranslatef(itsNoseXpos(), itsNoseYpos(), 0.0);
	 glScalef(abs(itsNoseWidth())/2.0, abs(itsNoseLength()), 1.0);
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
    glTranslatef(itsMouthXpos(), itsMouthYpos(), 0.0);
	 glScalef(itsMouthWidth(), itsMouthCurvature(), 1.0);
	 glEnable(GL_MAP1_VERTEX_3);
	 glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, mouth_ctrlpnts);
	 // Evaluate the 1-d Bezier curve
	 glMapGrid1d(num_subdivisions, 0.0, 1.0);
	 glEvalMesh1(GL_LINE, 0, num_subdivisions);
  glPopMatrix();

  //
  // Draw hair.
  //

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
    glVertexPointer(2, GL_DOUBLE, 0, hair_vertices);
	 
	 glPushAttrib(GL_POLYGON_BIT);
	   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glPushMatrix();
		  glScalef(itsFaceWidth(), itsTopHeight(), 1.0);

		  glBegin(GL_TRIANGLE_FAN);
	       glArrayElement(0);
			 glArrayElement(1);
			 glArrayElement(2);
			 glArrayElement(3);
			 glArrayElement(4);
			 glArrayElement(5);
		  glEnd();
		
		  glBegin(GL_TRIANGLE_STRIP);
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
		  glEnd();

		  GLint foreground, background;
		  glGetIntegerv(GL_CURRENT_INDEX, &foreground);
		  glGetIntegerv(GL_INDEX_CLEAR_VALUE, &background);
		  
		  glIndexi(background);
		  glBegin(GL_LINE_STRIP);
		    glArrayElement(14);
		    glArrayElement(15);
		    glArrayElement(16);
		    glArrayElement(17);
		  glEnd();

		  glIndexi(foreground);
		  glBegin(GL_LINE_STRIP);
		    glArrayElement(18);
		    glArrayElement(19);
		    glArrayElement(20);
		    glArrayElement(21);
		  glEnd();


		glPopMatrix();
		
	 glPopAttrib();

  glDisableClientState(GL_VERTEX_ARRAY);

  // Undo pushes
  if (have_antialiasing) {
	 glPopAttrib();
  }
}

///////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////

bool MorphyFace::check() const {
DOTRACE("MorphyFace::check");
  return (itsEyeDistance() >= 0.0 && itsNoseLength() >= 0.0);
}

void MorphyFace::makeIoList(vector<IO *>& vec) {
DOTRACE("MorphyFace::makeIoList");
  vec.clear();

  vec.push_back(&itsCategory);
  vec.push_back(&itsEyeYpos);
  vec.push_back(&itsEyeDistance);
  vec.push_back(&itsNoseLength);
}

void MorphyFace::makeIoList(vector<const IO *>& vec) const {
DOTRACE("MorphyFace::makeIoList const");
  vec.clear();

  vec.push_back(&itsCategory);
  vec.push_back(&itsEyeYpos);
  vec.push_back(&itsEyeDistance);
  vec.push_back(&itsNoseLength);
}

static const char vcid_morphyface_cc[] = "$Header$";
#endif // !MORPHYFACE_CC_DEFINED
