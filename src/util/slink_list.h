///////////////////////////////////////////////////////////////////////
//
// lists.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Mar 18 11:22:40 2000
// written: Mon Mar 20 08:19:57 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LISTS_H_DEFINED
#define LISTS_H_DEFINED

template <class T>
class slink_list {
public:
  typedef T value_type;

  typedef unsigned int size_type;

  typedef T* pointer;
  typedef T& reference;
  typedef const T* const_pointer;
  typedef const T& const_reference;

  struct node {
	 node(const T& v=T(), node* n=0) : val(v), next(n) {}

	 T val;
	 node* next;
  };

  class iterator {
  private:
	 node* nn;
  public:
	 iterator(node* n = 0) : nn(n) {}

	 typedef T value_type;
	 typedef T* pointer;
	 typedef T& reference;

	 reference operator*() { return nn->val; }
	 pointer operator->() { return &(nn->val); }

	 iterator& operator++() { nn = nn->next; return *this; }
	 iterator operator++(int) { iterator temp(*this); ++*this; return temp; }

	 bool operator==(const iterator& other) { return nn == other.nn; }
	 bool operator!=(const iterator& other) { return nn != other.nn; }
  };

  slink_list() : head(0) {}

  slink_list(const slink_list& other) :
	 head(new node(other.head->val, 0))
	 {
		node* other_next_node = other.head->next;
		node* this_node = head;
		while (other_next_node != 0)
		  {
			 this_node->next = new node(other_next_node->val, 0);
			 other_next_node = other_next_node->next;
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

  reference front() { return head->val; }
  const_reference front() const { return head->val; }

  iterator begin() { return iterator(head); }
  iterator end() { return iterator(0); }

  bool empty() const { return (head == 0); }

  size_type size() const
	 {
		node* n = head;
		size_type result = 0;
		while ( n )
		  {
			 ++result;
			 n = n->next;
		  }
	 }


  void clear()
	 {
		while ( !empty() ) pop_front();
	 }

  void swap(slink_list& other)
	 {
		node* other_head = other.head;
		other.head = this->head;
		this->head = other_head;
	 }

private:
  node* head;
};

static const char vcid_lists_h[] = "$Header$";
#endif // !LISTS_H_DEFINED
