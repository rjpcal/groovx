///////////////////////////////////////////////////////////////////////
//
// gxtraversal.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Nov  3 00:24:54 2000
// written: Fri Nov  3 00:33:54 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTRAVERSAL_CC_DEFINED
#define GXTRAVERSAL_CC_DEFINED

#include "gx/gxtraversal.h"

#include "gx/gxseparator.h"

#include "io/iditem.h"

#include <list>

class GxTraversal::Impl {
public:
  Impl(const GxNode* root) :
	 itsNodes()
	 {
		addNode(root);
	 }

  void addNode(const GxNode* node)
	 {
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
  list<const GxNode*> itsNodes;
};

GxTraversal::GxTraversal(const GxNode* root) :
  itsImpl(new Impl(root))
{}

GxTraversal::~GxTraversal()
{ delete itsImpl; }

void GxTraversal::addNode(const GxNode* node)
{ itsImpl->addNode(node); }

bool GxTraversal::hasMore() const
{ return itsImpl->hasMore(); }

const GxNode* GxTraversal::current() const
{ return itsImpl->current(); }

void GxTraversal::advance()
{ itsImpl->advance(); }

static const char vcid_gxtraversal_cc[] = "$Header$";
#endif // !GXTRAVERSAL_CC_DEFINED
