///////////////////////////////////////////////////////////////////////
//
// dlink_list.h
//
// Copyright (c) 2000-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed May 31 14:24:31 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef DLINK_LIST_H_DEFINED
#define DLINK_LIST_H_DEFINED

#include "util/algo.h"

#include <new>

///////////////////////////////////////////////////////////////////////
/**
 *
 * A doubly-linked list.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class dlink_list
{
public:

  //
  // Typedefs
  //
  typedef T value_type;                   ///< STL value type

  typedef unsigned int size_type;         ///< STL size type

  typedef T* pointer;                     ///< STL pointer type
  typedef T& reference;                   ///< STL reference type
  typedef const T* const_pointer;         ///< STL const-pointer type
  typedef const T& const_reference;       ///< STL const-reference type

  class iterator;                         ///< STL iterator type
  class const_iterator;                   ///< STL const-iterator type

  /// Doubly-linked node class.
  class node
  {
  private:
    ~node(); // not defined
    node(const node& other); // not defined
    node& operator=(const node& other); // not defined

    friend class dlink_list;
    friend class dlink_list::iterator;
    friend class dlink_list::const_iterator;

    /// Construct our value using placement new copy construction.
    void construct(const T& new_val) { new (&val) T(new_val); }

    /// Destruct our value.
    void destroy() { (&val)->~T(); }

    /// Create a new node.
    static node* allocate()
      { return static_cast<node*>(::operator new(sizeof(node))); }

    /// Create a new node and set up its prev/next links.
    static node* allocate(node* p, node* n)
      { node* x = allocate(); x->prev = p; x->next = n; return x; }

    /// Create a new node with prev/next links and an initial element value.
    static node* allocate(const T& val, node* p, node* n)
      {
        node* x = allocate(p,n);
        x->construct(val);
        return x;
      }

    /// Destroy a node (its element must have been destroyed first).
    static void deallocate(node* n)
      { ::operator delete(static_cast<void*>(n)); }

    T val;       ///< Element value.
    node* prev;  ///< Backward link.
    node* next;  ///< Forward link.
  };

  /// Doubly-linked list iterator.
  class iterator
  {
  private:
    node* nn;

    friend class dlink_list;
    friend class dlink_list::const_iterator;

    /// Raw constructor only available to dlink_list itself.
    iterator(node* n) : nn(n) {}

  public:
    typedef T value_type;  ///< STL value type
    typedef T* pointer;    ///< STL pointer type
    typedef T& reference;  ///< STL reference type

    /// Copy constructor.
    iterator(const iterator& other) : nn(other.nn) {}

    /// Assignment operator.
    iterator& operator=(const iterator& other)
      { nn = other.nn; return *this; }

    /// Dereferencing operator.
    reference operator*() { return nn->val; }
    /// Dereferencing operator.
    pointer operator->() { return &(nn->val); }

    /// Pre-increment operator.
    iterator& operator++() { nn = nn->next; return *this; }
    /// Post-increment operator.
    iterator operator++(int) { iterator temp(*this); ++*this; return temp; }

    /// Pre-decrement operator.
    iterator& operator--() { nn = nn->prev; return *this; }
    /// Post-decrement operator.
    iterator operator--(int) { iterator temp(*this); ++*this; return temp; }

    /// Equality operator.
    bool operator==(const iterator& other) const { return nn == other.nn; }
    /// Inequality operator.
    bool operator!=(const iterator& other) const { return nn != other.nn; }
  };

  /// Doubly-linked list const iterator.
  class const_iterator
  {
  private:
    node* nn;

  public:
    friend class dlink_list;

    typedef const T value_type;  ///< STL value type
    typedef const T* pointer;    ///< STL pointer type
    typedef const T& reference;  ///< STL reference type

  private:
    /// Raw constructor only available to dlink_list itself.
    const_iterator(node* n) : nn(n) {}

  public:
    /// Conversion constructor from non-const iterator.
    const_iterator(const iterator& other) : nn (other.nn) {}

    /// Copy constructor.
    const_iterator(const const_iterator& other) : nn(other.nn) {}

    /// Assignment operator.
    const_iterator& operator=(const const_iterator& other)
      { nn = other.nn; return *this; }

    /// Dereferencing operator.
    const_reference operator*() { return nn->val; }
    /// Dereferencing operator.
    const_pointer operator->() { return &(nn->val); }

    /// Pre-increment operator.
    const_iterator& operator++() { nn = nn->next; return *this; }
    /// Post-increment operator.
    const_iterator operator++(int)
      { const_iterator temp(*this); ++*this; return temp; }

    /// Pre-decrement operator.
    const_iterator& operator--() { nn = nn->prev; return *this; }
    /// Post-decrement operator.
    const_iterator operator--(int)
      { const_iterator temp(*this); ++*this; return temp; }

    /// Equality operator.
    bool operator==(const const_iterator& other) { return nn == other.nn; }
    /// Inequality operator.
    bool operator!=(const const_iterator& other) { return nn != other.nn; }
  };

private:
  node* the_node;

  void init()
    {
#ifdef LOCAL_TRACE
    DOTRACE("dlink_list::init");
#endif
      the_node = node::allocate();
      the_node->prev = the_node->next = the_node;
    }

  //
  // Member functions
  //

public:
  /// Construct an empty list.
  dlink_list() : the_node(0) { init(); }

  /// Copy constructor.
  dlink_list(const dlink_list& other) : the_node(0)
    {
      init();

      insert_range(begin(), other.begin(), other.end());
    }

  /// Destructor
  ~dlink_list() { clear(); node::deallocate(the_node); }

  /// Assignment operator.
  dlink_list& operator=(const dlink_list& other)
    {
      if (this != &other)
        {
          dlink_list other_copy(other);
          this->swap(other_copy);
        }
      return *this;
    }

  /// Push a new value onto the front of the list.
  void push_front(const T& val)   { insert(begin(), val); }
  /// Push a new value onto the back of the list.
  void push_back(const T& val)    { insert(end(), val); }

  /// Pop the front element off of the list.
  void pop_front()                { erase(begin()); }
  /// Pop the back element off of the list.
  void pop_back()                 { iterator tmp = end(); erase(--tmp); }

  /// Insert a range of elements at the given position.
  template <class Itr>
  void insert_range(iterator pos, Itr itr, Itr stop)
    {
      while (itr != stop)
        {   insert(pos, *itr);   ++itr; }
    }

  /// Insert a new link containing \a val at a position just before \a pos.
  void insert(iterator pos, const T& val)
    {
#ifdef LOCAL_TRACE
    DOTRACE("dlink_list::insert");
#endif
      // Before the function:

      //               itr
      //                |
      //        ----->  |
      //  before      after
      //        <-----

      // After the function:

      //                             itr
      //                              |
      //        ----->        ----->  |
      //  before      new_node      after
      //        <-----        <-----

      node* after = pos.nn;
      node* before = after->prev;

      node* new_node = node::allocate(val, before, after);

      before->next = new_node;
      after->prev = new_node;
    }

  /// Erase the link at \a pos.
  void erase(iterator pos)
    {
#ifdef LOCAL_TRACE
    DOTRACE("dlink_list::erase");
#endif
      if (pos == end()) return;

      // Before the function:

      //               itr
      //                |
      //        ----->  |   ----->
      //  before      target      after
      //        <-----      <-----

      // After the function:

      //
      //
      //        ----->
      //  before      after
      //        <-----

      node* target = pos.nn;
      node* before = target->prev;
      node* after = target->next;

      before->next = after;
      after->prev = before;

      target->destroy();
      node::deallocate(target);
    }

  /// Removes the first link (if any) with the value \a val.
  void remove(const T& val)
    {
      iterator ii = find(val);
      erase(ii);
    }

  /// Get a reference to the back element.
  reference back() { return *(--end()); }
  /// Get a const-reference to the back element.
  const_reference back() const { return *(--end()); }

  /// Get a reference to the front element.
  reference front() { return *(begin()); }
  /// Get a const-reference to the front element.
  const_reference front() const { return *(begin()); }

  /// Get a start iterator.
  iterator begin()             { return iterator(the_node->next); }
  /// Get a one-past-the-end iterator.
  iterator end()               { return iterator(the_node); }
  /// Get a start const-iterator.
  const_iterator begin() const { return const_iterator(the_node->next); }
  /// Get a one-past-the-end const-iterator.
  const_iterator end()   const { return const_iterator(the_node); }

  /// Query if the list is empty.
  bool is_empty() const { return begin() == end(); }

  /// Get the number of elements in the list (this is an O(N) operation!).
  size_type size() const
    {
      const_iterator itr(begin()), stop(end());
      size_type result = 0;
      while ( itr != stop )
        {
          ++result;
          ++itr;
        }
      return result;
    }

  /// Get an iterator to the first element whose value matches val.
  /** Returns end() if no matching element is found. */
  iterator find(const T& val)
    {
      iterator itr(begin()), stop(end());
      while (itr != stop)
        {
          if (*itr == val) return itr;
          ++itr;
        }
      return end();
    }

  /// Get a const-iterator to the first element whose value matches val.
  /** Returns end() if no matching element is found. */
  const_iterator find(const T& val) const
    { return const_cast<dlink_list*>(this)->find(val); }

  /// Remove all elements from the list.
  void clear() { while ( !is_empty() ) pop_front(); }

  /// Swap contents with another dlink_list object.
  void swap(dlink_list& other)
    {
      Util::swap2(the_node, other.the_node);
    }
};

static const char vcid_dlink_list_h[] = "$Header$";
#endif // !DLINK_LIST_H_DEFINED
