///////////////////////////////////////////////////////////////////////
//
// gxdrawstyle.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Aug 29 10:16:30 2001
// written: Fri Jan 18 16:06:58 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXDRAWSTYLE_H_DEFINED
#define GXDRAWSTYLE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GXNODE_H_DEFINED)
#include "gfx/gxnode.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

class GxDrawStyle : public GxNode, public FieldContainer
{
private:
  GxDrawStyle(const GxDrawStyle&);
  GxDrawStyle& operator=(const GxDrawStyle&);

protected:
  /// Default constructor.
  GxDrawStyle();

public:
  virtual ~GxDrawStyle();

  static GxDrawStyle* make() { return new GxDrawStyle; }

private:
  bool polygonFill;
  double pointSize;
  double lineWidth;
  int linePattern;

public:
  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  static const FieldMap& classFields();

  virtual void draw(Gfx::Canvas& canvas) const;
  virtual void undraw(Gfx::Canvas& canvas) const;
};

static const char vcid_gxdrawstyle_h[] = "$Header$";
#endif // !GXDRAWSTYLE_H_DEFINED
