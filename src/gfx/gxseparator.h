/** @file gfx/gxseparator.h GxNode subclass that holds a variable
    number of child nodes */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Nov  2 11:20:15 2000
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

#ifndef GROOVX_GFX_GXSEPARATOR_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXSEPARATOR_H_UTC20050626084023_DEFINED

#include "gfx/gxnode.h"

namespace nub
{
  template <class T> class ref;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * GxSeparator is a subclass of GxNode that holds a list of children,
 * and can be used to implement arbitrary tree structured hierarchies
 * of GxNode's. GxSeparator saves the OpenGL state before drawing or
 * undrawing its children, and restores the state when it is done
 * handling its children.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GxSeparator : public GxNode
{
protected:
  /// Default constructor.
  GxSeparator();

public:
  /// Factory function.
  static GxSeparator* make();

  /// Virtual destructor.
  virtual ~GxSeparator() noexcept;

  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  /// Index type to refer to child nodes.
  typedef unsigned int ChildId;

  /// Add the given child node, and return its index.
  ChildId addChild(nub::ref<GxNode> item);
  /// Insert the given child at the given index.
  void insertChild(nub::ref<GxNode> item, ChildId at_index);
  /// Remove the child at the given index.
  void removeChildAt(ChildId index);
  /// Remove the child matching the given node.
  void removeChild(nub::ref<GxNode> item);

  /// Get the number of child nodes.
  unsigned int numChildren() const;
  /// Get the child node at the given index.
  nub::ref<GxNode> getChild(ChildId index) const;

  /// Get an iterator of all the child nodes.
  rutz::fwd_iter<nub::ref<GxNode> > children() const;

  /** Returns an iterator to all the children recursively contained in
      this object. */
  virtual rutz::fwd_iter<const nub::ref<GxNode> > deepChildren() override;

  virtual bool contains(GxNode* other) const override;

  /// Apply all of the contained objects' transformations to the cube.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const override;

  /// Query whether debug-mode drawing is turned on.
  bool getDebugMode() const;

  /// Turn debug-mode drawing on or off.
  void setDebugMode(bool b);

  /// Draw all of the contained objects on \a canvas.
  virtual void draw(Gfx::Canvas& canvas) const override;

private:
  GxSeparator(const GxSeparator&);
  GxSeparator& operator=(const GxSeparator&);

  class Impl;
  Impl* const rep;
};

#endif // !GROOVX_GFX_GXSEPARATOR_H_UTC20050626084023_DEFINED
