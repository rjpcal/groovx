///////////////////////////////////////////////////////////////////////
// gfxattribs.h
// Rob Peters
// created: Tue Mar  9 18:28:13 1999
// written: Tue Jun 22 12:06:28 1999
// $Id$
//
// This class containing only static members serves as a
// platform-independent and low-level means for different parts of a
// program to communicate about various properties of the local
// graphics environment,
///////////////////////////////////////////////////////////////////////

#ifndef GFXATTRIBS_H_DEFINED
#define GFXATTRIBS_H_DEFINED

class GfxAttribs {
public:
  enum GfxFlag { NO_FLAGS               = 0,
                 RGBA_FLAG              = 1 << 0,
                 PRIVATE_CMAP_FLAG      = 1 << 1,
					  DOUBLE_BUFFER_FLAG     = 1 << 2, };

  static bool usingRgba() { return isTrue(RGBA_FLAG); }
  static bool usingDoubleBuffer() { return isTrue(DOUBLE_BUFFER_FLAG); }

  static void setFlagsIf(GfxFlag flags, bool cond) {
    if (cond) theFlags = GfxFlag(theFlags | flags);
    else theFlags = GfxFlag(theFlags & ~flags);
  }

  static bool isTrue(GfxFlag flag) {
    return ((theFlags & flag) != 0);
  }

  static double getScreenPpi() { return theScreenPpi; }
  static void setScreenPpi(double f) { theScreenPpi = f; }

private:
  GfxAttribs ();                // this class not to be instantiated
  
  static GfxFlag theFlags;
  static double theScreenPpi;
};

static const char vcid_gfxattribs_h[] = "$Header$";
#endif // !GFXATTRIBS_H_DEFINED
