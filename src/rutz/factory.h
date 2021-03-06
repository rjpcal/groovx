/** @file rutz/factory.h template class for object factories, which
    create objects belonging to some inheritance hierarchy given just
    a type name as a string */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Sat Jun 26 23:40:55 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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
#include "rutz/mutex.h"

#include <functional>
#include <memory>
#include <type_traits>
#include <typeinfo>

namespace rutz
{
  class fstring;

  /// Abstract interface for creating objects of a particular type.
  /** rutz::creator_base is a template class that defines a single
      abstract function, create(), that returns an object of type
      creator_base::base_t. */
  template <class T>
  class creator_base
  {
  public:
    typedef T base_t;

    /// Default constructor
    creator_base() = default;

    /// Default copy constructor
    creator_base(const creator_base&) = default;

    /// Default copy assignment operator
    creator_base& operator=(const creator_base&) = default;

    /// Virtual destructor.
    virtual ~creator_base() {}

    /// Return a clone of this Creator
    virtual creator_base* clone() const = 0;

    /// Return a new pointer (or smart pointer) to type \c Base.
    virtual base_t create() = 0;
  };


  /// Implements rutz::creator_base by calling a function pointer.
  /** rutz::creator_from_func implements the rutz::creator_base
      interface by storing a pointer to function that returns an
      object of the appropriate type. */
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
    virtual rutz::creator_base<base_t>* clone() const override
    { return new creator_from_func<base_t, derived_t>(*this); }

    /// Create an object using the creator function.
    virtual base_t create() override { return base_t(m_creator_func()); }

  private:
    creator_func_t m_creator_func;
  };


  /// Non-template helper class for rutz::factory.
  class factory_base
  {
  public:
    typedef void (fallback_t)(const rutz::fstring&);

    /// Default constructor sets the fallback to null.
    factory_base() noexcept;

    /// Virtual no-throw destructor for proper inheritance.
    virtual ~factory_base() noexcept;

    /// Change the fallback function.
    void set_fallback(fallback_t* fptr);

    /// Try running the fallback function for the given key.
    void try_fallback(const rutz::fstring& key) const;

  private:
    std::function<void(const rutz::fstring&)> m_fallback;
  };


  /// Create objects base on 'key' strings.
  /** rutz::factory can create objects of different types associated
      with different 'key' strings. rutz::factory maintains a mapping
      from key strings to rutz::creator_base's, and so given a key can
      call the create() function of the associated creator_base. All
      of the product types of a factory must be derived from
      factory::base_t. The recommended usage is for factories for
      specific types to be implemented as singleton classes derived
      from rutz::factory.

      rutz::factory uses an internal mutex so that all of its member
      functions can safely be called in a multithreaded program
      without external locking.
  */
  template <class T>
  class factory
  {
  public:
    // typedefs

    typedef factory_base::fallback_t fallback_t;
    typedef T base_t;

  private:
    rutz::factory_base                               m_base;
    rutz::assoc_array<rutz::creator_base<base_t> >   m_map;
    mutable std::mutex                               m_mutex;

  protected:
    /// Find a creator for the given key; otherwise return null.
    rutz::creator_base<base_t>*
    find_creator(const rutz::fstring& key) const
    {
      rutz::creator_base<base_t>* creator = 0;

      {
        GVX_MUTEX_LOCK(m_mutex);
        creator = m_map.get_ptr_for_key(key);
      }

      if (creator == nullptr)
        {
          // note that we must call try_fallback() with our mutex
          // UN-locked because the intention is that the fallback
          // function will try do something to cause a new entry to be
          // inserted into this very factory object; that insertion
          // will require locking the mutex so if we have the mutex
          // locked here we will end up with a deadlock:
          m_base.try_fallback(key);

          {
            GVX_MUTEX_LOCK(m_mutex);
            creator = m_map.get_ptr_for_key(key);
          }
        }

      return creator;
    }

  public:
    /// Default constructor.
    /** @param keydescr a human-readable description of what this
        factory's keys represent; this is used in error messages,
        e.g. if descr is "frobnicator", then error messages would
        include "unknown frobnicator"

        @param nocase true if the factory should use case-insensitive
        string comparisons (default is false, giving normal
        case-sensitive string comparisons)
    */
    factory(const char* keydescr = "object type", bool nocase = false)
      : m_base(), m_map(keydescr, nocase), m_mutex()
    {}

    /// Virtual no-throw destructor.
    virtual ~factory() noexcept
    {}

    /// Registers a creation object with the factory.
    /** The function returns the actual key that was paired with the
        creation function.*/
    const char* register_creator(rutz::creator_base<base_t>* creator,
                                 const char* name)
    {
      GVX_MUTEX_LOCK(m_mutex);

      m_map.set_ptr_for_key(name, creator);

      return name;
    }

    /// Registers a creation function with the factory.
    /** The default key associated with the creation function will be
        given by the type's actual C++ name. The function returns the
        actual key that was paired with the creation function.*/
    template <class derived_t>
    const char* register_creator(derived_t (*func) (),
                                 const char* key = nullptr)
    {
      GVX_MUTEX_LOCK(m_mutex);

      if (key == nullptr)
        key = rutz::demangled_name
          (typeid(std::remove_reference_t<decltype(*(func()))>));

      m_map.set_ptr_for_key
        (key, new rutz::creator_from_func<base_t, derived_t>(func));

      return key;
    }

    /// Introduces an alternate key for an existing key.
    /** There must already have been a creation function registered
        for the original key. */
    void register_alias(const char* orig_key, const char* alias_key)
    {
      GVX_MUTEX_LOCK(m_mutex);

      rutz::creator_base<base_t>* creator =
        m_map.get_ptr_for_key(orig_key);

      if (creator != nullptr)
        {
          m_map.set_ptr_for_key(alias_key, creator->clone());
        }
    }

    /// Query whether a given key is a valid, known key in the factory.
    bool is_valid_key(const char* key) const
    {
      return (this->find_creator(key) != nullptr);
    }

    /// Get a list of known keys, separated by sep.
    rutz::fstring get_known_keys(const char* sep) const
    {
      GVX_MUTEX_LOCK(m_mutex);

      return m_map.get_known_keys(sep);
    }

    /// Returns a new object of a given type.
    /** If the given type has not been registered with the factory, a
        null pointer is returned. */
    base_t new_object(const rutz::fstring& type) const
    {
      rutz::creator_base<base_t>* creator = this->find_creator(type);

      if (creator == nullptr) return base_t();

      return creator->create();
    }

    /// Returns a new object of a given type.
    /** If the given type has not been registered with the factory, an
        exception is thrown. */
    base_t new_checked_object(const rutz::fstring& type) const
    {
      rutz::creator_base<base_t>* creator = this->find_creator(type);

      if (creator == nullptr)
        {
          GVX_MUTEX_LOCK(m_mutex);

          m_map.throw_for_key(type, SRC_POS);
        }

      return creator->create();
    }

    /// Change the fallback function.
    void set_fallback(fallback_t* fptr)
    {
      GVX_MUTEX_LOCK(m_mutex);

      m_base.set_fallback(fptr);
    }
  };

} // end namespace rutz


#endif // !GROOVX_RUTZ_FACTORY_H_UTC20050626084020_DEFINED
