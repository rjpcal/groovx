///////////////////////////////////////////////////////////////////////
//
// gfxattribs.h
// Rob Peters
// created: Tue Mar  9 18:28:13 1999
// written: Mon Dec  6 16:08:00 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GFXATTRIBS_H_DEFINED
#define GFXATTRIBS_H_DEFINED

///////////////////////////////////////////////////////////////////////
/**
 *
 * The GfxAttribs class serves as a global repository for information
 * about the graphics environment.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GfxAttribs {
public:
  ///
  enum GfxFlag { NO_FLAGS               = 0,
                 RGBA_FLAG              = 1 << 0,
                 PRIVATE_CMAP_FLAG      = 1 << 1,
					  DOUBLE_BUFFER_FLAG     = 1 << 2 };

  ///
  static bool usingRgba() { return isTrue(RGBA_FLAG); }
  ///
  static bool usingDoubleBuffer() { return isTrue(DOUBLE_BUFFER_FLAG); }

  ///
  static void setFlagsIf(GfxFlag flags, bool cond) {
    if (cond) theFlags = GfxFlag(theFlags | flags);
    else theFlags = GfxFlag(theFlags & ~flags);
  }

  ///
  static bool isTrue(GfxFlag flag) {
    return ((theFlags & flag) != 0);
  }

private:
  GfxAttribs ();                // this class not to be instantiated
  
  static GfxFlag theFlags;
};

static const char vcid_gfxattribs_h[] = "$Header$";
#endif // !GFXATTRIBS_H_DEFINED
