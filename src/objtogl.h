///////////////////////////////////////////////////////////////////////
//
// objtogl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Nov-98
// written: Wed Jun  6 20:23:41 2001
// $Id$
//
// This package provides functionality that allows a Togl widget to
// work with a Glist, controlling its display, reshaping, etc.
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJTOGL_H_DEFINED
#define OBJTOGL_H_DEFINED

class Toglet;

namespace ObjTogl {
  void setCurrentTogl(Toglet* togl);
}

static const char vcid_objtogl_h[] = "$Header$";
#endif // !OBJTOGL_H_DEFINED
