///////////////////////////////////////////////////////////////////////
//
// element.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  4 15:35:37 2002
// written: Wed Dec  4 17:01:33 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ELEMENT_H_DEFINED
#define ELEMENT_H_DEFINED

#include "io/io.h"

namespace Util
{
  class ErrorHandler;
  template <class T> class SoftRef;
};

class Block;
class Response;
class Toglet;

class fstring;

//  ###################################################################
//  ===================================================================

/// Element is an abstract base class for all experiment components.
/** This conceptually includes individual trials, blocks, experiment
    sessions, series of multiple experiments, etc. */

class Element : public IO::IoObject
{
public:
  virtual ~Element();

//   virtual int numLeafs() const = 0;

//   virtual bool isComplete() const = 0;

  virtual int trialType() const = 0;

  virtual int lastResponse() const = 0;

  virtual fstring status() const = 0;


  virtual void run(const Util::SoftRef<Toglet>& widget,
                   Util::ErrorHandler& errhdlr, Block& block) = 0;

  virtual void halt() = 0;

  virtual void undoPrevious() = 0;
};

static const char vcid_element_h[] = "$Header$";
#endif // !ELEMENT_H_DEFINED
