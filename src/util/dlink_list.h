///////////////////////////////////////////////////////////////////////
//
// dlink_list.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed May 31 14:24:31 2000
// written: Wed Mar 19 17:58:55 2003
// $Id$
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
  typedef T value_type;

  typedef unsigned int size_type;

  typedef T* pointer;
  typedef T& reference;
  typedef const T* const_pointer;
  typedef const T& const_reference;

  class iterator;
  class const_iterator;

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

    void construct(const T& new_val) { new (&val) T(new_val); }
    void destroy() { (&val)->~T(); }

    static node* allocate()
      { return static_cast<node*>(::operator new(sizeof(node))); }

    static node* allocate(node* p, node* n)
      { node* x = allocate(); x->prev = p; x->next = n; return x; }

    static node* allocate(const T& val, node* p, node* n)
      {
        node* x = allocate(p,n);
        x->construct(val);
        return x;
      }

    static void deallocate(node* n)
      { ::operator delete(static_cast<void*>(n)); }

    T val;
    node* prev;
    node* next;
  };

  /// Doubly-linked list iterator.
  class iterator
  {
  private:
    node* nn;

  public:
    friend class dlink_list;
    friend class const_iterator;

    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;

  private:
    iterator(node* n) : nn(n) {}

  public:
    iterator(const iterator& other) : nn(other.nn) {}

    iterator& operator=(const iterator& other)
      { nn = other.nn; return *this; }

    reference operator*() { return nn->val; }
    pointer operator->() { return &(nn->val); }

    iterator& operator++() { nn = nn->next; return *this; }
    iterator operator++(int) { iterator temp(*this); ++*this; return temp; }

    iterator& operator--() { nn = nn->prev; return *this; }
    iterator operator--(int) { iterator temp(*this); ++*this; return temp; }

    bool operator==(const iterator& other) const { return nn == other.nn; }
    bool operator!=(const iterator& other) const { return nn != other.nn; }
  };

  /// Doubly-linked list const iterator.
  class const_iterator
  {
  private:
    node* nn;

  public:
    friend class dlink_list;

    typedef const T value_type;
    typedef const T* pointer;
    typedef const T& reference;

  private:
    const_iterator(node* n) : nn(n) {}

  public:
    const_iterator(const iterator& other) : nn (other.nn) {}

    const_iterator(const const_iterator& other) : nn(other.nn) {}
    const_iterator& operator=(const const_iterator& other)
      { nn = other.nn; return *this; }

    const_reference operator*() { return nn->val; }
    const_pointer operator->() { return &(nn->val); }

    const_iterator& operator++() { nn = nn->next; return *this; }
    const_iterator operator++(int)
      { const_iterator temp(*this); ++*this; return temp; }

    const_iterator& operator--() { nn = nn->prev; return *this; }
    const_iterator operator--(int)
      { const_iterator temp(*this); ++*this; return temp; }

    bool operator==(const const_iterator& other) { return nn == other.nn; }
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
  dlink_list() : the_node(0) { init(); }

  dlink_list(const dlink_list& other) : the_node(0)
    {
      init();

      insert_range(begin(), other.begin(), other.end());
    }

  ~dlink_list() { clear(); node::deallocate(the_node); }

  dlink_list& operator=(const dlink_list& other)
    {
      if (this != &other)
        {
          dlink_list other_copy(other);
          this->swap(other_copy);
        }
      return *this;
    }

  void push_front(const T& val)   { insert(begin(), val); }
  void push_back(const T& val)    { insert(end(), val); }

  void pop_front()                { erase(begin()); }
  void pop_back()                 { iterator tmp = end(); erase(--tmp); }

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

  reference back() { return *(--end()); }
  const_reference back() const { return *(--end()); }

  reference front() { return *(begin()); }
  const_reference front() const { return *(begin()); }

  iterator begin()             { return iterator(the_node->next); }
  iterator end()               { return iterator(the_node); }
  const_iterator begin() const { return const_iterator(the_node->next); }
  const_iterator end()   const { return const_iterator(the_node); }

  bool is_empty() const { return begin() == end(); }

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

  const_iterator find(const T& val) const
    { return const_cast<dlink_list*>(this)->find(val); }

  void clear() { while ( !is_empty() ) pop_front(); }

  void swap(dlink_list& other)
    {
      Util::swap2(the_node, other.the_node);
    }
};

static const char vcid_dlink_list_h[] = "$Header$";
#endif // !DLINK_LIST_H_DEFINED
