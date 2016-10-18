/** @file gfx/gxseparator.cc GxNode subclass that holds a variable
    number of child nodes */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Nov  2 11:24:04 2000
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

#ifndef GROOVX_GFX_GXSEPARATOR_CC_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXSEPARATOR_CC_UTC20050626084024_DEFINED

#include "gxseparator.h"

#include "geom/box.h"
#include "geom/rect.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "nub/ref.h"
#include "nub/volatileobject.h"

#include "rutz/error.h"
#include "rutz/iter.h"
#include "rutz/sfmt.h"

#include <list>
#include <vector>

#include "rutz/trace.h"

using rutz::shared_ptr;

class GxSeparator::Impl : public nub::volatile_object
{
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(GxSeparator* p) :
    owner(p),
    children(),
    debugMode(false)
  {}

  virtual ~Impl() noexcept {}

  static Impl* make(GxSeparator* owner) { return new Impl(owner); }

  bool contains(GxNode* other) const
    {
      if (owner == other) return true;

      for(VecType::const_iterator
            itr = children.begin(),
            end = children.end();
          itr != end;
          ++itr)
        {
          if ((*itr)->contains(other)) return true;
        }

      return false;
    }

  void ensureNoCycle(GxNode* other) const
    {
      if (other->contains(owner))
        {
          throw rutz::error("couldn't add node without generating a cycle", SRC_POS);
        }
    }

  GxSeparator* owner;

  typedef std::vector<nub::ref<GxNode> > VecType;
  VecType children;

  bool debugMode;
};

GxSeparator* GxSeparator::make()
{
GVX_TRACE("GxSeparator::make");
  return new GxSeparator;
}

GxSeparator::GxSeparator() :
  rep(Impl::make(this))
{
GVX_TRACE("GxSeparator::GxSeparator");
}

GxSeparator::~GxSeparator() noexcept
{
GVX_TRACE("GxSeparator::~GxSeparator");
  rep->destroy();
}

void GxSeparator::read_from(io::reader& reader)
{
GVX_TRACE("GxSeparator::read_from");

  for(unsigned int i = 0; i < rep->children.size(); ++i)
    {
      rep->children[i]->sigNodeChanged
        .disconnect(this->sigNodeChanged.slot());
    }

  rep->children.clear();
  io::read_utils::read_object_seq<GxNode>(
          reader, "children", std::back_inserter(rep->children));

  for(unsigned int i = 0; i < rep->children.size(); ++i)
    {
      rep->children[i]->sigNodeChanged
        .connect(this->sigNodeChanged.slot());
    }

  this->sigNodeChanged.emit();
}

void GxSeparator::write_to(io::writer& writer) const
{
GVX_TRACE("GxSeparator::write_to");
  io::write_utils::write_object_seq(writer, "children",
                                    rep->children.begin(),
                                    rep->children.end());
}

GxSeparator::ChildId GxSeparator::addChild(nub::ref<GxNode> item)
{
GVX_TRACE("GxSeparator::addChild");

  rep->ensureNoCycle(item.get());

  rep->children.push_back(item);

  item->sigNodeChanged.connect(this->sigNodeChanged.slot());

  this->sigNodeChanged.emit();

  return (rep->children.size() - 1);
}

void GxSeparator::insertChild(nub::ref<GxNode> item, ChildId at_index)
{
GVX_TRACE("GxSeparator::insertChild");

  rep->ensureNoCycle(item.get());

  if (at_index > rep->children.size())
    at_index = rep->children.size();

  rep->children.insert(rep->children.begin()+at_index, item);

  item->sigNodeChanged.connect(this->sigNodeChanged.slot());

  this->sigNodeChanged.emit();
}

void GxSeparator::removeChildAt(ChildId index)
{
GVX_TRACE("GxSeparator::removeChildAt");
  if (index < rep->children.size())
    {
      rep->children[index]->sigNodeChanged
        .disconnect(this->sigNodeChanged.slot());
      rep->children.erase(rep->children.begin()+index);

      this->sigNodeChanged.emit();
    }
}

void GxSeparator::removeChild(nub::ref<GxNode> item)
{
GVX_TRACE("GxSeparator::removeChild");

  const nub::uid target = item.id();

  for(Impl::VecType::iterator
        itr = rep->children.begin(),
        end = rep->children.end();
      itr != end;
      ++itr)
    {
      if ( (*itr)->id() == target )
        {
          (*itr)->sigNodeChanged.disconnect(this->sigNodeChanged.slot());
          rep->children.erase(itr);
          this->sigNodeChanged.emit();
          break;
        }
    }
}

unsigned int GxSeparator::numChildren() const
{
GVX_TRACE("GxSeparator::numChildren");
  return rep->children.size();
}

nub::ref<GxNode> GxSeparator::getChild(ChildId index) const
{
GVX_TRACE("GxSeparator::getChild");
  if (index >= rep->children.size())
    {
      throw rutz::error(rutz::sfmt("GxSeparator has no child with "
                                   "index '%u'", index), SRC_POS);
    }

  return rep->children[index];
}

rutz::fwd_iter<nub::ref<GxNode> > GxSeparator::children() const
{
GVX_TRACE("GxSeparator::children");

  return rutz::fwd_iter<nub::ref<GxNode> >(rep->children.begin(),
                                           rep->children.end());
}

class GxSepIter : public rutz::fwd_iter_ifx<const nub::ref<GxNode> >
{
public:
  GxSepIter(GxSeparator* root) :
    itsNodes()
  {
    for (rutz::fwd_iter<nub::ref<GxNode> > itr(root->children());
         itr.is_valid();
         ++itr)
      {
        addDeepChildren(*itr);
      }
  }

  void addDeepChildren(nub::ref<GxNode> node)
  {
    for (rutz::fwd_iter<const nub::ref<GxNode> > itr(node->deepChildren());
         itr.is_valid();
         ++itr)
      {
        itsNodes.push_back(*itr);
      }
  }

  typedef const nub::ref<GxNode> ValType;

  virtual rutz::fwd_iter_ifx<ValType>* clone() const
  {
    return new GxSepIter(*this);
  }

  virtual bool     at_end() const { return itsNodes.empty(); }
  virtual ValType&   get()  const { return itsNodes.front(); }
  virtual void      next()        { if (!at_end()) itsNodes.pop_front(); }

private:
  // Want to use a list instead of a vector-type container here since
  // we need both push_back() and pop_front(). Could potentially use a
  // deque instead.
  mutable std::list<nub::ref<GxNode> > itsNodes;
};

rutz::fwd_iter<const nub::ref<GxNode> > GxSeparator::deepChildren()
{
GVX_TRACE("GxSeparator::deepChildren");

  return rutz::fwd_iter<const nub::ref<GxNode> >
    (shared_ptr<GxSepIter>(new GxSepIter(this)));
}

bool GxSeparator::contains(GxNode* other) const
{
GVX_TRACE("GxSeparator::contains");
  return rep->contains(other);
}

void GxSeparator::getBoundingCube(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxSeparator::getBoundingCube");

  if (!rep->children.empty())
    {
      bbox.push();

      for(Impl::VecType::iterator
            itr = rep->children.begin(),
            end = rep->children.end();
          itr != end;
          ++itr)
        {
          (*itr)->getBoundingCube(bbox);
        }

      bbox.pop();
    }
}

bool GxSeparator::getDebugMode() const
{
GVX_TRACE("GxSeparator::getDebugMode");

  return rep->debugMode;
}

void GxSeparator::setDebugMode(bool b)
{
GVX_TRACE("GxSeparator::setDebugMode");

  if (rep->debugMode != b)
    {
      rep->debugMode = b;
      this->sigNodeChanged.emit();
    }
}

void GxSeparator::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxSeparator::draw");

  if (rep->debugMode)
    {
      Gfx::Bbox bbox(canvas);
      getBoundingCube(bbox);
      canvas.drawBox(bbox.cube());
    }

  if (!rep->children.empty())
    {
      Gfx::MatrixSaver state(canvas);
      Gfx::AttribSaver attribs(canvas);

      for(Impl::VecType::iterator
            itr = rep->children.begin(),
            end = rep->children.end();
          itr != end;
          ++itr)
        {
          (*itr)->draw(canvas);
        }
    }
}

#endif // !GROOVX_GFX_GXSEPARATOR_CC_UTC20050626084024_DEFINED
