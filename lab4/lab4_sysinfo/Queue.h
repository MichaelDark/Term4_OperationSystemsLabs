#pragma once

struct Node {
	int data;
	Node* next;
};

class Queue {
	struct Node* head;
	struct Node* tail;
public:
	Queue(); 
	~Queue();
	void EnQueue(int element);
	bool DeQueue(int* element);
	void PrintQueue();
};