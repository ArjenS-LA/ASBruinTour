#ifndef SUPPORT_H
#define SUPPORT_H

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <functional>

template <typename T>
class DubHashMap {
private:
	//IMPLEMENT A STRUCT FOR THE NODES IN THE HASHMAP
	struct Node
	{
		Node(std::string k, T v) : key(k), value(v) {}

		std::string key;
		T value;

		Node* indexNext = nullptr;
		Node* next = nullptr; Node* prev = nullptr;
	};

	//IMPLEMENT A FUNCTION TO DELETE THE NODES IN THE HASHMAP
	void clear()
	{
		//DELETE THE NODES IN THE HASHMAP
		Node* temp = head;
		while (temp != nullptr)
		{
			Node* next = temp->next;
			delete temp;
			temp = next;
		}
	}

	//IMPLEMENT A FUNCTION TO CHECK IF THE BUCKET NEEDS TO BE RESIZED
	bool needsResize() const
	{
		//CALCULATE THE LOAD FACTOR
		double loadFactor = m_size / m_bucket_capacity;

		//IF THE LOAD FACTOR IS GREATER THAN THE MAX LOAD FACTOR, RESIZE THE BUCKET
		if (loadFactor > m_load_factor)
			return true;

		return false;
	}

	//IMPLEMENT A FUNCTION TO EXPAND THE BUCKET CAPACITY
	//AND REHASH THE NODES
	void resize()
	{
		//DOUBLE THE BUCKET CAPACITY
		m_bucket_capacity *= 2;

		//CREATE A NEW VECTOR OF NODES
		std::vector<Node*> newBuckets(m_bucket_capacity, nullptr);

		//REHASH THE NODES
		Node* temp = head;
		while (temp != nullptr)
		{
			//GET THE NEW INDEX
			unsigned int index = hash(temp->key);

			//ADD THE NODE TO THE NEW BUCKET
			if (newBuckets[index] == nullptr)
			{
				newBuckets[index] = temp;
				temp->indexNext = nullptr;
			}
			else
			{
				Node* temp2 = newBuckets[index];
				while (temp2->indexNext != nullptr)
					temp2 = temp2->indexNext;

				temp2->indexNext = temp;
				temp->indexNext = nullptr;
			}

			//MOVE TO THE NEXT NODE
			temp = temp->next;
		}

		//UPDATE THE BUCKET
		buckets.resize(m_bucket_capacity, nullptr);
		buckets.swap(newBuckets);

	}

	//IMPLEMENT A FUNCTION THAT ADDS NODES TO THE TAIL
	void addToTail(Node* node)
	{
		//IF TAIL IS NULL, SET THE NODE AS THE HEAD AND TAIL
		if (tail == nullptr)
		{
			head = node;
			tail = node;
		}
		else
		{
			//ADD THE NODE TO THE TAIL
			tail->next = node;
			node->prev = tail;
			node->next = nullptr;  // Ensure next is nullptr for the new tail node
			tail = node;
		}
	}

	//IMPLEMENT A HASH FUNCTION
	unsigned int hash(const std::string& key) const
	{
		std::hash<std::string> str_hash;
		unsigned int hashVal = str_hash(key);

		//ADD KEY
		unsigned int bucketNum = hashVal % m_bucket_capacity;
		return bucketNum;
	}

	//IMPLEMENT A VECTOR OF NODES WITH BUCKET CAPACITY OF 100
	std::vector<Node*> buckets;
	double m_load_factor;
	int m_bucket_capacity;
	double m_size = 0;

	Node* head = nullptr, *tail = nullptr;

public:
    DubHashMap(double max_load = 0.75)
		: m_load_factor(max_load), m_bucket_capacity(10) // constructor
	{
		//INITIALIZE THE BUCKET WITH 10 BUCKETS
		buckets.resize(10, nullptr);
	}

	~DubHashMap() { clear(); } // destructor

    // Insert a key-value pair into the hash table
    void insert(const std::string& key, const T& value) 
	{
		unsigned int index = hash(key);

		//CHECK IF BUCKET HAS KEY NODE
		if (buckets[index] != nullptr)
		{
			//IF BUCKET HAS AN EQUAL KEY, REPLACE VALUE
			Node* temp = buckets[index];
			while (temp->indexNext != nullptr)
			{
				////PREVENT DUPLICATE KEY-VALUE PAIRS
				//if (temp->key == key && temp->value == value)
				//{
				//	return;
				//}
				temp = temp->indexNext;
			}

			//BUCKET DOES NOT HAVE AN EQUAL KEY
			//CHECK FOR RESIZE, THEN ADD A NEW NODE VIA TAIL POINTER
			if (needsResize())
				resize();

			//ADD A NEW NODE TO THE TAIL
			Node* addMe = new Node(key, value);
			addToTail(addMe);

			//UPDATE INDEX POINTER
			temp->indexNext = addMe;
			m_size++;
		}
		else //BUCKET IS EMPTY
		{
			//IF BUCKET IS EMPTY
			//CHECK FOR RESIZE, THEN ADD A NEW NODE VIA TAIL POINTER
			if (needsResize())
				resize();

			//ADD A NEW NODE TO THE TAIL
			Node* addMe = new Node(key, value);
			addToTail(addMe);

			//ADD INDEX POINTER
			buckets[index] = addMe;
			m_size++;
		}
    }

    // Find all values associated with a given key
    std::vector<T> find(const std::string& key) const 
	{
		unsigned int index = hash(key);
        Node* temp = buckets[index];

        std::vector<T> values;
		while (temp != nullptr)
		{
			if (temp->key == key)
				values.push_back(temp->value);
			temp = temp->indexNext;
		}

        return values;
    }
};

#endif