#include "stdafx.h"
#include "Queue.h"

Queue::Queue()
{
	head = NULL;
	tail = NULL;
}

void Queue::EnQueue(int elem) {
	Node* pointer = new Node;
	pointer->data = elem;
	pointer->next = NULL;
	if (head == NULL) {
		head = pointer;
	}
	else
		tail->next = pointer;
	tail = pointer;
	_tprintf(_T("Element has been inserted in the queue!\r\n"));
}
bool Queue::DeQueue(int* elem) {
	if (head == NULL) {
		_tprintf(_T("Queue is empty!\r\n"));
		return false;
	}
	if (elem != NULL)
	{
		*elem = head->data;
	}
	Node* temp = head;
	head = head->next;
	delete temp;
	return true;
}
void Queue::PrintQueue() {
	Node* pointer1 = head;
	if (head == NULL) {
		_tprintf(_T("Queue is empty!\r\n"));
	}
	else
		_tprintf(_T("Queue:\r\n"));
	while (pointer1 != NULL) {
		_tprintf(_T("%d,\r\n"), pointer1->data);
		pointer1 = pointer1->next;
	}
	_tprintf(_T("End"));
}
Queue::~Queue()
{
	while (DeQueue(0)) {}
}
