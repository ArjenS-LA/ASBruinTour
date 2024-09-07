#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cctype>


//IMPLEMENTING AN OPEN HASHMAP THAT ACTS LIKE <unordered_map> IN C++ STL
//TIME COMPLEXITY OF HASHMAP OPERATIONS IS O(1) ON AVERAGE
template <typename T>
class HashMap
{
public:
	HashMap(double max_load = 0.75)
		: m_load_factor(max_load), m_bucket_capacity(10) // constructor
	{
		//INITIALIZE THE BUCKET WITH 10 BUCKETS
		buckets.resize(10, nullptr);
	}

	~HashMap() // destructor; deletes all of the items in the hashmap 
	{
		//DELETE THE NODES IN THE HASHMAP
		clear();
	}

	int size() const { return m_size; }
	// return the number of associations in the hashmap 
	// The insert method associates one item (key) with another (value). 
	// If no association currently exists with that key, this method inserts 
	// a new association into the hashmap with that key/value pair. If there is 
	// already an association with that key in the hashmap, then the item 
	// associated with that key is replat6ced by the second parameter (value). 
	// Thus, the hashmap must contain no duplicate keys. 
	void insert(const std::string& key, const T& value)
	{
		unsigned int index = hash(key);

		//CHECK IF BUCKET HAS KEY NODE
		if (buckets[index] != nullptr)
		{
			//IF BUCKET HAS AN EQUAL KEY, REPLACE VALUE
			Node* temp = buckets[index];
			if (temp->key == key)
			{
				temp->value = value;
				return;
			}
			while (temp->indexNext != nullptr)
			{
				if (temp->key == key)
				{
					temp->value = value;
					return;
				}
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

	// Defines the bracket operator for HashMap, so you can use your map like this:
	// your_map["david"] = 2.99;
	// If the key does not exist in the hashmap, this will create a new entry in 
	// the hashmap and map it to the default value of type T. Then it will return a
	// reference to the newly created value in the map.
	T& operator[](const std::string& key)
	{
		//CHECK IF KEY EXISTS
		T* value = find(key);

		//IF KEY DOES NOT EXIST, INSERT A NEW NODE
		if (value == nullptr)
		{
			insert(key, T());
			value = find(key);
		}

		//RETURN THE VALUE
		return *value;
	}

	// If no association exists with the given key, return nullptr; otherwise, 
	// return a pointer to the value associated with that key. This pointer can be 
	// used to examine that value or modify it directly within the map. 
	T* find(const std::string& key) const
	{
		unsigned int index = hash(key);

		//CHECK IF BUCKET HAS KEY NODE
		if (buckets[index] != nullptr)
		{
			Node* temp = buckets[index];
			while (temp != nullptr)
			{
				//IF BUCKET HAS AN EQUAL KEY, RETURN VALUE
				if (temp->key == key)
					return &temp->value;

				temp = temp->indexNext;
			}
		}

		//BUCKET DOES NOT HAVE AN EQUAL KEY, RETURN NULLPTR
		return nullptr;
	}

	T* find(const std::string& key)
	{
		const auto& hm = *this;
		return const_cast<T*>(hm.find(key));
	}
private:
	//IMPLEMENT A STRUCT FOR THE NODES IN THE HASHMAP
	struct Node
	{
		Node(std::string k, T v) : key(k), value(v) {}

		std::string key;
		T value;

		Node* next = nullptr, * prev = nullptr;
		Node* indexNext = nullptr;

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


	//IMPLEMENT A HASH FUNCTION
	unsigned int hash(const std::string& key) const
	{
		std::hash<std::string> str_hash;
		unsigned int hashVal = str_hash(key);

		//ADD KEY
		unsigned int bucketNum = hashVal % m_bucket_capacity;
		return bucketNum;
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

	//IMPLEMENT A VECTOR OF NODES WITH BUCKET CAPACITY OF 100
	std::vector<Node*> buckets;
	double m_load_factor;
	int m_bucket_capacity;
	double m_size = 0;

	//IMPLEMENT A HEAD AND TAIL POINTER FOR TRAVERSAL
	Node* head = nullptr, * tail = nullptr;
};

#endif // !"HASHMAP_H"#pragma once
