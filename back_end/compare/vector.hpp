#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.h"
#include <iostream>

#include <cstdio>
#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector {
private:
	T * data;
	size_t theSize;
	size_t theCapacity;
	size_t bound;
	void double_space(){
		theCapacity *= 2;
		data = (T *) realloc(data, theCapacity * sizeof(T));
	}
public:

	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator {
	private:

	public:
	    vector * v;
		size_t ptr;
		iterator(vector * v, size_t ptr) : v(v), ptr(ptr){};
		iterator(const iterator& other) : v(other.v), ptr(other.ptr){};
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {
			return iterator(v, ptr + n);
		}
		iterator operator-(const int &n) const {
			return iterator(v, ptr - n);
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invalid_iterator.
		int operator-(const iterator &rhs) const {
			if (v != rhs.v) throw(invalid_iterator());
			return rhs.ptr - ptr;
		}
		iterator& operator+=(const int &n) {
			ptr += n;
			return (*this);
		}
		iterator& operator-=(const int &n) {
			ptr -= n;
			return (*this);
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator old = (*this);
			ptr++;
			return old;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			++ptr;
			return (*this);
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator old = (*this);
			ptr--;
			return old;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			--ptr;
			return (*this);
		}
		/**
		 * TODO *it
		 */
        T& operator*() const{
			return (v -> at(ptr));
		}

		bool operator==(const iterator &rhs) const {
			return v == rhs.v && ptr  == rhs.ptr ;
		}
		bool operator==(const const_iterator &rhs) const {
			return v == rhs.v && ptr  == rhs.ptr ;
		}

		bool operator!=(const iterator &rhs) const {
			return v != rhs.v || ptr  != rhs.ptr ;
		}
		bool operator!=(const const_iterator &rhs) const {
			return v != rhs.v || ptr  != rhs.ptr ;
		}
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator {
	private:
		const vector * v;
		size_t ptr;
	public:
		const_iterator(const vector * v, size_t ptr) : v(v), ptr(ptr){};
		const_iterator(const iterator& other) : v(other.v), ptr(other.ptr){};
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		const_iterator operator+(const int &n) const {
			return const_iterator(v, ptr + n);
		}
		const_iterator operator-(const int &n) const {
			return const_iterator(v, ptr - n);
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const const_iterator &rhs) const {
			if (v != rhs.v) throw(invalid_iterator());
			return rhs.ptr - ptr;
		}
		const_iterator& operator+=(const int &n) {
			ptr += n;
			return (*this);
		}
		const_iterator& operator-=(const int &n) {
			ptr -= n;
			return (*this);
		}
		/**
		 * TODO iter++
		 */
		const_iterator operator++(int) {
			const_iterator old = (*this);
			ptr++;
			return old;
		}
		/**
		 * TODO ++iter
		 */
		const_iterator& operator++() {
			++ptr;
			return (*this);
		}
		/**
		 * TODO iter--
		 */
		const_iterator operator--(int) {
			const_iterator old = (*this);
			ptr--;
			return old;
		}
		/**
		 * TODO --iter
		 */
		const_iterator& operator--() {
			--ptr;
			return (*this);
		}
		/**
		 * TODO *it
		 */
		const T& operator*() const{
			return (v -> at(ptr));
		}

		bool operator==(const iterator &rhs) const {
			return v == rhs.v && ptr  == rhs.ptr ;
		}
		bool operator==(const const_iterator &rhs) const {
			return v == rhs.v && ptr  == rhs.ptr ;
		}

		bool operator!=(const iterator &rhs) const {
			return v != rhs.v || ptr  != rhs.ptr ;
		}
		bool operator!=(const const_iterator &rhs) const {
			return v != rhs.v || ptr  != rhs.ptr ;
		}
	};
	/**
	 * TODO Constructs
	 * Atleast three: default constructor, copy constructor and a constructor for std::vector
	 */
	vector(size_t init_size = 10) : theSize(0),theCapacity(init_size), bound(0){
		data = (T *)malloc(sizeof(T) * theCapacity);
	}
	vector(const vector &other) : theSize(0),theCapacity(other.theCapacity), bound(0){
		data = (T *)malloc(sizeof(T) * theCapacity);
		int i;
		for (i = 0 ; i < other.theSize ; i++) push_back(other[i]);
	}
	/**
	 * TODO Destructor
	 */
	~vector() {
	    int i;
	    for (i = 0 ; i < bound ; i++) data[i].~T();
		free(data);
	}
	/**
	 * TODO Assignment operator
	 */
	vector &operator=(const vector &other) {
		clear();
		int i;
		for (i = 0 ; i < other.theSize ; i++) push_back(other[i]);
		return *this;
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, theSize)
	 */
	T & at(const size_t &pos) {
		if (pos < 0 || pos >= theSize) throw(index_out_of_bound());
		return data[pos];
	}
	const T & at(const size_t &pos) const {
		if (pos < 0 || pos >= theSize) throw(index_out_of_bound());
		return data[pos];
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, theSize)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos) {
		return this->at(pos);
	}
	const T & operator[](const size_t &pos) const  {
		return this->at(pos);
	}
	/**
	 * access the first element.
	 * throw container_is_empty if theSize == 0
	 */
	const T & front() const {
		if (theSize == 0) throw(container_is_empty());
		return data[0];
	}
	/**
	 * access the last element.
	 * throw container_is_empty if theSize == 0
	 */
	const T & back() const {
		if (theSize == 0) throw(container_is_empty());
		return data[theSize - 1];
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
		return iterator(this, 0);
	}
	const_iterator cbegin() const {
		return const_iterator(this, 0);
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
		return iterator(this, theSize);
	}
	const_iterator cend() const {
		return const_iterator(this, theSize);
	}
	/**
	 * checks whether the container is empty
	 */
	bool empty() const {
		return theSize == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
		return theSize;
	}
	/**
	 * returns the number of elements that can be held in currently allocated storage.
	 */
	size_t capacity() const {
		return theCapacity;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		theSize = 0;
	}
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) {
		if (theSize >= theCapacity) double_space();
		iterator p = end();
		++theSize;
        if (bound < theSize){
            new(data + bound)T(value);
            bound++;
        }
		for (; p != pos ; --p){
			 *p = *(p - 1);
		}
		*pos = value;
		return pos;
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value is true
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > theSize (in this situation ind can be theSize because after inserting the theSize will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value) {
		if (ind > theSize) throw(index_out_of_bound());
		if (theSize >= theCapacity) double_space();
        ++theSize;
        if (bound < theSize){
            new(data + bound)T(value);
            bound++;
        }
		size_t i;
		for (i = theSize - 1; i > ind; i--){
            data[i] = data[i - 1];
		}
		data[ind] = value;
		return iterator(this, ind);
	}
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) {
	    //std::cout << "orz" << std::endl;
		iterator p = pos;
		for (p = pos ; p != end() - 1 ; ++p){
			*p = *(p + 1);

		}
		--theSize;
		return pos;
	}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= theSize
	 */
	iterator erase(const size_t &ind) {
	    //std::cout << "orz" << std::endl;
		if (ind >= theSize) throw(index_out_of_bound());
		--theSize;
		int i;
		//std::cout << "orz" << std::endl;
		for (i = ind ; i < theSize ; ++i ) {

            data[i] = data[i + 1];
		}
		return iterator(this, ind);
	}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) {
		insert(end(), value);
	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if theSize() == 0
	 */
	void pop_back() {
		if (theSize == 0) throw(container_is_empty());
		theSize--;
	}
};


}

#endif