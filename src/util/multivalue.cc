///////////////////////////////////////////////////////////////////////
//
// multivalue.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Aug 22 16:58:56 2001
// written: Tue Nov  5 06:40:19 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MULTIVALUE_CC_DEFINED
#define MULTIVALUE_CC_DEFINED

#include "util/multivalue.h"

#include "util/error.h"

#include <iostream>

#include "util/debug.h"
#include "util/trace.h"

//  template <class T>
//  TMultiValue<T>::~TMultiValue() {}

#define INSTANTIATE(T)                                          \
                                                                \
template <>                                                     \
void TMultiValue<T>::printTo(STD_IO::ostream& os) const         \
{                                                               \
DOTRACE("TMultiValue<T>::printTo");                             \
  const T* dat = constBegin();                                  \
  const T* end = constEnd();                                    \
                                                                \
  while (dat < end)                                             \
    {                                                           \
      os << *dat;                                               \
      if (++dat < end)                                          \
        os << ' ';                                              \
    }                                                           \
}                                                               \
                                                                \
template <>                                                     \
void TMultiValue<T>::scanFrom(STD_IO::istream& is)              \
{                                                               \
DOTRACE("TMultiValue<T>::scanFrom");                            \
                                                                \
  if (is.fail())                                                \
    throw Util::Error("istream started out in fail state");     \
  if (is.eof())                                                 \
    throw Util::Error("istream started out in eof state");      \
                                                                \
  T* dat = mutableBegin();                                      \
  T* end = mutableEnd();                                        \
                                                                \
  while (dat < end)                                             \
    {                                                           \
      is >> *dat;                                               \
      dbgEvalNL(3, *dat);                                       \
      ++dat;                                                    \
      if (dat < end)                                            \
        {                                                       \
          if (is.fail() || is.eof())                            \
            throw Util::Error("stream underflow "               \
                              "while scanning a multi-value");  \
        }                                                       \
      else                                                      \
        {                                                       \
          if (is.fail() && !is.eof())                           \
            throw Util::Error("stream error "                   \
                              "while scanning a multi-value");  \
        }                                                       \
    }                                                           \
}

INSTANTIATE(double);
INSTANTIATE(int);

template class TMultiValue<int>;
template class TMultiValue<double>;

static const char vcid_multivalue_cc[] = "$Header$";
#endif // !MULTIVALUE_CC_DEFINED
