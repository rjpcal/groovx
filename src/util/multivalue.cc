///////////////////////////////////////////////////////////////////////
//
// multivalue.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Aug 22 16:58:56 2001
// written: Wed Aug 22 17:36:47 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MULTIVALUE_CC_DEFINED
#define MULTIVALUE_CC_DEFINED

#include "util/multivalue.h"

#include <strstream.h>

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
            throw ValueError("error while scanning a multi-value");
        }
      else
        {
          if (is.fail() && !is.eof())
            throw ValueError("error while scanning a multi-value");
        }
    }
}

template <class T>
const char* TMultiValue<T>::get(Util::TypeCue<const char*>) const
{
  static char buf[256];
  ostrstream ost(buf, 256);
  printTo(ost);
  ost << '\0';
  return buf;
}

template <class T>
void TMultiValue<T>::assignTo(Value& other) const
{ other.set(this->get(Util::TypeCue<const char*>())); }

template <class T>
void TMultiValue<T>::assignFrom(const Value& other)
{
  istrstream ist(other.get(Util::TypeCue<const char*>()));
  DebugEvalNL(other.get(Util::TypeCue<const char*>()));
  scanFrom(ist);
}

template class TMultiValue<int>;
template class TMultiValue<double>;

static const char vcid_multivalue_cc[] = "$Header$";
#endif // !MULTIVALUE_CC_DEFINED
