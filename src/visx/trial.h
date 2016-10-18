/** @file visx/trial.h run a series of timed events and collect user
    responses in a psychophysics experiment */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Mar  1 08:00:00 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_TRIAL_H_UTC20050626084016_DEFINED
#define GROOVX_VISX_TRIAL_H_UTC20050626084016_DEFINED

#include "io/fields.h"

#include "rutz/tracer.h"

#include "visx/element.h"

namespace rutz
{
  template <class T> class fwd_iter;
}

namespace nub
{
  template <class T> class ref;
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

  virtual ~Trial () noexcept;

  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  static const FieldMap& classFields();

  ////////////////////////////
  // accessors+manipulators //
  ////////////////////////////

  int getCorrectResponse() const;
  void setCorrectResponse(int response);

  nub::ref<ResponseHandler> getResponseHandler() const;
  void setResponseHandler(nub::ref<ResponseHandler> rh);

  nub::ref<TimingHdlr> getTimingHdlr() const;
  void setTimingHdlr(nub::ref<TimingHdlr> th);

  void setType(int t);

  rutz::fwd_iter<Response> responses() const;

  unsigned int numResponses() const;

  void clearResponses();

  double avgResponse() const;
  double avgRespTime() const;

  void addNode(nub::ref<GxNode> item);

  void trNextNode();

  /// Returns an iterator to all the nodes contained in the Trial.
  rutz::fwd_iter<nub::ref<GxNode> > nodes() const;

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

  virtual const nub::soft_ref<Toglet>& getWidget() const override;

  /// returns some user-defined info about relationship between objects in trial
  virtual int trialType() const override;

  virtual int lastResponse() const override;

  /// Overridden from Element.
  virtual rutz::fstring vxInfo() const override;

  virtual void vxRun(Element& parent) override;

  virtual void vxHalt() const override;

  virtual void vxReturn(ChildStatus s) override;

  virtual void vxUndo() override;

  virtual void vxReset() override;

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

#endif // !GROOVX_VISX_TRIAL_H_UTC20050626084016_DEFINED
