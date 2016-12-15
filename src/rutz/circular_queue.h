/** @file rutz/circular_queue.h */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Oct  5 10:43:45 2006
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

#ifndef GROOVX_RUTZ_CIRCULAR_QUEUE_H_UTC20061005174345_DEFINED
#define GROOVX_RUTZ_CIRCULAR_QUEUE_H_UTC20061005174345_DEFINED

#include <atomic>
#include <cstddef> // for size_t

namespace rutz
{
  /// Circular fixed-size queue; T must have a default constructor
  /** This class is designed to be used in a situation where one
      thread is filling the queue while another thread is emptying the
      queue; in that situation, no locks should be needed to access
      the queue safely. */
  template <class T>
  class circular_queue
  {
  public:
    circular_queue(const size_t n)
      :
      m_q(new entry[n == 0 ? 1 : n]),
      m_q_size(n == 0 ? 1 : n),
      m_front(nullptr),
      m_back(nullptr)
    {}

    ~circular_queue()
    {
      delete [] m_q;
    }

    /// Get the number of spaces in the queue (not all of which may be currently occupied)
    size_t size() const { return m_q_size; }

    /// Returns true of the pop succeeded
    bool pop_front(T& dest)
    {
      if (m_q[m_front].valid.load() == false)
        // the queue is empty right now:
        return false;

      dest = m_q[m_front].value;

      m_q[m_front].value = T();
      m_q[m_front].valid.store(false);
      if (m_front+1 == m_q_size)
        m_front = 0;
      else
        ++m_front;

      return true;
    }

    /// Returns true if the push succeeded
    bool push_back(const T& val)
    {
      if (m_q[m_back].valid.load() == true)
        // we don't have any space in the queue for another entry
        // right now:
        return false;

      m_q[m_back].value = val;
      m_q[m_back].valid.store(true);
      if (m_back+1 == m_q_size)
        m_back = 0;
      else
        ++m_back;

      return true;
    }

  private:
    circular_queue(const circular_queue&);
    circular_queue& operator=(const circular_queue&);

    struct entry
    {
      entry() : value(), valid(false) {}

      T value;
      std::atomic<bool> valid;
    };

    entry* const m_q;
    size_t m_q_size;
    size_t m_front;
    size_t m_back;
  };
}

#endif // !GROOVX_RUTZ_CIRCULAR_QUEUE_H_UTC20061005174345DEFINED
