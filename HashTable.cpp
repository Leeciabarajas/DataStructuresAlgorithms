//Copyright 2019, Bradley Peterson, Weber State University, all rights reserved. (Feb 2019)

#include <iostream>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include <functional>  //For the C++11 hash
#include <chrono>      

//forward declarations
template <typename T> class Iterator;

using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::list;
using std::string;
using std::stringstream;


//************************************************************************
//A class I designed to help keep track of how much memory you allocate
//Do not modify, this is not part of your assignment, it just helps test it.
//For this to work, a class needs to inherit off of this one.
//Then this does the rest of the work, since it
//overloads new, new[], delete, and delete[].
//************************************************************************
class ManageMemory {
public:

	static std::size_t getTotalSize() {
		std::size_t total = 0;
		std::map<void *, std::size_t>::iterator iter;
		for (iter = mapOfAllocations.begin(); iter != mapOfAllocations.end(); ++iter) {
			total += iter->second;
		}
		return total;
	}

	//I overloaded the new and delete keywords so I could manually track allocated memory.
	void* operator new(std::size_t x) {
		void *ptr = ::operator new(x);
		mapOfAllocations[ptr] = x;
		return ptr;
	}
	void* operator new[](std::size_t x) {
		void *ptr = ::operator new[](x);
		mapOfAllocations[ptr] = x;
		return ptr;
	}
		void operator delete(void* x) {
		mapOfAllocations.erase(x);
		::operator delete(x);
	}
	void operator delete[](void* x) {
		mapOfAllocations.erase(x);
		::operator delete[](x);
	}
private:
	static std::map<void *, std::size_t> mapOfAllocations;
};
std::map<void *, std::size_t> ManageMemory::mapOfAllocations;


//****************************
//The KeyValuePair struct
//****************************
template<typename K, typename V>
 struct KeyValuePair {
	K key;
	V value;
	KeyValuePair(); 
	KeyValuePair(const K& key ,const V& value); // make const and && 
};
//TODO:
template<typename K, typename V>
KeyValuePair<K,V>::KeyValuePair() {
}

template<typename K, typename V>
KeyValuePair<K, V>::KeyValuePair(const K& key, const V& value) {
	this->key = key; 
	this->value = value; 
}

//****************************
//The Node struct
//****************************
template <typename T>
struct Node {
	T data; 
	Node<T>*link; 
};

//****************************
//The Linked List class
//****************************
template <typename T>
class LinkedList : public ManageMemory
{
private:
	Node<T> * front{ nullptr };
	Node<T> * back{ nullptr }; 
	unsigned int count{ 0 };
public:
	~LinkedList();
	void insertLast(const T& data);
	Iterator<T> begin();
	Iterator<T> end();
};

template <typename T>
LinkedList<T>::~LinkedList() {
	if (front) {
		Node<T> * temp{ front->link };
		while (temp) {
			delete front;
			front = temp;
			temp = temp->link;
		}
		delete front;
	}
}

template <typename T>
void LinkedList<T>::insertLast(const T& data) {
	Node<T> * temp = new Node<T>();
	temp->data = data;
	if (!front) {
		front = temp;
	}
	else {
		back->link = temp;
	}
	back = temp;
	count++;
}
template <typename T>
Iterator<T> LinkedList<T>::begin() {
	Iterator<T> temp; 
	if (front == nullptr) {
		temp.AtEnd = true; 
	}
	else {
		temp.AtEnd = false; 
		temp.current = this->front;
	}
	return temp; 
}

template <typename T>
Iterator<T> LinkedList<T>::end() {
	Iterator<T> temp; 
	temp.AtEnd = true;
	temp.current = this->back; 
	return temp; 
}

//****************************
//The Linked List Iterator class
//****************************
template<typename T>
class Iterator {
friend class LinkedList<T>;
private:
	Node<T> * current{ nullptr };//check this
	bool AtEnd{ false };
public:
	T& operator*() const;
	bool operator!=(const Iterator<T>& rhs) const;
	Iterator<T> operator++();
	bool operator==(const Iterator<T>& rhs) const;
};

template<typename T>
T& Iterator<T>::operator*()const {
	return this->current->data; 
}

template<typename T>
bool Iterator<T>::operator!=(const Iterator<T>& rhs)const {
	return (this->AtEnd != rhs.AtEnd || this->current != rhs.current); 
}

template<typename T>
Iterator<T> Iterator<T>::operator++() {
	if (this->current->link != nullptr) {
		current = current->link;
	}
	else {
		this->AtEnd = true;
	}
	return *this; 
}
template <typename T>
bool Iterator<T>::operator==(const Iterator<T>& rhs) const
	{
		if (this->current != rhs.current) {
			return false;
		}
		else {
			return true;
		}
	

}

//****************************
//The HashTable class
//****************************
template <typename K, typename V>
class HashTable : public ManageMemory
{
	//friend KeyValuePair<K, V>; 
private:
	int buckets=0; 
	LinkedList<KeyValuePair<K, V>> * arr; 
public: 
	HashTable(int buckets);
	~HashTable(); 
	void add(K key, V value); 
	V get(K key);
	V& operator[](K key); 
	bool exists(K key); 
};// end class HashTable



template <typename K, typename V>
HashTable<K, V>::HashTable(int buckets) {//check w/adam 
	this->arr = new LinkedList<KeyValuePair<K,V>>[buckets];  //create array dynamically based off of buckets
	this->buckets = buckets;
}

template <typename K, typename V>
HashTable<K,V>::~HashTable() {
	if (arr) { 
		delete[] arr;
	}
}

template <typename K, typename V>
void HashTable<K,V>::add(K key, V value) {
	unsigned int index; 
	unsigned int hash;
	if (this->buckets != 0) {
		hash = std::hash<K>{}(key);			//hash key
		index = hash % this->buckets; 		//mod by number of buckets
		arr[index].insertLast(KeyValuePair<K, V>(key, value)); 	//go to bucket and add it in. Is this right? 1.04	
	}
}

template <typename K, typename V>
V HashTable<K,V>::get(K keyz) {
	unsigned int index;
	unsigned int hash;
	if (this->buckets == 0) {
		throw 1; 
	}
	else {
		hash = std::hash<K>{}(keyz);
		index = hash % this->buckets;

		auto start = arr[index].begin();
		auto ended = arr[index].end();

		while (start != ended) {

			if (keyz == (*start).key) {
				return (*start).value;
			}
			else {
				++start;
			}
		}

		if (start == ended) {
			throw 1;
		}
	}
}

template <typename K, typename V>
V& HashTable<K, V>::operator[](K keyz) {
	unsigned int index;
	unsigned int hash;
	K key = keyz;
	if (this->buckets != 0) {
		hash = std::hash<K>{}(keyz);
		index = hash % this->buckets;

		auto start = arr[index].begin();
		auto ended = arr[index].end();

		while (start != ended) {
			if (keyz == (*start).key) {
				return (*start).value;
			}
			else {
				++start;
			}
		}
		if (start == ended) {
			throw 1;
		}
	}

}
template <typename K, typename V>
bool HashTable<K, V>::exists(K keyz) {
	unsigned int index;
	unsigned int hash;
	if (this->buckets == 0) {
		throw 1;
	}
	else {
		hash = std::hash<K>{}(keyz);
		index = hash % this->buckets;

		auto start = arr[index].begin();
		auto ended = arr[index].end();

		while (start != ended) {

			if (keyz == (*start).key) {
				return true;
			}
			else {
				++start;
			}
		}

		if (start == ended) {
			return false; 
		}
	}

}






  //************************************************************************************
  //A quick and simple class that simulates a Product object.  Do not modify
  //************************************************************************************
class Product {
public:
	void setCost(int cost);
	void setName(const string&);
	string getName();
	int getCost();
	string getAllInfo();
private:
	string name;
	int cost;
};
void Product::setCost(int cost) {
	this->cost = cost;
}
void Product::setName(const string& name) {
	this->name = name;
}
string Product::getName() {
	return name;
}
int Product::getCost() {
	return cost;
}
string Product::getAllInfo() {
	stringstream ss;
	ss << "Name: " << name << ", Cost: " << cost;
	return ss.str();
}


//This helps with testing, do not modify.
template <typename T>
string NumberToString(T Number)
{
	stringstream ss;
	ss << Number;
	return ss.str();
}

//This helps with testing, do not modify.
bool checkEmpty(string testName, string whatItIs) {

	if (whatItIs != "") {
		cout << "Passed " << testName << ", the data was " << whatItIs << endl;
		return true;
	}
	else {
		cout << "***Failed test " << testName << " *** " << endl << "   No data was found! " << endl;
		return false;
	}
}

//This helps with testing, do not modify.
bool checkTest(string testName, string whatItShouldBe, string whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	}
	else if (whatItShouldBe == "") {
		cout << "****** Failed test " << testName << " ****** " << endl << "     Output was '" << whatItIs << endl << "'     Output should have been blank" << endl;
		return false;

	}
	else {
		cout << "****** Failed test " << testName << " ****** " << endl << "     Output was " << whatItIs << endl << "     Output should have been " << whatItShouldBe << endl;
		return false;
	}
}

//This helps with testing, do not modify.
bool checkTest(string testName, int whatItShouldBe, int whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	}
	else {
		cout << "****** Failed test " << testName << " ****** " << endl << "     Output was " << whatItIs << endl << "     Output should have been " << whatItShouldBe << endl;
		return false;
	}
}

//This helps with testing, do not modify.
bool checkTestMemory(string testName, int whatItShouldBe, int whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	}
	else {
		cout << "***Failed test " << testName << " *** " << endl << "  There are " << whatItIs << " bytes of memory yet to be reclaimed!" << endl;
		return false;
	}
}


//This helps with testing, do not modify.
void testSimpleIntHash() {

	//cout << "|||### You need to comment in testSimpleIntHash() when it's ready and remove this###|||" << endl;

	HashTable<string, string> myHash(200000);

	//Test #1, add "Nuggets" into our hash with a key of 6.  Try to get it.
	myHash.add("6", "Nuggets");
	checkTest("testSimpleIntHash #1", "Nuggets", myHash.get("6"));

	//Test #2, attempt to get the Nuggets using the operator[] code.
	checkTest("testSimpleIntHash #2", "Nuggets", myHash["6"]);

	//Test #3, attempt to change the value at this location:
	myHash["6"] = "Jazz";

	checkTest("testSimpleIntHash #3", "Jazz", myHash["6"]);

	//Test #4,
	//See if we can find it
	if (myHash.exists("6")) {
	  checkTest("testSimpleIntHash #4", "", "");
	}
	else {
	  checkTest("testSimpleIntHash #4", "This test should have found an item with key \"6\"", "This test did NOT find an item with key \"6\"");
	}

	//Test #5, see if we can find something that doesn't exist yet.
	if (myHash.exists("1234")) {
	  checkTest("testSimpleIntHash #5", "This test should NOT have found an item with key \"1234\".", "This test found an item with key \"1234\"");
	}
	else {
	  checkTest("testSimpleIntHash #5", "", "");
	}

	//Add more into the hash
	myHash.add("13", "Lakers");
	myHash.add("25", "Bulls");
	myHash.add("101", "Pelicans");
	myHash.add("77", "Bucks");
	myHash.add("12", "Thunder");
	myHash.add("42", "Nets");
	myHash.add("97", "Timberwolves");
	myHash.add("123", "Hornets");
	myHash.add("500", "Mavericks");

	//Attempt to get them back
	checkTest("testSimpleIntHash #6", "Thunder", myHash.get("12"));
	checkTest("testSimpleIntHash #7", "Bucks", myHash.get("77"));
	checkTest("testSimpleIntHash #8", "Pelicans", myHash.get("101"));
	checkTest("testSimpleIntHash #9", "Bulls", myHash.get("25"));
	checkTest("testSimpleIntHash #10", "Lakers", myHash.get("13"));
	checkTest("testSimpleIntHash #11", "Mavericks", myHash.get("500"));
	checkTest("testSimpleIntHash #12", "Hornets", myHash.get("123"));
	checkTest("testSimpleIntHash #13", "Timberwolves", myHash.get("97"));
	checkTest("testSimpleIntHash #14", "Nets", myHash.get("42"));

	// Start by preparing 200000 keys
	int key = 0;
	string *keys = new string[200000];
	stringstream out;
	cout << "Preparing to insert many items...be patient" << endl;
	for (unsigned int i = 0; i < 200000; i++) {

	  //this next part just helps create some variation in generated W#s...
	  if (i % 2 == 0) {
	    key += 17;
	  }
	  else if (i % 3 == 0) {
	    key += 23;
	  }
	  else if (i % 5 == 0) {
	    key += 51;
	  }
	  else if (i % 7 == 0) {
	    key += 13;
	  }
	  else {
	    key += 71;
	  }
	  //convert an int to a string via help from the stringstream class
	  out.str("");
	  out << key;
	  keys[i] = out.str();
	}

	cout << "Inserting 100000 items..." << endl;
	//Now add in 100000 keys and time it
	for (unsigned int i = 0; i < 100000; i++) {
	  myHash.add(keys[i], "a"); //Just add a bunch of letter a's
	}

	//Test retrieving two of the items
	checkTest("testSimpleIntHash #15", "a", myHash.get(keys[500]));
	checkTest("testSimpleIntHash #16", "a", myHash.get(keys[999]));

	//Now time how long it takes to get 100000 items
	cout << "Verifying 100000 items...this definitely shouldn't take more than 5 seconds" << endl;
	bool missing = false;
	auto start = std::chrono::high_resolution_clock::now();
	for (unsigned int i = 0; i < 100000; i++) {
	  if (!myHash.exists(keys[i])) {
	    cout << "*** ERROR! Key #" << i << " was not found!" << endl;
	    missing = true;
	  }
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::micro> diff = end - start;
	double benchmarkTime = diff.count() / 1000.0;
	cout << "Retrieving 100000 items took " << benchmarkTime << " milliseconds" << endl;


	//Now add in the next 100000 keys
	for (unsigned int i = 100000; i < 200000; i++) {
	  myHash.add(keys[i], "a"); //Just add a bunch of letter a's
	}
	start = std::chrono::high_resolution_clock::now();
	for (unsigned int i = 100000; i < 200000; i++) {
	  if (!myHash.exists(keys[i])) {
	    cout << "*** ERROR! Key #" << i << " was not found!" << endl;
	    missing = true;
	  }
	}
	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	double benchmarkTime2 = diff.count() / 1000.0;
	cout << "Retrieving 100000 more items took " << benchmarkTime2 << " milliseconds" << endl;

	if (missing) {
	  cout << "*** Failed testSimpleIntHash #17, one or more keys were not found" << endl;
	}
	else {
	  cout << "Passed testSimpleIntHash #17, one or more keys were not found" << endl;
	}

	if (benchmarkTime < 5000 && benchmarkTime2 < (benchmarkTime * 2 * 1.5)) { //The 1.5 gives an extra 50% wiggle room
	  cout << "Passed testSimpleIntHash #18, completed removeAllInstances in " << benchmarkTime2 << " milliseconds." << endl;

	}
	else {
	  cout << "*** Failed testSimpleIntHash #18, removeAllInstances took " << benchmarkTime2 << " milliseconds." << endl;

	  if (benchmarkTime > 5000) {
	    cout << "*** The insertion time took too long.  It hould have been a 5000 milliseconds or less" << endl;
	  }
	  if (benchmarkTime2 < (benchmarkTime * 2 * 1.5)) {
	    cout << "*** The two times should have been relatively similar" << endl;
	  }
	}
	delete[] keys;

}


void testHashOfObjects() {

	//cout << "|||### You need to comment in testHashOfObjects() when it's ready and remove this###|||" << endl;

	HashTable<string, Product> myHash(2000);

	//Test #1, add in a studentObject.  Try to retrive it.
	Product tempProduct;
	tempProduct.setCost(5);
	tempProduct.setName("Silly string");
	myHash.add("12341-51231", tempProduct);
	checkTest("testHashOfObjects #1", "Silly string", myHash.get("12341-51231").getName());

	//Test #2, attempt to get the Product using its ID code
	checkTest("testHashOfObjects #2", "Silly string", myHash["12341-51231"].getName());

	//Test #3, see what happens if two Products have the same ID code.  This should overwrite the former.
	tempProduct.setCost(18);
	tempProduct.setName("Novelty foam hat");
	myHash["12341-51231"] = tempProduct;
	checkTest("testHashOfObjects #3", "Novelty foam hat", myHash["12341-51231"].getName());

	//Test #4,
	//See if we can find it
	if (myHash.exists("12341-51231")) {
	  checkTest("testHashOfObjects #4", "", "");
	}
	else {
	  checkTest("testHashOfObjects #4", "This test should have found an item with key 12341-51231", "This test did NOT find an item with key 12341-51231");
	}

	//Test #5, see if we can find something that doesn't exist yet.
	if (myHash.exists("56756-75675")) {
	  checkTest("testHashOfObjects #5", "This test should NOT have found an item with key 56756-75675.", "This test found an item with key56756-75675");
	}
	else {
	  checkTest("testHashOfObjects #5", "", "");
	}

	//Now throw in many more items.
	int value = 0;
	string key;
	stringstream out;
	for (unsigned int i = 0; i < 2000; i++) {
	  //this next part just helps create some variation for our produce ID codes.
	  if (i % 2 == 0) {
	    value += 107;
	  }
	  else if (i % 3 == 0) {
	    value += 83;
	  }
	  else if (i % 5 == 0) {
	    value += 47;
	  }
	  else if (i % 7 == 0) {
	    value += 131;
	  }
	  else {
	    value += 53;
	  }
	  key = "12345-";
	  out.str("");
	  if (value < 100000)
	    key = key + "0";
	  if (value < 10000)
	    key = key + "0";
	  if (value < 1000)
	    key = key + "0";
	  if (value < 100)
	    key = key + "0";
	  if (value < 10)
	    key = key + "0";
	  out << value;
	  string temp = out.str();
	  if (temp.length() > 8) {
	    temp = temp.substr(0, 8);
	  }
	  key = key + temp;
	  //Whew, that took a while, but the W# is in key, and is ready to go

	  //Create the student record, and load in values.
	  tempProduct.setName("Acme Product");
	  tempProduct.setCost(rand() % 41);

	  //Testing the hash table "add" method
	  myHash.add(key, tempProduct);
	}

	//Make sure one went in correctly.  get it.
	checkEmpty("testHashOfObjects #6", myHash["12345-002112"].getAllInfo());

}


void pressAnyKeyToContinue() {
	cout << "Press Enter to continue...";
	cin.get();
}

int main() {

	testSimpleIntHash();
	checkTestMemory("Memory Leak/Allocation Test #1", 0, (int)ManageMemory::getTotalSize());
	pressAnyKeyToContinue();

	testHashOfObjects();
	checkTestMemory("Memory Leak/Allocation Test #2", 0, (int)ManageMemory::getTotalSize());
	pressAnyKeyToContinue();

	return 0;
}
