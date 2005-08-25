/** @file rutz/factory.h template class for object factories, which
    create objects belonging to some inheritance hierarchy given just
    a type name as a string */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Jun 26 23:40:55 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_RUTZ_FACTORY_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_FACTORY_H_UTC20050626084020_DEFINED

#include "rutz/assocarray.h"
#include "rutz/demangle.h"
#include "rutz/fileposition.h"
#include "rutz/traits.h"

#include <typeinfo>

namespace rutz
{
  class fstring;

  /**
   *
   * \c rutz::creator_base is a template class that defines a single
   * abstract function, \c create(), that returns an object of type \c
   * Base.
   *
   **/
  template <class base_t>
  class creator_base
  {
  public:
    /// Virtual destructor.
    virtual ~creator_base() {}

    /// Return a clone of this Creator
    virtual creator_base* clone() const = 0;

    /// Return a new pointer (or smart pointer) to type \c Base.
    virtual base_t create() = 0;
  };


  /**
   *
   * rutz::creator_from_func implements the rutz::creator_base
   * interface by storing a pointer to function that returns an object
   * of the appropriate type.
   *
   **/
  template <class base_t, class derived_t>
  class creator_from_func : public rutz::creator_base<base_t>
  {
  public:
    /// Creator function type.
    typedef derived_t (*creator_func_t) ();

    /// Construct with a creator function.
    creator_from_func(creator_func_t func) :
      rutz::creator_base<base_t>(), m_creator_func(func) {}

    /// Clone operator.
    virtual rutz::creator_base<base_t>* clone() const
    { return new creator_from_func<base_t, derived_t>(*this); }

    /// Create an object using the creator function.
    virtual base_t create() { return base_t(m_creator_func()); }

  private:
    creator_func_t m_creator_func;
  };


  /**
   *
   * Base class for factory functionality that doesn't depend on the
   * type of the product.
   *
   **/

  class factory_base
  {
  public:
    typedef void (fallback_t)(const rutz::fstring&);

    /// Default constructor sets the fallback to null.
    factory_base() throw();

    /// Virtual no-throw destructor for proper inheritance.
    virtual ~factory_base() throw();

    /// Change the fallback function.
    void set_fallback(fallback_t* fptr) throw();

  protected:
    /// Try running the fallback function for the given type.
    void try_fallback(const rutz::fstring& type) const;

  private:
    fallback_t* m_fallback;
  };


  ///////////////////////////////////////////////////////////
  /**
   *
   * \c rutz::factory can create objects from an inheritance hierarchy
   * given only a typename. \c rutz::factory maintains a mapping from
   * typenames to \c rutz::creator_base's, and so given a typename can
   * call the \c create() function of the associated \c Creator. All
   * of the product types of a factory must be derived from \c
   * Base. The constructor is protected because factories for specific
   * types should be implemented as singleton classes derived from
   * rutz::factory.
   *
   **/
  ///////////////////////////////////////////////////////////

  template <class base_t>
  class factory : public rutz::factory_base
  {
  private:
    rutz::assoc_array<rutz::creator_base<base_t> > m_map;

  protected:
    /// Default constructor.
    factory(const char* keydescr = "object type") : m_map(keydescr) {}

    /// Virtual no-throw destructor.
    virtual ~factory() throw() {}

    /// Find a creator for the given key; otherwise return null.
    rutz::creator_base<base_t>*
    find_creator(const rutz::fstring& key) const
    {
      rutz::creator_base<base_t>* creator = m_map.get_ptr_for_key(key);

      if (creator == 0)
        {
          factory_base::try_fallback(key);

          creator = m_map.get_ptr_for_key(key);
        }

      return creator;
    }

  public:
    /// Registers a creation object with the factory.
    /** The function returns the actual key that was paired with the
        creation function.*/
    const char* register_creator(rutz::creator_base<base_t>* creator,
                                 const char* name)
    {
      m_map.set_ptr_for_key(name, creator);

      return name;
    }

    /// Registers a creation function with the factory.
    /** The default key associated with the creation function will be
        given by the type's actual C++ name. The function returns the
        actual key that was paired with the creation function.*/
    template <class derived_t>
    const char* register_creator(derived_t (*func) (),
                                 const char* key = 0)
    {
      if (key == 0)
        key = rutz::demangled_name
          (typeid(typename rutz::type_traits<derived_t>::pointee_t));

      m_map.set_ptr_for_key
        (key, new rutz::creator_from_func<base_t, derived_t>(func));

      return key;
    }

    /// Introduces an alternate key for an existing key.
    /** There must already have been a creation function registered
        for the original key. */
    void register_alias(const char* orig_key, const char* alias_key)
    {
      rutz::creator_base<base_t>* creator =
        m_map.get_ptr_for_key(orig_key);

      if (creator != 0)
        {
          m_map.set_ptr_for_key(alias_key, creator->clone());
        }
    }

    /// Query whether a given key is a valid, known key in the factory.
    bool is_valid_key(const char* key) const
    {
      return (this->find_creator(key) != 0);
    }

    /// Get a list of known keys, separated by sep.
    rutz::fstring get_known_keys(const char* sep) const
    {
      return m_map.get_known_keys(sep);
    }

    /// Returns a new object of a given type.
    /** If the given type has not been registered with the factory, a
        null pointer is returned. */
    base_t new_object(const rutz::fstring& type) const
    {
      rutz::creator_base<base_t>* creator = this->find_creator(type);

      if (creator == 0) return base_t();

      return creator->create();
    }

    /// Returns a new object of a given type.
    /** If the given type has not been registered with the factory, an
        exception is thrown. */
    base_t new_checked_object(const rutz::fstring& type) const
    {
      rutz::creator_base<base_t>* creator = this->find_creator(type);

      if (creator == 0) m_map.throw_for_key(type, SRC_POS);

      return creator->create();
    }
  };

} // end namespace rutz


static const char vcid_groovx_rutz_factory_h_utc20050626084020[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_FACTORY_H_UTC20050626084020_DEFINED
