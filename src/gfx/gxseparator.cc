///////////////////////////////////////////////////////////////////////
//
// gxseparator.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Nov  2 11:24:04 2000
// written: Thu Nov  2 14:34:58 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSEPARATOR_CC_DEFINED
#define GXSEPARATOR_CC_DEFINED

#include "gx/gxseparator.h"

#include "gwt/canvas.h"

#include "io/iditem.h"
#include "io/readutils.h"
#include "io/writeutils.h"

#include "util/error.h"
#include "util/minivec.h"

#include "util/trace.h"

class GxSeparator::Impl {
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(GxSeparator* owner) : itsOwner(owner) {}
  ~Impl() {}

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
			 throw ErrorWithMsg("couldn't add node without generating a cycle");
		  }
	 }

  GxSeparator* itsOwner;

  typedef minivec<IdItem<GxNode> > VecType;
  VecType itsChildren;
};

GxSeparator* GxSeparator::make() {
DOTRACE("GxSeparator::make");
  return new GxSeparator;
}

GxSeparator::GxSeparator() :
  itsImpl(new Impl(this))
{
DOTRACE("GxSeparator::GxSeparator");
}

GxSeparator::~GxSeparator() {
DOTRACE("GxSeparator::~GxSeparator");
  delete itsImpl;
}

void GxSeparator::readFrom(IO::Reader* reader) {
DOTRACE("GxSeparator::readFrom");
  itsImpl->itsChildren.clear();
  IO::ReadUtils::readObjectSeq<GxNode>(
          reader, "children",
			 IdItem<GxNode>::makeInserter(itsImpl->itsChildren));
}

void GxSeparator::writeTo(IO::Writer* writer) const {
DOTRACE("GxSeparator::writeTo");
  IO::WriteUtils::writeSmartPtrSeq(writer, "children",
											  itsImpl->itsChildren.begin(),
											  itsImpl->itsChildren.end());
}

GxSeparator::ChildId GxSeparator::addChild(int ioUid) {
DOTRACE("GxSeparator::addChild");

  IdItem<GxNode> item(ioUid);

  itsImpl->ensureNoCycle(item.get());

  itsImpl->itsChildren.push_back(item);
  return (itsImpl->itsChildren.size() - 1);
}

void GxSeparator::insertChild(int ioUid, ChildId at_index) {
DOTRACE("GxSeparator::insertChild");

  IdItem<GxNode> item(ioUid);

  itsImpl->ensureNoCycle(item.get());

  if (at_index < 0)
	 at_index = 0;

  if (at_index > itsImpl->itsChildren.size())
	 at_index = itsImpl->itsChildren.size();

  itsImpl->itsChildren.insert(itsImpl->itsChildren.begin()+at_index,
										item);
}

void GxSeparator::removeChildId(ChildId index) {
DOTRACE("GxSeparator::removeChildId");
  if (index >= 0 && index < itsImpl->itsChildren.size())
	 itsImpl->itsChildren.erase(itsImpl->itsChildren.begin()+index);
}

void GxSeparator::removeChildUid(int io_uid) {
DOTRACE("GxSeparator::removeChildUid");
  for(Impl::VecType::iterator
		  itr = itsImpl->itsChildren.begin(),
		  end = itsImpl->itsChildren.end();
		itr != end;
		++itr)
	 {
		if ( (*itr)->id() == io_uid )
		  {
			 itsImpl->itsChildren.erase(itr);
			 break;
		  }
	 }   
}

int GxSeparator::numChildren() const {
DOTRACE("GxSeparator::numChildren");
  return itsImpl->itsChildren.size();
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

GxSeparator::ChildItr GxSeparator::beginChildren() const {
DOTRACE("GxSeparator::beginChildren");
  return itsImpl->itsChildren.begin();
}

GxSeparator::ChildItr GxSeparator::endChildren() const {
DOTRACE("GxSeparator::endChildren");
  return itsImpl->itsChildren.end();
}

bool GxSeparator::contains(GxNode* other) const {
DOTRACE("GxSeparator::contains");
  return itsImpl->contains(other);
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
