///////////////////////////////////////////////////////////////////////
//
// gxtraversal.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Nov  3 00:24:54 2000
// written: Fri Aug 17 16:09:33 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTRAVERSAL_CC_DEFINED
#define GXTRAVERSAL_CC_DEFINED

#include "gfx/gxtraversal.h"

#include "gfx/gxseparator.h"

#include "util/dlink_list.h"
#include "util/iter.h"
#include "util/ref.h"

#include "util/trace.h"

class GxTraversal::Impl : public Util::FwdIterIfx<const Util::Ref<GxNode> >
{
public:
  Impl(Util::Ref<GxNode> root) :
    itsNodes()
  {
    addNode(root);
  }

  void addNode(Util::Ref<GxNode> node)
  {
    DOTRACE("GxTraversal::Impl::addNode");

    const GxSeparator* sep = dynamic_cast<const GxSeparator*>(node.get());
    if (sep == 0)
      {
        itsNodes.push_back(Util::Ref<GxNode>(node));
      }
    else
      {
        for (Util::FwdIter<Util::Ref<GxNode> > itr(sep->children());
             itr.isValid();
             ++itr)
          {
            addNode(*itr);
          }
      }
  }

  typedef const Util::Ref<GxNode> ValType;

  virtual Util::FwdIterIfx<ValType>* clone() const
  {
    return new Impl(*this);
  }

  virtual bool     atEnd()  const { return itsNodes.empty(); }
  virtual ValType&   get()  const { return itsNodes.front(); }
  virtual void      next()        { if (!atEnd()) itsNodes.pop_front(); }

private:
  mutable dlink_list<Util::Ref<GxNode> > itsNodes;
};

GxTraversal::GxTraversal(Util::Ref<GxNode> root) :
  Util::FwdIter<const Util::Ref<GxNode> >(shared_ptr<Impl>(new Impl(root)))
{
DOTRACE("GxTraversal::GxTraversal");
}

static const char vcid_gxtraversal_cc[] = "$Header$";
#endif // !GXTRAVERSAL_CC_DEFINED
