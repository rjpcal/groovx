///////////////////////////////////////////////////////////////////////
//
// element.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  4 15:35:37 2002
// written: Wed Dec  4 18:21:18 2002
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

  /// Return an \c ErrorHandler that can deal with error messages.
  virtual Util::ErrorHandler& getErrorHandler() const = 0;

  /// Return the Widget in which the element is running.
  virtual const Util::SoftRef<Toglet>& getWidget() const = 0;

  virtual int trialType() const = 0;

  virtual int lastResponse() const = 0;

  virtual fstring status() const = 0;


  virtual void vxRun(Element& parent) = 0;

  virtual void vxHalt() const = 0;

  virtual void vxAbort() = 0;

  virtual void vxEndTrial() = 0;

  virtual void vxNext() = 0;

  virtual void vxProcessResponse(Response& response) = 0;

  virtual void vxUndo() = 0;
};

static const char vcid_element_h[] = "$Header$";
#endif // !ELEMENT_H_DEFINED