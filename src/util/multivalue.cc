///////////////////////////////////////////////////////////////////////
//
// multivalue.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Aug 22 16:58:56 2001
// written: Fri Jan 18 16:07:04 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MULTIVALUE_CC_DEFINED
#define MULTIVALUE_CC_DEFINED

#include "util/multivalue.h"

#include "util/error.h"

#include <iostream.h>

#include "util/debug.h"

template <class T>
void TMultiValue<T>::printTo(STD_IO::ostream& os) const
{
  const T* dat = constBegin();
  const T* end = constEnd();

  while (dat < end)
    {
      os << *dat;
      if (++dat < end)
        os << ' ';
    }
}

template <class T>
void TMultiValue<T>::scanFrom(STD_IO::istream& is)
{
  T* dat = mutableBegin();
  T* end = mutableEnd();

  while (dat < end)
    {
      is >> *dat++;
      if (dat < end)
        {
          if (is.fail() || is.eof())
            throw Util::Error("error while scanning a multi-value");
        }
      else
        {
          if (is.fail() && !is.eof())
            throw Util::Error("error while scanning a multi-value");
        }
    }
}

template class TMultiValue<int>;
template class TMultiValue<double>;

static const char vcid_multivalue_cc[] = "$Header$";
#endif // !MULTIVALUE_CC_DEFINED
