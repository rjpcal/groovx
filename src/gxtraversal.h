///////////////////////////////////////////////////////////////////////
//
// gxtraversal.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 23:42:45 2000
// written: Fri Aug 17 15:54:47 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTRAVERSAL_H_DEFINED
#define GXTRAVERSAL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ITER_H_DEFINED)
#include "util/iter.h"
#endif

namespace Util
{
  template <class T> class Ref;
}

class GxNode;

class GxTraversal : public Util::FwdIter<const Util::Ref<GxNode> >
{
public:
  GxTraversal(Util::Ref<GxNode> node);

  bool hasMore() const { return isValid(); }
  void advance() { next(); }

private:
  class Impl;
};

static const char vcid_gxtraversal_h[] = "$Header$";
#endif // !GXTRAVERSAL_H_DEFINED
