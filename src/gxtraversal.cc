///////////////////////////////////////////////////////////////////////
//
// gxtraversal.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Nov  3 00:24:54 2000
// written: Tue Jun 12 11:17:12 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTRAVERSAL_CC_DEFINED
#define GXTRAVERSAL_CC_DEFINED

#include "gx/gxtraversal.h"

#include "gx/gxseparator.h"

#include "util/dlink_list.h"
#include "util/ref.h"

#include "util/trace.h"

class GxTraversal::Impl {
public:
  Impl(const GxNode* root) :
    itsNodes()
    {
      addNode(root);
    }

  void addNode(const GxNode* node)
    {
    DOTRACE("GxTraversal::Impl::addNode");
      if (node == 0) return;

      const GxSeparator* sep = dynamic_cast<const GxSeparator*>(node);
      if (sep == 0)
        {
          itsNodes.push_back(node);
        }
      else
        {
          for(GxSeparator::ConstChildItr
                itr = sep->beginChildren(),
                end = sep->endChildren();
              itr != end;
              ++itr)
            {
              addNode(itr->get());
            }
        }
    }

  bool hasMore() const
    {
      return !itsNodes.empty();
    }

  const GxNode* current() const
    {
      return itsNodes.front();
    }

  void advance()
    {
      if (!itsNodes.empty()) itsNodes.pop_front();
    }

private:
  dlink_list<const GxNode*> itsNodes;
};

GxTraversal::GxTraversal(const GxNode* root) :
  itsImpl(new Impl(root))
{
DOTRACE("GxTraversal::GxTraversal");
}

GxTraversal::~GxTraversal()
{
DOTRACE("GxTraversal::~GxTraversal");
  delete itsImpl;
}

void GxTraversal::addNode(const GxNode* node) {
DOTRACE("GxTraversal::addNode");
  itsImpl->addNode(node);
}

bool GxTraversal::hasMore() const {
DOTRACE("GxTraversal::hasMore");
  return itsImpl->hasMore();
}

const GxNode* GxTraversal::current() const {
DOTRACE("GxTraversal::current");
  return itsImpl->current();
}

void GxTraversal::advance() {
DOTRACE("GxTraversal::advance");
  itsImpl->advance();
}

static const char vcid_gxtraversal_cc[] = "$Header$";
#endif // !GXTRAVERSAL_CC_DEFINED
