///////////////////////////////////////////////////////////////////////
//
// gtext.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  1 11:54:47 1999
// written: Mon Sep 20 16:14:24 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXT_H_DEFINED
#define GTEXT_H_DEFINED

#ifndef GROBJ_H_DEFINED
#include "grobj.h"
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

#ifndef STRING_DEFINED
#include <string>
#endif

class GtextError : public ErrorWithMsg {
public:
  GtextError(const string& str = "") : ErrorWithMsg(str) {}
};

class Gtext : public GrObj {
public:
  Gtext(const char* text=0);
  Gtext(istream& is, IOFlag flag);
  virtual ~Gtext();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void setCategory(int) {}
  virtual int getCategory() const { return -1; }

  void setText(const char* text);
  const char* getText() const;

protected:
  virtual void grRender() const;

private:
  string itsText;
  mutable unsigned int itsListBase;
};

static const char vcid_gtext_h[] = "$Header$";
#endif // !GTEXT_H_DEFINED
