///////////////////////////////////////////////////////////////////////
//
// gbvec.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 16 00:10:45 2000
// written: Wed Sep 25 18:52:20 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBVEC_H_DEFINED
#define GBVEC_H_DEFINED

#include "util/multivalue.h"

#include "gx/vec3.h"

/// GbVec3 is a field class for holding 3-D vectors/points/distances.
template <class T>
class GbVec3 : public Gfx::Vec3<T>,
               public TMultiValue<T>
{
public:
  GbVec3(T x_=T(), T y_=T(), T z_=T());
  virtual ~GbVec3();

  // These help to disambiguate function calls to set()
        Gfx::Vec3<T>& vec()       { return *this; }
  const Gfx::Vec3<T>& vec() const { return *this; }

  virtual fstring getNativeTypeName() const;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

  virtual unsigned int numValues() const;
  virtual const T* constBegin() const;
};

static const char vcid_gbvec_h[] = "$Header$";
#endif // !GBVEC_H_DEFINED
