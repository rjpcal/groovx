///////////////////////////////////////////////////////////////////////
//
// multivalue.cc
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Aug 22 16:58:56 2001
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef MULTIVALUE_CC_DEFINED
#define MULTIVALUE_CC_DEFINED

#include "util/multivalue.h"

#include "util/error.h"

#include <iostream>

#include "util/debug.h"
DBG_REGISTER
#include "util/trace.h"

template <class T>
TMultiValue<T>::TMultiValue(int num) : itsNumValues(num) {}

template <class T>
TMultiValue<T>::~TMultiValue() {}

template <class T>
void TMultiValue<T>::printTo(STD_IO::ostream& os) const
{
DOTRACE("TMultiValue<T>::printTo");
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
DOTRACE("TMultiValue<T>::scanFrom");

  if (is.fail())
    throw Util::Error("istream started out in fail state", SRC_POS);
  if (is.eof())
    throw Util::Error("istream started out in eof state", SRC_POS);

  T* dat = mutableBegin();
  T* end = mutableEnd();

  while (dat < end)
    {
      is >> *dat;
      dbgEvalNL(3, *dat);
      ++dat;
      if (dat < end)
        {
          if (is.fail() || is.eof())
            throw Util::Error("stream underflow "
                              "while scanning a multi-value", SRC_POS);
        }
      else
        {
          if (is.fail() && !is.eof())
            throw Util::Error("stream error "
                              "while scanning a multi-value", SRC_POS);
        }
    }
}

template class TMultiValue<int>;
template class TMultiValue<double>;

static const char vcid_multivalue_cc[] = "$Header$";
#endif // !MULTIVALUE_CC_DEFINED
