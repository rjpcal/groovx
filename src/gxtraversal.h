///////////////////////////////////////////////////////////////////////
//
// gxtraversal.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 23:42:45 2000
// written: Fri May 11 20:43:14 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTRAVERSAL_H_DEFINED
#define GXTRAVERSAL_H_DEFINED

class GxNode;

class GxTraversal {
public:
  GxTraversal(const GxNode* root);
  ~GxTraversal();

  void addNode(const GxNode* node);
  bool hasMore() const;
  const GxNode* current() const;
  void advance();

private:
  GxTraversal(const GxTraversal&);
  GxTraversal& operator=(const GxTraversal&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_gxtraversal_h[] = "$Header$";
#endif // !GXTRAVERSAL_H_DEFINED
