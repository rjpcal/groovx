///////////////////////////////////////////////////////////////////////
//
// assocarray.h
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Oct 14 18:40:34 2004
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

#ifndef ASSOCARRAY_H_DEFINED
#define ASSOCARRAY_H_DEFINED

class fstring;

namespace rutz
{
  class file_pos;

  /// \c rutz::assoc_array provides a non-typesafe wrapper around \c std::map.
  /** The use must provide a pointer to a function that knows how to
      properly destroy the actual contained objects according to their
      true type. */

  class assoc_array
  {
  public:
    /// Function type for destroying elements.
    typedef void (kill_func_t) (void*);

    /// Default constructor.
    assoc_array(kill_func_t* f);

    /// Virtual destructor.
    ~assoc_array();

    /// Raise an exception reporting an unknown key.
    void throw_for_key(const char* key, const rutz::file_pos& pos);

    /// Raise an exception reporting an unknown key.
    void throw_for_key(const fstring& key, const rutz::file_pos& pos);

    /// Retrieve the object associated with the tag \a name.
    void* get_value_for_key(const fstring& name) const;

    /// Retrieve the object associated with the tag \a name.
    void* get_value_for_key(const char* name) const;

    /// Associate the object at \a ptr with the tag \a name.
    void set_value_for_key(const char* name, void* ptr);

    /// Clear all entries, calling the kill function for each.
    void clear();

  private:
    assoc_array(const assoc_array&);
    assoc_array& operator=(const assoc_array&);

    struct impl;
    impl* const rep;
  };
}

static const char vcid_assocarray_h[] = "$Header$";
#endif // !ASSOCARRAY_H_DEFINED
