///////////////////////////////////////////////////////////////////////
//
// scopedptr.h
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Oct 15 14:51:04 2004
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

#ifndef SCOPEDPTR_H_DEFINED
#define SCOPEDPTR_H_DEFINED

#include "util/algo.h"

namespace rutz
{
  //  #######################################################
  //  =======================================================

  /// A smart-pointer for unshared objects.

  /** \c scoped_ptr mimics a built-in pointer except that it guarantees
      deletion of the object pointed to, either on destruction of the
      scoped_ptr or via an explicit \c reset(). */

  template<class T>
  class scoped_ptr
  {
  public:
    /// Construct with pointer to given object (or null).
    explicit scoped_ptr( T* p=0 ) throw() :
      ptr(p)
    {}

    /// Construct with pointer to given object of related type.
    template <class TT>
    explicit scoped_ptr( TT* p ) throw() :
      ptr(p)
    {}

    /// Destructor.
    ~scoped_ptr()
    { delete ptr; }

    /// Reset with pointer to different object (or null).
    void reset( T* p=0 )
    {
      if ( ptr != p )
        { delete ptr; ptr = p; }
    }

    /// Dereference.
    T& operator*() const throw()
    { return *ptr; }

    /// Dereference for member access.
    T* operator->() const throw()
    { return ptr; }

    /// Get a pointer to the referred-to object.
    T* get() const throw()
    { return ptr; }

  private:
    scoped_ptr(const scoped_ptr& other);
    scoped_ptr& operator=(const scoped_ptr& other);

    T* ptr;
  };
}

static const char vcid_scopedptr_h[] = "$Id$ $URL$";
#endif // !SCOPEDPTR_H_DEFINED
