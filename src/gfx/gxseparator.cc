///////////////////////////////////////////////////////////////////////
//
// gxseparator.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Nov  2 11:24:04 2000
// written: Thu Nov  2 13:20:45 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSEPARATOR_CC_DEFINED
#define GXSEPARATOR_CC_DEFINED

#include "gx/gxseparator.h"

#include "gwt/canvas.h"
#include "io/iditem.h"

#include "util/error.h"
#include "util/minivec.h"

#include "util/trace.h"

class GxSeparator::Impl {
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl() {}
  ~Impl() {}

  typedef minivec<IdItem<GxNode> > VecType;
  VecType itsChildren;
};

GxSeparator* GxSeparator::make() {
DOTRACE("GxSeparator::make");
  return new GxSeparator;
}

GxSeparator::GxSeparator() :
  itsImpl(new Impl)
{
DOTRACE("GxSeparator::GxSeparator");
}

GxSeparator::~GxSeparator() {
DOTRACE("GxSeparator::~GxSeparator");
  delete itsImpl;
}

GxSeparator::ChildId GxSeparator::addChild(int ioId) {
DOTRACE("GxSeparator::addChild");
  itsImpl->itsChildren.push_back(IdItem<GxNode>(ioId)); 
  return (itsImpl->itsChildren.size() - 1);
}

void GxSeparator::insertChild(int ioId, ChildId at_index) {
DOTRACE("GxSeparator::insertChild");
  if (at_index < 0)
	 at_index = 0;

  if (at_index > itsImpl->itsChildren.size())
	 at_index = itsImpl->itsChildren.size();

  itsImpl->itsChildren.insert(itsImpl->itsChildren.begin()+at_index,
										IdItem<GxNode>(ioId));
}

IdItem<GxNode> GxSeparator::getChild(ChildId index) const {
DOTRACE("GxSeparator::getChild");
  if (index < 0 || index >= itsImpl->itsChildren.size())
	 {
		ErrorWithMsg err("GxSeparator has no child with index '");
		err.appendNumber(index);
		err.appendMsg("'");
		throw err;
	 }

  return itsImpl->itsChildren[index];
}

void GxSeparator::removeChildId(ChildId index) {
DOTRACE("GxSeparator::removeChildId");
  if (index >= 0 && index < itsImpl->itsChildren.size())
	 itsImpl->itsChildren.erase(itsImpl->itsChildren.begin()+index);
}

void GxSeparator::removeChildItem(int ioId) {
DOTRACE("GxSeparator::removeChildItem");
  for(Impl::VecType::iterator
		  itr = itsImpl->itsChildren.begin(),
		  end = itsImpl->itsChildren.end();
		itr != end;
		++itr)
	 {
		if ( (*itr)->id() == ioId )
		  {
			 itsImpl->itsChildren.erase(itr);
			 break;
		  }
	 }   
}

GxSeparator::ChildItr GxSeparator::beginChildren() const {
DOTRACE("GxSeparator::beginChildren");
  return itsImpl->itsChildren.begin();
}

GxSeparator::ChildItr GxSeparator::endChildren() const {
DOTRACE("GxSeparator::endChildren");
  return itsImpl->itsChildren.end();
}

void GxSeparator::draw(GWT::Canvas& canvas) const {
DOTRACE("GxSeparator::draw");

  GWT::Canvas::StateSaver state(canvas);

  for(ChildItr itr = beginChildren(), end = endChildren();
		itr != end;
		++itr)
	 {
		(*itr)->draw(canvas);
	 }
}

void GxSeparator::undraw(GWT::Canvas& canvas) const {
DOTRACE("GxSeparator::undraw");

  GWT::Canvas::StateSaver state(canvas);

  for(ChildItr itr = beginChildren(), end = endChildren();
		itr != end;
		++itr)
	 {
		(*itr)->undraw(canvas);
	 }
}

static const char vcid_gxseparator_cc[] = "$Header$";
#endif // !GXSEPARATOR_CC_DEFINED
