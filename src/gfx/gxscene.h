///////////////////////////////////////////////////////////////////////
//
// gxscene.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Nov 23 17:42:38 2002
// written: Mon Jan 13 11:01:38 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSCENE_H_DEFINED
#define GXSCENE_H_DEFINED

#include "gfx/canvas.h"
#include "gfx/gxcamera.h"
#include "gfx/gxemptynode.h"
#include "gfx/gxnode.h"

#include "util/ref.h"
#include "util/volatileobject.h"

#include "tcl/tcltimer.h"

class GxScene : public Util::VolatileObject
{
public:
  GxScene(Util::SoftRef<Gfx::Canvas> canvas);

  void undraw();

  /// "Bare-bones rendering"
  /** Only render the current object; the caller is expected to take care
      of clearing the color buffer first and flushing the graphics stream
      afterwards. */
  void render();

  /// "Full-featured rendering"
  /** First clears the color buffer, then renders the current object, then
      flushes the graphics stream and swaps buffers if necessary. */
  void fullRender();

  /// "Bare-bones clearscreen"
  /** Clears the color buffer and set the current object to empty, but
      don't flush the graphics stream. */
  void clearscreen();

  /// "Full-featured clearscreen"
  /** Clears the color buffer, set the current object to empty, and flush
      the graphics stream. */
  void fullClearscreen();

  /// Change the global visibility
  /** Which determines whether anything will be displayed by a "redraw"
      command, or by remap events sent to the screen window. */
  void setVisibility(bool val);

  const Util::Ref<GxCamera>& getCamera() const { return itsCamera; }

  void setCamera(const Ref<GxCamera>& cam);

  void setDrawable(const Ref<GxNode>& node);

  void reshape(int width, int height);

  void setHold(bool val) { isItHolding = val; }

  void allowRefresh(bool allow) { isItRefreshing = allow; flushChanges(); }

  void animate(unsigned int framesPerSecond);

private:
  void flushChanges();
  void onNodeChange();

  GxScene(const GxScene&);
  GxScene& operator=(const GxScene&);

  Util::SoftRef<Gfx::Canvas> itsCanvas;
  Util::Ref<GxNode> itsDrawNode;
  Util::Ref<GxNode> itsUndrawNode;
  bool isItVisible;

  Util::Ref<GxCamera> itsCamera;
  int itsWidth;
  int itsHeight;
  bool isItHolding;
  bool isItRefreshing;
  bool isItRefreshed;

  Tcl::Timer itsTimer;

  Util::Ref<Util::Slot0> slotNodeChanged;
};

static const char vcid_gxscene_h[] = "$Header$";
#endif // !GXSCENE_H_DEFINED
