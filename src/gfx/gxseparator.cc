///////////////////////////////////////////////////////////////////////
//
// gxseparator.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 11:24:04 2000
// written: Fri Aug 17 15:11:34 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSEPARATOR_CC_DEFINED
#define GXSEPARATOR_CC_DEFINED

#include "gfx/gxseparator.h"

#include "gfx/canvas.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "util/error.h"
#include "util/iter.h"
#include "util/minivec.h"
#include "util/observer.h"
#include "util/ref.h"

#include "util/trace.h"

class GxSeparator::Impl : public Util::Observer {
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(GxSeparator* owner) : itsOwner(owner), itsChildren() {}
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

  virtual void receiveStateChangeMsg(const Observable*)
  {
    DOTRACE("GxSeparator::Impl::receiveStateChangeMsg");
    itsOwner->sendStateChangeMsg();
  }

  virtual bool isVolatile() const { return true; }

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
  itsImpl->incrRefCount();
}

GxSeparator::~GxSeparator()
{
DOTRACE("GxSeparator::~GxSeparator");
  itsImpl->decrRefCount();
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

  item->attach(itsImpl);

  sendStateChangeMsg();

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

  item->attach(itsImpl);

  sendStateChangeMsg();
}

void GxSeparator::removeChildAt(ChildId index)
{
DOTRACE("GxSeparator::removeChildAt");
  if (index < itsImpl->itsChildren.size())
    {
      itsImpl->itsChildren[index]->detach(itsImpl);
      itsImpl->itsChildren.erase(itsImpl->itsChildren.begin()+index);

      sendStateChangeMsg();
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
          (*itr)->detach(itsImpl);
          itsImpl->itsChildren.erase(itr);
          sendStateChangeMsg();
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

bool GxSeparator::contains(GxNode* other) const
{
DOTRACE("GxSeparator::contains");
  return itsImpl->contains(other);
}

void GxSeparator::draw(Gfx::Canvas& canvas) const
{
DOTRACE("GxSeparator::draw");

  Gfx::Canvas::StateSaver state(canvas);

  for(Impl::VecType::iterator
        itr = itsImpl->itsChildren.begin(),
        end = itsImpl->itsChildren.end();
      itr != end;
      ++itr)
    {
      (*itr)->draw(canvas);
    }
}

void GxSeparator::undraw(Gfx::Canvas& canvas) const
{
DOTRACE("GxSeparator::undraw");

  Gfx::Canvas::StateSaver state(canvas);

  for(Impl::VecType::iterator
        itr = itsImpl->itsChildren.begin(),
        end = itsImpl->itsChildren.end();
      itr != end;
      ++itr)
    {
      (*itr)->undraw(canvas);
    }
}

static const char vcid_gxseparator_cc[] = "$Header$";
#endif // !GXSEPARATOR_CC_DEFINED
