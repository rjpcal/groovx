///////////////////////////////////////////////////////////////////////
//
// gxemptynode.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 14 16:59:04 2002
// written: Tue Nov 19 12:50:34 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXEMPTYNODE_H_DEFINED
#define GXEMPTYNODE_H_DEFINED

#include "gfx/gxnode.h"

//  ###################################################################
//  ===================================================================

/// GxEmptyNode is a totally empty GxNode subclass for use as "null object".

class GxEmptyNode : public GxNode
{
private:
  GxEmptyNode();
public:
  virtual ~GxEmptyNode();

  virtual void readFrom(IO::Reader*);
  virtual void writeTo(IO::Writer*) const;

  virtual void getBoundingCube(Gfx::Bbox&) const;

  virtual void draw(Gfx::Canvas&) const;

  static GxEmptyNode* make();
};

static const char vcid_gxemptynode_h[] = "$Header$";
#endif // !GXEMPTYNODE_H_DEFINED
