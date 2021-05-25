#include <cstddef>
#include <iostream>
#include <cstring>
#include <typeinfo>
#include <cstdlib>
#include "LinkedList.hpp"

template<class T>
LinkedList<T>::LinkedList() {
	head = NULL;
	tail = NULL;
	size = 0;
}

template<class T>
void LinkedList<T>::pushHead(T val) {
	size++;
	node<T>* temp = new node<T>;
	temp->next = head;
	temp->prev = NULL;
	temp->data = val;
	if (head != NULL)
		head->prev = temp;
	head = temp;
	if (tail == NULL)
		tail = head;
}

template<class T>
void LinkedList<T>::pushTail(T val) {
	size++;
	node<T>* temp = new node<T>;
	temp->next = NULL;
	temp->prev = tail;
	temp->data = val;
	if (tail != NULL)
		tail->next = temp;
	tail = temp;
	if (head == NULL)
		head = tail;
}

template<class T>
T LinkedList<T>::popHead() {
	if (size <= 0) {
		cout << "Error: can't pop head, list is empty" << endl;
		exit(EXIT_FAILURE);
	}
	T val = head->data;
	size--;
	if (size == 0) {
		delete head;
		head = NULL;
		tail = NULL;
	}
	else {
		head = head->next;
		delete head->prev;
		head->prev = NULL;
	}
	return val;
}

template<class T>
T LinkedList<T>::popTail() {
	if (size <= 0) {
		cout << "Error: can't pop tail, list is empty" << endl;
		exit(EXIT_FAILURE);
	}
	T val = tail->data;
	size--;
	if (size == 0) {
		delete tail;
		head = NULL;
		tail = NULL;
	}
	else {
		tail = tail->prev;
		delete tail->next;
		tail->next = NULL;
	}
	return val;
}

template <class T>
void LinkedList<T>::remove(node<T>* n) {
	if (n == NULL) return;
	if (n->prev == NULL)
		head = n->next;
	else
		(n->prev)->next = n->next;
	if (n->next == NULL)
		tail = n->prev;
	else
		(n->next)->prev = n->prev;
	size--;
	delete n;
}

template <class T>
void LinkedList<T>::remove(T val) {
	if (size <= 0) return;
	if (val == head->data) {
		popHead();
		return;
	}
	if (size == 1) return;
	node<T> iter = head->next;
	while (iter != tail) {
		if (val == iter->data) {
			(iter->prev)->next = iter->next;
			(iter->next)->prev = iter->prev;
			delete iter;
			size--;
			return;
		}
		iter = iter->next;
	}
	if (val == tail->data)
		popTail();
}

template <class T>
void LinkedList<T>::reverse() {
	node<T>* temp = head;
	head = tail;
	tail = temp;
	node<T> iter = tail;
	while (iter != NULL) {
		temp = iter->next;
		iter->next = iter->prev;
		iter->prev = temp;
		iter = iter->prev;
	}
}

template<class T>
T& LinkedList<T>::operator[](int index) {
	if (index >= size || index < 0) {
		cout << "Error: index out of bounds" << endl;
		exit(EXIT_FAILURE);
	}
	node<T>* iter;
	if (index + 1 <= size / 2) {
		iter = head;
		for (int i = 0; i < index; i++, iter = iter->next);
	}
	else {
		iter = tail;
		for (int i = size - 1; i > index; i--, iter = iter->prev);
	}
	return iter->data;
}

template <class T>
void LinkedList<T>::printList() {
	using namespace std;
	if (typeid(T).name() == typeid(int).name()) {
		node<T>* iter = head;
		cout << "[ prev<->(data, index)<->next ]" << endl << "NULL<->";
		for (int i = 0; i < size; i++, iter = iter->next)
			cout << "(" << iter->data << ", " << i << ")<->";
		cout << "NULL" << endl;
	}
	else
		cout << typeid(T).name() << " UNSUPPORTED TYPE" << endl;
}

template <class T>
int LinkedList<T>::getSize() { return size; }

template <class T>
void LinkedList<T>::randGenInt(int length, int lowBound, int upBound) {
	if (typeid(T).name() == typeid(int).name()) {
		while (length-- > 0)
			pushTail(rand() % (upBound - lowBound + 1) + lowBound);
	}
}