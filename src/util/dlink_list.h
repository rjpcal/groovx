///////////////////////////////////////////////////////////////////////
//
// dlink_list.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed May 31 14:24:31 2000
// written: Wed May 31 14:24:43 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DLINK_LIST_H_DEFINED
#define DLINK_LIST_H_DEFINED


///////////////////////////////////////////////////////////////////////
/**
 *
 * A doubly-linked list.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class dlink_list {
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

  struct node {
	 node(const T& v, node* p, node* n) : val(v), prev(p), next(n) {}

	 node(const node& other) :
		val(other.val), prev(other.prev), next(other.next) {}

	 node& operator=(const node& other)
	   { val = other.val; prev = other.prev; next = other.next; return *this; }

	 T val;
	 node* prev;
	 node* next;
  };

  class iterator {
  private:
	 node* nn;

  public:
	 friend class dlink_list;
	 friend class const_iterator;

	 typedef T value_type;
	 typedef T* pointer;
	 typedef T& reference;

	 iterator(node* n = 0) : nn(n) {}

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

  class const_iterator {
  private:
	 node* nn;

  public:
	 friend class dlink_list;

	 typedef const T value_type;
	 typedef const T* pointer;
	 typedef const T& reference;

	 const_iterator(node* n = 0) : nn(n) {}
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

  //
  // Member functions
  //

  dlink_list() : head(0), tail(0) {}

  dlink_list(const dlink_list& other) :
	 head(0), tail(0)
	 {
		if ( !other.empty() )
		  {
			 head = new node(other.head->val, 0, 0);

			 node* other_next_node = other.head->next;
			 tail = head;
			 while (other_next_node != 0)
				{
				  tail->next = new node(other_next_node->val, tail, 0);

				  other_next_node = other_next_node->next;
				  tail = tail->next;
				}
		  }
	 }

  ~dlink_list() { clear(); }

  dlink_list& operator=(const dlink_list& other)
	 {
		dlink_list other_copy(other);
		this->swap(other_copy);
		return *this;
	 }

  void push_front(const T& val)
	 {
		// old_head->next is unchanged
		// old_head->prev becomes head
		// head->next becomes old_head
		// head->prev becomes 0
		node* old_head = head;
		head = new node(val, 0, old_head);
		if (old_head != 0) old_head->prev = head;

		if (tail == 0) tail = head;
	 }

  void pop_front()
	 {
		// new_head->next is unchanged
		// new_head->prev becomes 0
		node* new_head = head->next;
		delete head;
		if (new_head != 0) new_head->prev = 0;
		head = new_head;

		if (head == 0) tail = 0;
	 }

  void insert_after(iterator pos, const T& val)
	 {
		node* before = pos.nn;
		node* after = before->next;

		node* current = new node(val, before, after);
		before->next = current;

		if (after != 0) after->prev = current;

		if (tail == before) tail = current;
	 }

  void push_back(const T& val)
	 {
		insert_after(iterator(tail), val);
	 }

  reference back() { return tail->val; }
  const_reference back() const { return tail->val; }

  reference front() { return head->val; }
  const_reference front() const { return head->val; }

  iterator begin()             { return iterator(head); }
  iterator end()               { return iterator(0); }
  const_iterator begin() const { return const_iterator(head); }
  const_iterator end()   const { return const_iterator(0); }

  iterator rbegin()             { return iterator(tail); }
  iterator rend()               { return iterator(0); }
  const_iterator rbegin() const { return const_iterator(tail); }
  const_iterator rend()   const { return const_iterator(0); }

  bool empty() const { return (head == 0); }

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

  void clear()
	 {
		while ( !empty() ) pop_front();
	 }

  void swap(dlink_list& other)
	 {
		node* other_head = other.head;
		other.head = this->head;
		this->head = other_head;

		node* other_tail = other.tail;
		other.tail = this->tail;
		this->tail = other_tail;
	 }

private:
  node* head;
  node* tail;
};

static const char vcid_dlink_list_h[] = "$Header$";
#endif // !DLINK_LIST_H_DEFINED
