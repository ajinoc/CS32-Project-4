// HashTable.h

#ifndef HASHTABLE_INCLUDED
#define HASHTABLE_INCLUDED

template <typename KeyType, typename ValueType>
class HashTable
{
public:
	HashTable(unsigned int numBuckets, unsigned int capacity);
	~HashTable();
	bool isFull() const;
	bool set(const KeyType& key, const ValueType& value, bool permanent = false);
	bool get(const KeyType& key, ValueType& value) const;
	bool touch(const KeyType& key);
	bool discard(KeyType& key, ValueType& value);

private:
	//  We prevent a HashTable from being copied or assigned by declaring the
	//  copy constructor and assignment operator private and not implementing them.
	HashTable(const HashTable&);
	HashTable& operator=(const HashTable&);

	struct Node
	{
		KeyType key;
		ValueType value;
		bool isPermanent;
		Node* m_next = nullptr;
		Node* m_prev = nullptr;
	};

	struct ListNode
	{
		KeyType key;
		ListNode* m_prev = nullptr;
		ListNode* m_next = nullptr;
	};

	struct RecentList
	{
		ListNode* front = nullptr;
		ListNode* back = nullptr;
	};

	const unsigned int m_numBuckets;
	const unsigned int m_capacity;
	unsigned int numEntries = 0;
	Node** m_table;
	RecentList m_list;
};

template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::HashTable(unsigned int numBuckets, unsigned int capacity)
	: m_numBuckets(numBuckets), m_capacity(capacity)
{
	m_table = new Node*[m_numBuckets];
	for (unsigned int i = 0; i < m_numBuckets; i++)
		m_table[i] = nullptr;
}

template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::~HashTable()
{
	for (unsigned int i = 0; i < m_numBuckets; i++)
	{
		Node* curNode = m_table[i];
		while (curNode != nullptr)
		{
			if (curNode->m_next == nullptr)
			{
				delete curNode;
				break;
			}
			curNode = curNode->m_next;
			delete curNode->m_prev;
		}
	}

	delete[] m_table;

	ListNode* curNode = m_list.front;
	while (curNode != nullptr)
	{
		if (curNode->m_next == nullptr)
		{
			delete curNode;
			break;
		}
		curNode = curNode->m_next;
		delete curNode->m_prev;
	}
}

template <typename KeyType, typename ValueType>
inline bool HashTable<KeyType, ValueType>::isFull() const
{
	return numEntries == m_capacity;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::set(const KeyType& key, const ValueType& value, bool permanent)
{
	unsigned int computeHash(KeyType);
	unsigned int bucketNum = computeHash(key) % m_numBuckets;

	Node* curNode = m_table[bucketNum];
	if (curNode == nullptr)
	{
		if (isFull())
			return false;
		else
		{
			// First time setting this value, and first in bucket
			m_table[bucketNum] = new Node;
			m_table[bucketNum]->key = key;
			m_table[bucketNum]->value = value;
			m_table[bucketNum]->isPermanent = permanent;
			numEntries++;

			// Add key to beginning of RecentList
			if (!permanent)
			{
				if (m_list.front == nullptr)
				{
					// first entry
					m_list.front = new ListNode;
					m_list.front->key = key;
					m_list.back = m_list.front;
				}
				else
				{
					// insert before first node
					ListNode* tempNode = m_list.front;
					m_list.front = new ListNode;
					m_list.front->key = key;
					m_list.front->m_next = tempNode;
					tempNode->m_prev = m_list.front;
				}
			}
			return true;
		}
	}
	else
	{
		// bucket is not empty, key may have already been set
		while (true)
		{
			// Node already present
			if (curNode->key == key)
			{
				curNode->value = value;
				if (!curNode->isPermanent)
				{
					// push listNode to the front
					ListNode* curListNode = m_list.front;
					while (true)
					{
						if (curListNode->key == key)
						{
							// First and/or only ListNode, our job is already done
							if (curListNode->m_prev == nullptr)
								break;

							// Last ListNode
							if (curListNode->m_next == nullptr)
							{
								m_list.back->m_next = m_list.front;
								m_list.front->m_prev = m_list.back;
								m_list.front = m_list.back;
								m_list.back = m_list.back->m_prev;
								m_list.back->m_next = nullptr;
								break;
							}

							// ListNode is in the middle somewhere
							curListNode->m_prev->m_next = curListNode->m_next;
							curListNode->m_next->m_prev = curListNode->m_prev;
							curListNode->m_prev = nullptr;
							curListNode->m_next = m_list.front;
							m_list.front->m_prev = curListNode;
							m_list.front = curListNode;
							break;
						}
						curListNode = curListNode->m_next;
					}
				}
				return true;
			}
			if (curNode->m_next == nullptr)
			{
				// end of bucket linked list, first time adding this mapping
				if (isFull())
					return false;
				else
				{
					// append node to end of bucket linked list
					curNode->m_next = new Node;
					curNode->m_next->key = key;
					curNode->m_next->value = value;
					curNode->m_next->m_next = nullptr;
					curNode->m_next->m_prev = curNode;
					curNode->m_next->isPermanent = permanent;
					numEntries++;

					// Update RecentList
					if (!permanent)
					{
						if (m_list.front == nullptr)
						{
							// first entry
							m_list.front = new ListNode;
							m_list.front->key = key;
							m_list.back = m_list.front;
						}
						else
						{
							// insert before first node
							ListNode* tempNode = m_list.front;
							m_list.front = new ListNode;
							m_list.front->key = key;
							m_list.front->m_next = tempNode;
							tempNode->m_prev = m_list.front;
						}
					}
					return true;
				}
			}
			curNode = curNode->m_next;
		}
	}
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::get(const KeyType& key, ValueType& value) const
{
	unsigned int computeHash(KeyType);
	unsigned int bucketNum = computeHash(key) % m_numBuckets;

	Node* curNode = m_table[bucketNum];
	if (curNode == nullptr)
		return false;
	
	while (true)
	{
		if (curNode->key == key)
		{
			value = curNode->value;
			return true;
		}
		if (curNode->m_next == nullptr)
			return false;
		curNode = curNode->m_next;
	}
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::touch(const KeyType& key)
{
	// Hunt down the node where the mapping is held
	unsigned int computeHash(KeyType);
	unsigned int bucketNum = computeHash(key) % m_numBuckets;

	Node* curNode = m_table[bucketNum];
	if (curNode == nullptr)
		return false;

	while (true)
	{
		if (curNode->key == key)
		{
			if (curNode->isPermanent)
				return false;

			// Push ListNode to the front
			ListNode* curListNode = m_list.front;
			while (true)
			{
				if (curListNode->key == key)
				{
					// First and/or only ListNode, our job is already done
					if (curListNode->m_prev == nullptr)
						break;

					// Last ListNode
					if (curListNode->m_next == nullptr)
					{
						m_list.back->m_next = m_list.front;
						m_list.front->m_prev = m_list.back;
						m_list.front = m_list.back;
						m_list.back = m_list.back->m_prev;
						m_list.back->m_next = nullptr;
						m_list.front->m_prev = nullptr;
						break;
					}

					// ListNode is in the middle somewhere
					curListNode->m_prev->m_next = curListNode->m_next;
					curListNode->m_next->m_prev = curListNode->m_prev;
					curListNode->m_prev = nullptr;
					curListNode->m_next = m_list.front;
					m_list.front->m_prev = curListNode;
					m_list.front = curListNode;
					break;
				}
				curListNode = curListNode->m_next;
			}
			return true;
		}
		if (curNode->m_next == nullptr)
			return false;
		curNode = curNode->m_next;
	}
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::discard(KeyType& key, ValueType& value)
{
	if (m_list.back == nullptr) // no associations
		return false;
	key = m_list.back->key;

	// Delete ListNode
	if (m_list.back->m_prev == nullptr) // One item list
	{
		delete m_list.back;
		m_list.back = nullptr;
		m_list.front = nullptr;
	}
	else // More items before the last one
	{
		ListNode* tempNode = m_list.back->m_prev;
		delete m_list.back;
		m_list.back = tempNode;
		m_list.back->m_next = nullptr;
	}

	// Delete Node from HashTable
	unsigned int computeHash(KeyType);
	unsigned int bucketNum = computeHash(key) % m_numBuckets;

	Node* curNode = m_table[bucketNum];
	while (true)
	{
		if (curNode->key == key)
		{
			value = curNode->value;

			// Only node in bucket
			if (curNode->m_prev == nullptr && curNode->m_next == nullptr)
			{
				delete curNode;
				m_table[bucketNum] = nullptr;
				break;
			}

			// First node in bucket
			if (curNode->m_prev == nullptr)
			{
				curNode = curNode->m_next;
				delete m_table[bucketNum];
				m_table[bucketNum] = curNode;
				m_table[bucketNum]->m_prev = nullptr;
				break;
			}

			// Last node in bucket
			if (curNode->m_next == nullptr)
			{
				Node* tempNode = curNode->m_prev;
				delete curNode;
				curNode = tempNode;
				curNode->m_next = nullptr;
				break;
			}
			
			// Node is somewhere in the middle
			curNode->m_prev->m_next = curNode->m_next;
			curNode->m_next->m_prev = curNode->m_prev;
			delete curNode;
			break;
		}
		curNode = curNode->m_next;
	}

	numEntries--;
	return true;
}

#endif HASHTABLE_INCLUDED