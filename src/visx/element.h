///////////////////////////////////////////////////////////////////////
//
// element.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  4 15:35:37 2002
// written: Wed Dec  4 15:50:05 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ELEMENT_H_DEFINED
#define ELEMENT_H_DEFINED

#include "util/object.h"

class fstring;

//  ###################################################################
//  ===================================================================

/// Element is an abstract base class for all experiment components.
/** This conceptually includes individual trials, blocks, experiment
    sessions, series of multiple experiments, etc. */

class Element
{
public:
  virtual ~Element();

//   virtual int numLeafs() const = 0;

//   virtual bool isComplete() const = 0;

  virtual fstring status() const = 0;
};

static const char vcid_element_h[] = "$Header$";
#endif // !ELEMENT_H_DEFINED
