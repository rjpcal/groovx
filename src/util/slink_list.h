///////////////////////////////////////////////////////////////////////
//
// slink_list.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Mar 18 11:22:40 2000
// written: Wed Sep 25 19:00:12 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SLINK_LIST_H_DEFINED
#define SLINK_LIST_H_DEFINED

#include "util/algo.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * A singly-linked list.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class slink_list
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

  //
  // Nested types: node and iterators
  //

  /// Singly-linked list node.
  struct node
  {
    node(const T& v, node* n) : val(v), next(n) {}

    node(const node& other) : val(other.val), next(other.next) {}

    node& operator=(const node& other)
      { val = other.val; next = other.next; return *this; }

    T val;
    node* next;
  };

  /// Singly-linked list iterator.
  class iterator
  {
  private:
    node* nn;

  public:
    friend class slink_list;
    friend class const_iterator;

    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;

    iterator(node* n = 0) : nn(n) {}

    iterator(const iterator& other) : nn(other.nn) {}

    iterator& operator=(const iterator& other)
      { nn = other.nn; return *this; }

    reference operator*() const { return nn->val; }
    pointer operator->() const { return &(nn->val); }

    iterator& operator++() { nn = nn->next; return *this; }
    iterator operator++(int) { iterator temp(*this); ++*this; return temp; }

    bool operator==(const iterator& other) const { return nn == other.nn; }
    bool operator!=(const iterator& other) const { return nn != other.nn; }
  };

  /// Singly-linked list const iterator.
  class const_iterator
  {
  private:
    node* nn;

  public:
    friend class slink_list;

    typedef const T value_type;
    typedef const T* pointer;
    typedef const T& reference;

    const_iterator(node* n = 0) : nn(n) {}
    const_iterator(const iterator& other) : nn (other.nn) {}

    const_iterator(const const_iterator& other) : nn(other.nn) {}
    const_iterator& operator=(const const_iterator& other)
      { nn = other.nn; return *this; }

    const_reference operator*() const { return nn->val; }
    const_pointer operator->() const { return &(nn->val); }

    const_iterator& operator++() { nn = nn->next; return *this; }
    const_iterator operator++(int)
      { const_iterator temp(*this); ++*this; return temp; }

    bool operator==(const const_iterator& other) const { return nn == other.nn; }
    bool operator!=(const const_iterator& other) const { return nn != other.nn; }
  };

  //
  // Member functions
  //

  slink_list() : head(0) {}

  slink_list(const slink_list& other) :
    head(0)
    {
      if ( !other.is_empty() )
        {
          head = new node(other.head->val, 0);

          node* other_next_node = other.head->next;
          node* tail = head;
          while (other_next_node != 0)
            {
              tail->next = new node(other_next_node->val, 0);

              other_next_node = other_next_node->next;
              tail = tail->next;
            }
        }
    }

  ~slink_list() { clear(); }

  slink_list& operator=(const slink_list& other)
    {
      slink_list other_copy(other);
      this->swap(other_copy);
      return *this;
    }

  void push_front(const T& val)
    {
      node* new_node = new node(val, head);
      head = new_node;
    }

  void pop_front()
    {
      node* new_head = head->next;
      delete head;
      head = new_head;
    }

  void insert_after(iterator pos, const T& val)
    {
      node* before = pos.nn;
      node* after = before->next;

      node* current = new node(val, after);
      before->next = current;
    }

  reference front() { return head->val; }
  const_reference front() const { return head->val; }

  iterator begin()             { return iterator(head); }
  iterator end()               { return iterator(0); }
  const_iterator begin() const { return const_iterator(head); }
  const_iterator end()   const { return const_iterator(0); }

  bool is_empty() const { return (head == 0); }

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
    { return const_cast<slink_list*>(this)->find(val); }

  void clear()
    {
      while ( !is_empty() ) pop_front();
    }

  void swap(slink_list& other)
    {
      Util::swap2(head, other.head);
    }

private:
  node* head;
};

static const char vcid_slink_list_h[] = "$Header$";
#endif // !SLINK_LIST_H_DEFINED
