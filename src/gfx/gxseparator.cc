///////////////////////////////////////////////////////////////////////
//
// gxseparator.cc
//
// Copyright (c) 2000-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Nov  2 11:24:04 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSEPARATOR_CC_DEFINED
#define GXSEPARATOR_CC_DEFINED

#include "gfx/gxseparator.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/box.h"
#include "gx/rect.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "util/error.h"
#include "util/iter.h"
#include "util/ref.h"
#include "util/volatileobject.h"

#include <list>
#include <vector>

#include "util/trace.h"

class GxSeparator::Impl : public Util::VolatileObject
{
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(GxSeparator* p) :
    owner(p),
    children(),
    debugMode(false)
  {}

  virtual ~Impl() throw() {}

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
          throw Util::Error("couldn't add node without generating a cycle");
        }
    }

  GxSeparator* owner;

  typedef std::vector<Util::Ref<GxNode> > VecType;
  VecType children;

  bool debugMode;
};

GxSeparator* GxSeparator::make()
{
DOTRACE("GxSeparator::make");
  return new GxSeparator;
}

GxSeparator::GxSeparator() :
  rep(Impl::make(this))
{
DOTRACE("GxSeparator::GxSeparator");
}

GxSeparator::~GxSeparator() throw()
{
DOTRACE("GxSeparator::~GxSeparator");
  rep->destroy();
}

void GxSeparator::readFrom(IO::Reader& reader)
{
DOTRACE("GxSeparator::readFrom");

  for(unsigned int i = 0; i < rep->children.size(); ++i)
    {
      rep->children[i]->sigNodeChanged
        .disconnect(this->sigNodeChanged.slot());
    }

  rep->children.clear();
  IO::ReadUtils::readObjectSeq<GxNode>(
          reader, "children", std::back_inserter(rep->children));

  for(unsigned int i = 0; i < rep->children.size(); ++i)
    {
      rep->children[i]->sigNodeChanged
        .connect(this->sigNodeChanged.slot());
    }

  this->sigNodeChanged.emit();
}

void GxSeparator::writeTo(IO::Writer& writer) const
{
DOTRACE("GxSeparator::writeTo");
  IO::WriteUtils::writeObjectSeq(writer, "children",
                                 rep->children.begin(),
                                 rep->children.end());
}

GxSeparator::ChildId GxSeparator::addChild(Util::Ref<GxNode> item)
{
DOTRACE("GxSeparator::addChild");

  rep->ensureNoCycle(item.get());

  rep->children.push_back(item);

  item->sigNodeChanged.connect(this->sigNodeChanged.slot());

  this->sigNodeChanged.emit();

  return (rep->children.size() - 1);
}

void GxSeparator::insertChild(Util::Ref<GxNode> item, ChildId at_index)
{
DOTRACE("GxSeparator::insertChild");

  rep->ensureNoCycle(item.get());

  if (at_index > rep->children.size())
    at_index = rep->children.size();

  rep->children.insert(rep->children.begin()+at_index, item);

  item->sigNodeChanged.connect(this->sigNodeChanged.slot());

  this->sigNodeChanged.emit();
}

void GxSeparator::removeChildAt(ChildId index)
{
DOTRACE("GxSeparator::removeChildAt");
  if (index < rep->children.size())
    {
      rep->children[index]->sigNodeChanged
        .disconnect(this->sigNodeChanged.slot());
      rep->children.erase(rep->children.begin()+index);

      this->sigNodeChanged.emit();
    }
}

void GxSeparator::removeChild(Util::Ref<GxNode> item)
{
DOTRACE("GxSeparator::removeChild");

  const Util::UID target = item.id();

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
DOTRACE("GxSeparator::numChildren");
  return rep->children.size();
}

Util::Ref<GxNode> GxSeparator::getChild(ChildId index) const
{
DOTRACE("GxSeparator::getChild");
  if (index >= rep->children.size())
    {
      throw Util::Error(fstring("GxSeparator has no child with index '",
                                index, "'"));
    }

  return rep->children[index];
}

Util::FwdIter<Util::Ref<GxNode> > GxSeparator::children() const
{
DOTRACE("GxSeparator::children");

  return Util::FwdIter<Util::Ref<GxNode> >(rep->children.begin(),
                                           rep->children.end());
}

class GxSepIter : public Util::FwdIterIfx<const Util::Ref<GxNode> >
{
public:
  GxSepIter(GxSeparator* root) :
    itsNodes()
  {
    for (Util::FwdIter<Util::Ref<GxNode> > itr(root->children());
         itr.isValid();
         ++itr)
      {
        addDeepChildren(*itr);
      }
  }

  void addDeepChildren(Util::Ref<GxNode> node)
  {
    for (Util::FwdIter<const Util::Ref<GxNode> > itr(node->deepChildren());
         itr.isValid();
         ++itr)
      {
        itsNodes.push_back(*itr);
      }
  }

  typedef const Util::Ref<GxNode> ValType;

  virtual Util::FwdIterIfx<ValType>* clone() const
  {
    return new GxSepIter(*this);
  }

  virtual bool     atEnd()  const { return itsNodes.empty(); }
  virtual ValType&   get()  const { return itsNodes.front(); }
  virtual void      next()        { if (!atEnd()) itsNodes.pop_front(); }

private:
  // Want to use a list instead of a vector-type container here since we
  // need both push_back() and pop_front(). Could potentially use a deque
  // instead.
  mutable std::list<Util::Ref<GxNode> > itsNodes;
};

Util::FwdIter<const Util::Ref<GxNode> > GxSeparator::deepChildren()
{
DOTRACE("GxSeparator::deepChildren");

  return Util::FwdIter<const Util::Ref<GxNode> >
    (shared_ptr<GxSepIter>(new GxSepIter(this)));
}

bool GxSeparator::contains(GxNode* other) const
{
DOTRACE("GxSeparator::contains");
  return rep->contains(other);
}

void GxSeparator::getBoundingCube(Gfx::Bbox& bbox) const
{
DOTRACE("GxSeparator::getBoundingCube");

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
DOTRACE("GxSeparator::getDebugMode");

  return rep->debugMode;
}

void GxSeparator::setDebugMode(bool b)
{
DOTRACE("GxSeparator::setDebugMode");

  if (rep->debugMode != b)
    {
      rep->debugMode = b;
      this->sigNodeChanged.emit();
    }
}

void GxSeparator::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxSeparator::draw");

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

static const char vcid_gxseparator_cc[] = "$Header$";
#endif // !GXSEPARATOR_CC_DEFINED
