///////////////////////////////////////////////////////////////////////
//
// gxseparator.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 11:24:04 2000
// written: Fri Jul  5 14:11:35 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSEPARATOR_CC_DEFINED
#define GXSEPARATOR_CC_DEFINED

#include "gfx/gxseparator.h"

#include "gfx/canvas.h"

#include "gx/box.h"
#include "gx/rect.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "util/dlink_list.h"
#include "util/error.h"
#include "util/iter.h"
#include "util/minivec.h"
#include "util/ref.h"
#include "util/volatileobject.h"

#include "util/trace.h"

class GxSeparator::Impl : public Util::VolatileObject
{
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(GxSeparator* owner) :
    itsOwner(owner),
    itsChildren()
  {}
  ~Impl() {}

  static Impl* make(GxSeparator* owner) { return new Impl(owner); }

  bool contains(GxNode* other) const
    {
      if (itsOwner == other) return true;

      for(VecType::const_iterator
            itr = itsChildren.begin(),
            end = itsChildren.end();
          itr != end;
          ++itr)
        {
          if ((*itr)->contains(other)) return true;
        }

      return false;
    }

  void ensureNoCycle(GxNode* other) const
    {
      if (other->contains(itsOwner))
        {
          throw Util::Error("couldn't add node without generating a cycle");
        }
    }

  GxSeparator* itsOwner;

  typedef minivec<Ref<GxNode> > VecType;
  VecType itsChildren;
};

GxSeparator* GxSeparator::make()
{
DOTRACE("GxSeparator::make");
  return new GxSeparator;
}

GxSeparator::GxSeparator() :
  itsImpl(Impl::make(this))
{
DOTRACE("GxSeparator::GxSeparator");
}

GxSeparator::~GxSeparator()
{
DOTRACE("GxSeparator::~GxSeparator");
  itsImpl->destroy();
}

void GxSeparator::readFrom(IO::Reader* reader)
{
DOTRACE("GxSeparator::readFrom");
  itsImpl->itsChildren.clear();
  IO::ReadUtils::readObjectSeq<GxNode>(
          reader, "children", std::back_inserter(itsImpl->itsChildren));
}

void GxSeparator::writeTo(IO::Writer* writer) const
{
DOTRACE("GxSeparator::writeTo");
  IO::WriteUtils::writeObjectSeq(writer, "children",
                                 itsImpl->itsChildren.begin(),
                                 itsImpl->itsChildren.end());
}

GxSeparator::ChildId GxSeparator::addChild(Util::Ref<GxNode> item)
{
DOTRACE("GxSeparator::addChild");

  itsImpl->ensureNoCycle(item.get());

  itsImpl->itsChildren.push_back(item);

  item->sigNodeChanged.connect(this->sigNodeChanged.slot());

  this->sigNodeChanged.emit();

  return (itsImpl->itsChildren.size() - 1);
}

void GxSeparator::insertChild(Util::Ref<GxNode> item, ChildId at_index)
{
DOTRACE("GxSeparator::insertChild");

  itsImpl->ensureNoCycle(item.get());

  if (at_index > itsImpl->itsChildren.size())
    at_index = itsImpl->itsChildren.size();

  itsImpl->itsChildren.insert(itsImpl->itsChildren.begin()+at_index,
                              item);

  item->sigNodeChanged.connect(this->sigNodeChanged.slot());

  this->sigNodeChanged.emit();
}

void GxSeparator::removeChildAt(ChildId index)
{
DOTRACE("GxSeparator::removeChildAt");
  if (index < itsImpl->itsChildren.size())
    {
      itsImpl->itsChildren[index]->sigNodeChanged
        .disconnect(this->sigNodeChanged.slot());
      itsImpl->itsChildren.erase(itsImpl->itsChildren.begin()+index);

      this->sigNodeChanged.emit();
    }
}

void GxSeparator::removeChild(Util::Ref<GxNode> item)
{
DOTRACE("GxSeparator::removeChild");

  const Util::UID target = item.id();

  for(Impl::VecType::iterator
        itr = itsImpl->itsChildren.begin(),
        end = itsImpl->itsChildren.end();
      itr != end;
      ++itr)
    {
      if ( (*itr)->id() == target )
        {
          (*itr)->sigNodeChanged.disconnect(this->sigNodeChanged.slot());
          itsImpl->itsChildren.erase(itr);
          this->sigNodeChanged.emit();
          break;
        }
    }
}

unsigned int GxSeparator::numChildren() const
{
DOTRACE("GxSeparator::numChildren");
  return itsImpl->itsChildren.size();
}

Ref<GxNode> GxSeparator::getChild(ChildId index) const
{
DOTRACE("GxSeparator::getChild");
  if (index >= itsImpl->itsChildren.size())
    {
      throw Util::Error(fstring("GxSeparator has no child with index '",
                                index, "'"));
    }

  return itsImpl->itsChildren[index];
}

Util::FwdIter<Util::Ref<GxNode> > GxSeparator::children() const
{
DOTRACE("GxSeparator::children");

  return Util::FwdIter<Util::Ref<GxNode> >(itsImpl->itsChildren.begin(),
                                           itsImpl->itsChildren.end());
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

  virtual bool     atEnd()  const { return itsNodes.is_empty(); }
  virtual ValType&   get()  const { return itsNodes.front(); }
  virtual void      next()        { if (!atEnd()) itsNodes.pop_front(); }

private:
  mutable dlink_list<Util::Ref<GxNode> > itsNodes;
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
  return itsImpl->contains(other);
}

void GxSeparator::getBoundingCube(Gfx::Box<double>& bbox,
                                  Gfx::Canvas& canvas) const
{
DOTRACE("GxSeparator::getBoundingCube");

  Gfx::Box<double> mybox;

  if (!itsImpl->itsChildren.empty())
    {
      Gfx::MatrixSaver state(canvas);
      Gfx::AttribSaver attribs(canvas);

      for(Impl::VecType::reverse_iterator
            itr = itsImpl->itsChildren.rbegin(),
            end = itsImpl->itsChildren.rend();
          itr != end;
          ++itr)
        {
          (*itr)->getBoundingCube(mybox, canvas);
        }
    }

  bbox.unionize(mybox);
}

void GxSeparator::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxSeparator::draw");

  Gfx::Box<double> cube;

  getBoundingCube(cube, canvas);

  canvas.drawBox(cube);

  if (!itsImpl->itsChildren.empty())
    {
      Gfx::MatrixSaver state(canvas);
      Gfx::AttribSaver attribs(canvas);

      for(Impl::VecType::iterator
            itr = itsImpl->itsChildren.begin(),
            end = itsImpl->itsChildren.end();
          itr != end;
          ++itr)
        {
          (*itr)->draw(canvas);
        }
    }
}

static const char vcid_gxseparator_cc[] = "$Header$";
#endif // !GXSEPARATOR_CC_DEFINED
