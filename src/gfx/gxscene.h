///////////////////////////////////////////////////////////////////////
//
// gxscene.h
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sat Nov 23 17:42:38 2002
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

#ifndef GXSCENE_H_DEFINED
#define GXSCENE_H_DEFINED

#include "gfx/canvas.h"
#include "gfx/gxcamera.h"
#include "gfx/gxemptynode.h"
#include "gfx/gxnode.h"

#include "nub/ref.h"
#include "nub/timer.h"
#include "nub/volatileobject.h"

#include "util/sharedptr.h"

namespace Nub
{
  class Scheduler;
}

/// Manages a graphics "scene" for a window object.
class GxScene : public Nub::VolatileObject
{
public:
  /// Construct with a canvas in which to draw.
  GxScene(Nub::SoftRef<Gfx::Canvas> canvas);

  /// Virtual no-throw destructor.
  virtual ~GxScene() throw();

  /// Undraw the scene from the canvas.
  void undraw();

  /// "Bare-bones rendering"
  /** Only render the current object; the caller is expected to take
      care of clearing the color buffer first and flushing the
      graphics stream afterwards. */
  void render();

  /// "Full-featured rendering"
  /** First clears the color buffer, then renders the current object,
      then flushes the graphics stream and swaps buffers if
      necessary. */
  void fullRender();

  /// "Bare-bones clearscreen"
  /** Clears the color buffer and set the current object to empty, but
      don't flush the graphics stream. */
  void clearscreen();

  /// "Full-featured clearscreen"
  /** Clears the color buffer, set the current object to empty, and
      flush the graphics stream. */
  void fullClearscreen();

  /// Change the global visibility
  /** Which determines whether anything will be displayed by a
      "redraw" command, or by remap events sent to the screen
      window. */
  void setVisibility(bool val);

  /// Get the camera used to view the scene.
  const Nub::Ref<GxCamera>& getCamera() const
  { return itsCamera; }

  /// Set the camera to be used to view the scene.
  void setCamera(const Nub::Ref<GxCamera>& cam);

  /// Get the drawable object currently being viewed.
  void setDrawable(const Nub::Ref<GxNode>& node);

  /// To be called when the associated window is resized.
  void reshape(int width, int height);

  /// Whether to retain current contents when something new is drawn.
  void setHold(bool val) { isItHolding = val; }

  /// Whether the scene should be refreshed when something changes.
  void allowRefresh(bool allow)
  { isItRefreshing = allow; flushChanges(); }

  /// Redraw the scene at a given frame rate.
  void animate(unsigned int framesPerSecond);

private:
  void flushChanges();
  void onNodeChange();

  GxScene(const GxScene&);
  GxScene& operator=(const GxScene&);

  Nub::SoftRef<Gfx::Canvas> itsCanvas;
  Nub::Ref<GxNode> itsDrawNode;
  Nub::Ref<GxNode> itsUndrawNode;
  bool isItVisible;

  Nub::Ref<GxCamera> itsCamera;
  int itsWidth;
  int itsHeight;
  bool isItHolding;
  bool isItRefreshing;
  bool isItRefreshed;

  const rutz::shared_ptr<Nub::Scheduler> itsScheduler;
  Nub::Timer itsTimer;

  Nub::Ref<Nub::Slot0> slotNodeChanged;
};

static const char vcid_gxscene_h[] = "$Header$";
#endif // !GXSCENE_H_DEFINED
