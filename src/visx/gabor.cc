///////////////////////////////////////////////////////////////////////
//
// gabor.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct  6 10:45:58 1999
// written: Sat Mar  4 00:27:15 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GABOR_CC_DEFINED
#define GABOR_CC_DEFINED

#include "gabor.h"

#include "randutils.h"
#include "reader.h"
#include "rect.h"
#include "writer.h"

#include <GL/gl.h>
#include <cmath>
#include <string>

#include "trace.h"
#include "debug.h"


namespace {
  const string ioTag = "Gabor";
}

const Gabor::ColorMode Gabor::GRAYSCALE;
const Gabor::ColorMode Gabor::BW_DITHER_POINT;
const Gabor::ColorMode Gabor::BW_DITHER_RECT;

Gabor::Gabor() :
  colorMode(2),
  contrast(1.0),
  spatialFreq(3.5),
  phase(0.0),
  sigma(0.15),
  aspectRatio(1.0), 
  orientation(0.0),
  resolution(60),
  pointSize(1)
{
DOTRACE("Gabor::Gabor");
  GrObj::setUnRenderMode(GrObj::GROBJ_CLEAR_BOUNDING_BOX);
  GrObj::setScalingMode(GrObj::MAINTAIN_ASPECT_SCALING);
}

Gabor::~Gabor () {
DOTRACE("Gabor::~Gabor");

}

void Gabor::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Gabor::serialize");
  if (flag & TYPENAME) { os << ioTag << IO::SEP; }
  if (os.fail()) throw OutputError(ioTag);
  if (flag & BASES) { GrObj::serialize(os, flag | TYPENAME); }
}

void Gabor::deserialize(istream &is, IOFlag flag) {
DOTRACE("Gabor::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }
  if (is.fail()) throw InputError(ioTag);
  if (flag & BASES) { GrObj::deserialize(is, flag | TYPENAME); }
}

int Gabor::charCount() const {
DOTRACE("Gabor::charCount");
  return ioTag.length() + 1 + GrObj::charCount();
}

void Gabor::readFrom(Reader* reader) {
DOTRACE("Gabor::readFrom");
  const vector<PInfo>& infos = getPropertyInfos();
  for (size_t i = 0; i < infos.size(); ++i) {
	 reader->readValueObj(infos[i].name_cstr(),
								 const_cast<Value&>(get(infos[i].property())));
  }

  GrObj::readFrom(reader);
}

void Gabor::writeTo(Writer* writer) const {
DOTRACE("Gabor::writeTo");
  const vector<PInfo>& infos = getPropertyInfos();
  for (size_t i = 0; i < infos.size(); ++i) {
	 writer->writeValueObj(infos[i].name_cstr(), get(infos[i].property()));
  }

  GrObj::writeTo(writer);
}

const vector<Gabor::PInfo>& Gabor::getPropertyInfos() {
DOTRACE("Fish::getPropertyInfos");

  static vector<PInfo> p;

  typedef Gabor G;

  if (p.size() == 0) {
	 p.push_back(PInfo("colorMode", &G::colorMode, 1, 3, 1, true));
	 p.push_back(PInfo("contrast", &G::contrast, 0.0, 1.0, 0.05));
	 p.push_back(PInfo("spatialFreq", &G::spatialFreq, 0.5, 10.0, 0.5));
	 p.push_back(PInfo("phase", &G::phase, -180, 179, 1));
	 p.push_back(PInfo("sigma", &G::sigma, 0.025, 0.5, 0.025));
	 p.push_back(PInfo("aspectRatio", &G::aspectRatio, 0.1, 10.0, 0.1));
	 p.push_back(PInfo("orientation", &G::orientation, -180, 179, 1));
	 p.push_back(PInfo("resolution", &G::resolution, 5, 500, 5));
	 p.push_back(PInfo("pointSize", &G::pointSize, 1, 25, 1));
  }
  return p;
}

void Gabor::grGetBoundingBox(Rect<double>& bbox,
									  int& border_pixels) const {
DOTRACE("Gabor::grGetBoundingBox");
  bbox.left() = -0.5;
  bbox.right() = 0.5;
  bbox.bottom() = -0.5;
  bbox.top() = 0.5;

  border_pixels = 2;
}

bool Gabor::grHasBoundingBox() const {
DOTRACE("Gabor::grHasBoundingBox");
  return true;
}

void Gabor::grRender(Canvas&) const {
DOTRACE("Gabor::grRender");
  double xsigma = sigma()*aspectRatio();
  double ysigma = sigma();
  double xmean = 0.0;
  double ymean = 0.0;

  static const double PI = acos(-1.0);

  //                      1            ( -(x-mean)^2 / (2*sigma)^2 )
  // gaussian(x) = ---------------- * e
  //               sigma*sqrt(2*pi)

  // For our purposes, we ignore the scaling factor out front, so that
  // the maximum value the function takes on is 1 (the scaling factor
  // is otherwise there to keep the total area under the curve to 1,
  // but this affects the maximum height).

  double res_step = 1.0/resolution();

  glPushAttrib(GL_POLYGON_BIT | GL_POINT_BIT);
  {
	 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	 glPointSize(pointSize());
	 
	 for (int x_step = 0; x_step < resolution(); ++x_step) {
		
		double unrotated_x = x_step*res_step - 0.5;

		double x = unrotated_x;

		for (int y_step = 0; y_step < resolution(); ++y_step) {
		  
		  double unrotated_y = y_step*res_step - 0.5;

		  double y = unrotated_y;

		  if ( orientation() != 0 ) {
			 if ( orientation() == 90 ) {
				x = -unrotated_y;
				y = unrotated_x;
			 } else if ( orientation() == 180 ) {
				x = -unrotated_x;
				y = -unrotated_y;
			 } else if ( orientation() == 270 ) {
				x = unrotated_y;
				y= -unrotated_x;
			 }
			 else {
				double r = sqrt(unrotated_x*unrotated_x + unrotated_y*unrotated_y);
				
				double atan_y_x = atan2(unrotated_x, unrotated_y);
				
				// Note: the (orientation() + 90) is required to translate
				// from North == 0 to East == 0
				double new_theta = atan_y_x + (orientation()+90)*PI/180.0;

				while (new_theta > PI) {
				  new_theta -= 2*PI;
				}
				while (new_theta < -PI) {
				  new_theta += 2*PI;
				}
				
				double unscaled_new_x = 1.0;
				double unscaled_new_y = abs(tan(new_theta));
				
				double scale_factor = 
				  sqrt(unscaled_new_x*unscaled_new_x + unscaled_new_y*unscaled_new_y);
				
				x = r * unscaled_new_x / scale_factor;
				y = r * unscaled_new_y / scale_factor;

				if (new_theta < -PI/2.0) {
				  x = -x;
				  y = -y;
				} else if (new_theta < 0.0) {
				  y = -y;
				} else if (new_theta < PI/2.0) {
				} else {
				  x = -x;
				}
			 }
		  }

		  double gauss_x = exp( -1.0*(x-xmean)*(x-xmean) / (4.0*xsigma*xsigma) );
		  
		  double sin_x = sin(2*PI*spatialFreq()*x + phase()*PI/180.0);
		
		  double gauss_y = exp( -1.0*(y-ymean)*(y-ymean) / (4.0*ysigma*ysigma) );
		  
		  double gabor = 0.5*sin_x*gauss_x*gauss_y*contrast() + 0.5;
		  
		  if ( colorMode() == GRAYSCALE ) {
			 glColor4d(gabor, gabor, gabor, 1.0);
			 glBegin(GL_POINTS);
			 glVertex2d(unrotated_x, unrotated_y);
			 glEnd();			 
		  }
		  else if ( colorMode() == BW_DITHER_POINT ) {
			 if ( randDoubleRange(0.0, 1.0) < gabor ) {
				glBegin(GL_POINTS);
				glVertex2d(unrotated_x, unrotated_y);
				glEnd();
			 }
		  }
		  else if ( colorMode() == BW_DITHER_RECT ) {
			 if ( randDoubleRange(0.0, 1.0) < gabor ) {
				glRectd(unrotated_x, unrotated_y,
						  unrotated_x+res_step, unrotated_y+res_step);
			 }
		  }
		}
	 } 
  }
  glPopAttrib();
}

static const char vcid_gabor_cc[] = "$Header$";
#endif // !GABOR_CC_DEFINED
