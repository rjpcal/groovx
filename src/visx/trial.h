///////////////////////////////////////////////////////////////////////
//
// trial.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Mar  1 08:00:00 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_H_DEFINED
#define TRIAL_H_DEFINED

#include "io/fields.h"

#include "util/tracer.h"

#include "visx/element.h"

namespace rutz
{
  template <class T> class fwd_iter;
}

namespace Nub
{
  template <class T> class Ref;
}

class GxNode;
class Response;
class ResponseHandler;
class TimingHdlr;

/// Trial collaborates with ResponseHandler+TimingHdlr.
class Trial : public Element, public FieldContainer
{
public:

  /** This tracer dynamically controls the tracing of Trial member
      functions. */
  static rutz::tracer tracer;

  //////////////
  // creators //
  //////////////

protected:
  Trial();
public:
  static Trial* make();

  virtual ~Trial () throw();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  static const FieldMap& classFields();

  ////////////////////////////
  // accessors+manipulators //
  ////////////////////////////

  int getCorrectResponse() const;
  void setCorrectResponse(int response);

  Nub::Ref<ResponseHandler> getResponseHandler() const;
  void setResponseHandler(Nub::Ref<ResponseHandler> rh);

  Nub::Ref<TimingHdlr> getTimingHdlr() const;
  void setTimingHdlr(Nub::Ref<TimingHdlr> th);

  void setType(int t);

  rutz::fwd_iter<Response> responses() const;

  unsigned int numResponses() const;

  void clearResponses();

  double avgResponse() const;
  double avgRespTime() const;

  void addNode(Nub::Ref<GxNode> item);

  void trNextNode();

  /// Returns an iterator to all the nodes contained in the Trial.
  rutz::fwd_iter<Nub::Ref<GxNode> > nodes() const;

  unsigned int getCurrentNode() const;
  void setCurrentNode(unsigned int nodeNumber);

  void clearObjs();

  /// Returns a standard set of descriptive information.
  /** This is here mostly for legacy reasons; this function used to be
      the implementation of vxInfo(). */
  rutz::fstring stdInfo() const;

  /// Set the contents of what will be returned from vxInfo().
  /** If setInfo() is called with an empty string, then vxInfo() will
      by default return the result of stdInfo() instead. */
  void setInfo(rutz::fstring info);

  //
  // Element interface
  //

  virtual const Nub::SoftRef<Toglet>& getWidget() const;

  /// returns some user-defined info about relationship between objects in trial
  virtual int trialType() const;

  virtual int lastResponse() const;

  /// Overridden from Element.
  virtual rutz::fstring vxInfo() const;

  virtual void vxRun(Element& parent);

  virtual void vxHalt() const;

  virtual void vxReturn(ChildStatus s);

  virtual void vxUndo();

  virtual void vxReset();

  /////////////
  // actions //
  /////////////

  double trElapsedMsec();

  void trProcessResponse(Response& response);
  void trDraw();
  void trRender();
  void trUndraw();
  void trSwapBuffers();
  void trRenderBack();
  void trRenderFront();
  void trClearBuffer();
  void trFinishDrawing();
  void trAllowResponses();
  void trDenyResponses();
  void trAbortTrial();
  void trEndTrial();

private:
  Trial(const Trial&);
  Trial& operator=(const Trial&);

  class Impl;
  Impl* const rep;
};

static const char vcid_trial_h[] = "$Id$ $URL$";
#endif // !TRIAL_H_DEFINED
