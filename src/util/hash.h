///////////////////////////////////////////////////////////////////////
//
// hash.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 20 08:50:34 2000
// written: Sun Nov  3 13:41:11 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HASH_H_DEFINED
#define HASH_H_DEFINED

#include "util/algo.h"
#include "util/arrays.h"
#include "util/slink_list.h"

#include <cstddef>

#include "util/debug.h"

namespace
{
  // Note: assumes long is at least 32 bits.
  const int num_primes = 28;
  const unsigned long prime_list[num_primes] =
  {
    53ul,         97ul,         193ul,       389ul,       769ul,
    1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
    49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
    1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
    50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
    1610612741ul, 3221225473ul, 4294967291ul
  };

  inline unsigned long least_prime_greater_than(unsigned long hint)
    {
      int n = 0;
      while (n < (num_primes-1))
        {
          if (prime_list[n] > hint) break;
          ++n;
        }
      return prime_list[n];
    }
}

/// Computes hash values for generic key types.
template <class key_type>
struct default_hasher
{
public:
  size_t operator()(const key_type& key);
};

template <class key_type>
size_t default_hasher<key_type>::operator()(const key_type& key)
{
  size_t res = 0;
  size_t len = sizeof(key_type);
  const char* p = reinterpret_cast<const char*>(&key);

  while (len --) res = (res<<1)^*p++;
  return res;
}

template <>
size_t default_hasher<const char*>::operator()(const char* const& key)
{
  const char* ptr = key;
  size_t res = 0;
  while (*ptr) res = (res<<1)^*ptr++;
  return res;
};

/// Computes hash values for string types.
template <class string_type>
struct string_hasher
{
public:
  size_t operator()(const string_type& key)
    {
      return default_hasher<const char*>()(key.c_str());
    }
};

/// Hash-array class.
template <class key_type, class value_type,
  class hash_func = default_hasher<key_type> >
class hash_array
{

  //
  // nested types and typedefs
  //

public:
  /// Key/value entry in a hash-array.
  struct entry_type
  {
    entry_type(const key_type& k, const value_type& v) :
      key(k), value(v) {}

    entry_type(const entry_type& other) :
      key(other.key), value(other.value) {}

    entry_type& operator=(const entry_type& other)
      { key = other.key; value = other.value; return *this; }

    key_type key;
    value_type value;
  };

private:
  typedef slink_list<entry_type> list_type;
  typedef dynamic_block<list_type> table_type;

  typedef typename list_type::iterator list_iterator;
  typedef typename table_type::iterator table_iterator;

public:
  typedef typename table_type::size_type size_type;

  class iterator;
  friend class iterator;

  /// Hash-array iterator.
  class iterator
  {
  private:
    friend class hash_array;

    table_type* buckets_ptr;
    table_iterator table_itr;
    list_iterator list_itr;
    bool at_end;

    enum position { BEGIN, END };

    // XXX This requires the table to always contain at least one list
    iterator(table_type* tp, position pos = BEGIN) :
      buckets_ptr(tp),
      table_itr( (pos == BEGIN) ? tp->begin() : tp->end()-1),
      list_itr( (pos == BEGIN) ? table_itr->begin() : table_itr->end() ),
      at_end( pos == END )
      {
        if (pos == BEGIN)
          {
            start_next_nonempty_bucket();
          }
      }

    iterator(table_type* tp, table_iterator ti, list_iterator li) :
      buckets_ptr(tp), table_itr(ti), list_itr(li), at_end(false) {}

    void start_next_nonempty_bucket()
      {
        while (table_itr->is_empty() && table_itr < (buckets_ptr->end()-1))
          ++table_itr;
        if (table_itr->is_empty())
          at_end = true;
        list_itr = table_itr->begin();
      }

  public:
    typedef entry_type* pointer;
    typedef entry_type& reference;

    iterator(const iterator& other) :
      buckets_ptr(other.buckets_ptr),
      table_itr(other.table_itr),
      list_itr(other.list_itr),
      at_end(other.at_end)
      {}

    iterator& operator=(const iterator& other)
    {
      buckets_ptr = other.buckets_ptr;
      table_itr = other.table_itr;
      list_itr = other.list_itr;
      at_end = other.at_end;
      return *this;
    }

    reference operator*() { return list_itr.operator*(); }
    pointer operator->()
    {
      dbgEval(3, table_itr - buckets_ptr->begin());
      dbgEvalNL(3, &*list_itr);

      return list_itr.operator->();
    }

    iterator& operator++()
      {
        if (!at_end)
          {
            if (++list_itr == table_itr->end())
              {
                if (table_itr >= (buckets_ptr->end()-1))
                  at_end = true;
                else
                  {
                    ++table_itr;
                    start_next_nonempty_bucket();
                  }
              }
          }
        return *this;
      }

    iterator operator++(int)
      { iterator tmp(*this); ++*this; return tmp; }

    bool operator==(const iterator& other) const
      { return ( (at_end == other.at_end) &&
                 (table_itr == other.table_itr) &&
                 (list_itr == other.list_itr) ); }
    bool operator!=(const iterator& other) const
      { return ( (at_end != other.at_end) ||
                 (table_itr != other.table_itr) ||
                 (list_itr != other.list_itr) ); }
  };

  //
  // data members
  //
private:
  table_type buckets;
  hash_func hasher;
  size_type entry_count;
  double max_load;
  double grow_factor;

  //
  // public member functions
  //

public:
  hash_array() :
    buckets(prime_list[0]),
    hasher(hash_func()),
    entry_count(0),
    max_load(0.7),
    grow_factor(1.6)
    {}
  hash_array(size_type size_hint) :
    buckets(least_prime_greater_than(size_hint)),
    hasher(hash_func()),
    entry_count(0),
    max_load(0.7),
    grow_factor(1.6)
    {}
  hash_array(const hash_array& other) :
    buckets(other.buckets),
    hasher(other.hasher),
    entry_count(other.entry_count),
    max_load(other.max_load),
    grow_factor(other.grow_factor) {}

  hash_array& operator=(const hash_array& other)
    {
      hash_array other_copy(other);
      this->swap(other_copy);
      return *this;
    }

  iterator begin() { return iterator(&buckets); }
  iterator end() { return iterator(&buckets, iterator::END); }

  size_type size() const { return entry_count; }
  bool is_empty() const { return entry_count == 0; }
  size_type bucket_count() const { return buckets.size(); }
  double load_factor() const { return double(size()) / bucket_count(); }
  size_type longest_chain() const
    {
      size_type res = 0;
      for (table_iterator itr = buckets.begin(), stop = buckets.end();
           itr != stop;
           ++itr)
        {
          size_type c = itr->size();
          if (res < c) res = c;
        }
      return res;
    }

  void set_max_load(double load) const { max_load = load; }

  iterator insert(const entry_type& entry, bool allow_resize = true)
    {
      size_t h = hasher(entry.key) % buckets.size();

      dbgEvalNL(3, h);

      table_iterator bucket_itr = buckets.begin()+h;
      list_iterator itr(bucket_itr->begin()), stop(bucket_itr->end());

      // See if an entry with the given key already exists...
      bool created_new_entry = false;
      for ( ; itr != stop; ++itr)
        {
          if (itr->key == entry.key) break;
        }

      // ... if not, then add a new entry
      if ( itr == stop )
        {
          if ( allow_resize &&
               entry_count+1 > max_load*buckets.size() )
            {
              resize(size_type(entry_count*grow_factor)); // resize...
              h = hasher(entry.key) % buckets.size();
              bucket_itr = buckets.begin()+h;
            }

          bucket_itr->push_front(entry);
          itr = bucket_itr->begin();
          ++entry_count;
          created_new_entry = true;
        }

      // ... otherwise, do nothing (just keep the existing entry)
      else { ; }

      return iterator(&buckets, bucket_itr, itr);
    }

  iterator insert(const key_type& key, const value_type& value)
    { return insert(entry_type(key, value)); }

  value_type& operator[](const key_type& key)
    {
      dbgEvalNL(3, key);

      return insert(key, value_type())->value;
    }

  iterator find(const key_type& key)
    {
      size_t h = hasher(key) % buckets.size();

      table_iterator bucket_itr = buckets.begin()+h;
      list_iterator itr(bucket_itr->begin()), stop(bucket_itr->end());

      for ( ; itr != stop; ++itr)
        {
          if (itr->key == key) break;
        }

      return iterator(&buckets, bucket_itr, itr);
    }

  void swap(hash_array& other)
    {
      buckets.swap(other.buckets);
      Util::swap2(entry_count, other.entry_count);
    }

  void resize(size_type new_size_hint)
    {
      hash_array new_array(new_size_hint);
      for (iterator itr = begin(), stop = end(); itr != stop; ++itr)
        {
          new_array.insert(*itr, false);
        }
      this->swap(new_array);
    }

  void clear()
    {
      for (table_iterator itr = buckets.begin(), stop = buckets.end();
           itr != stop;
           ++itr)
        {
          /* size_type sz = itr->size(); */
          itr->clear();
          /* entry_count -= sz; */
        }
      entry_count = 0;
    }
};


static const char vcid_hash_h[] = "$Header$";
#endif // !HASH_H_DEFINED
