///////////////////////////////////////////////////////////////////////
//
// xitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep  7 14:39:09 1999
// written: Wed Dec  1 20:15:25 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_H_DEFINED
#define XBITMAP_H_DEFINED

#ifndef BITMAP_H_DEFINED
#include "bitmap.h"
#endif

class ToglConfig;

class XBmapRenderer;

class XBitmap : public Bitmap {
public:
  // Creators
  XBitmap();
  XBitmap(const char* filename);
  XBitmap(istream& is, IOFlag flag);

  private: void init();
  
public:
  static void initClass(const ToglConfig* config);

  virtual ~XBitmap();

  virtual void serialize(ostream& os, IOFlag flag) const;
  virtual void deserialize(istream& is, IOFlag flag);

  virtual int charCount() const;

private:
  XBmapRenderer* itsRenderer;
};

static const char vcid_xbitmap_h[] = "$Header$";
#endif // !XBITMAP_H_DEFINED
