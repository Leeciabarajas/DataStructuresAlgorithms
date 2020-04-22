//Copyright 2019, Bradley Peterson, Weber State University, all rights reserved. (03/2019)

#include <cstdio>
#include <random>
#include <thread>
#include <mutex> 
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <sstream>


using std::cout;
using std::endl;
using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::string;
using std::stringstream;
using std::mutex;
using std::thread;

//*** Prototypes ***
void recQuickSort(vector<unsigned long>& arr, int first, int last);
void printList();
void pressAnyKeyToContinue();

class ManyBuckets;

//***GLOBAL VARIABLES***
unique_ptr<unsigned long[]> list;
unique_ptr<ManyBuckets> globalBuckets;

unsigned int listSize{ 0 };
unsigned int numBuckets{ 0 };
unsigned int currentWorkUnit{ 0 };
const unsigned long ULONGMAX = 4294967295;
//const unsigned long ULONGMAX = 100;

// TODO for multithreading assignment: Create a global mutex object

mutex myMutex;
unsigned int currentBucket{ 0 };

class ManyBuckets { 
public:
	ManyBuckets() {}; 
	ManyBuckets(const unsigned int numBuckets);
	~ManyBuckets() {}
	void addItem(unsigned long item);
	unsigned int getNumBuckets() const;
	unsigned int getNumItemsInABucket(const unsigned int bucket) const;
	vector<unsigned long>& getBucket(const unsigned int bucket);
	void printAllBuckets() const;

private:
	vector< vector<unsigned long> > arr;
	unsigned int buckets;

};

//*** Provide methods for the bucket class ***
ManyBuckets::ManyBuckets(const unsigned int numBuckets) {
	arr.resize(numBuckets);
	buckets = numBuckets;
}

unsigned int ManyBuckets::getNumBuckets() const {
	return buckets;
}

unsigned int ManyBuckets::getNumItemsInABucket(const unsigned int bucket) const {
	return arr[bucket].size();
}

void ManyBuckets::printAllBuckets() const {
	//Displays the contents of all buckets to the screen.

	printf("******\n");
	for (unsigned int i = 0; i < numBuckets; i++) {
		printf("bucket number %d\n", i);
		for (unsigned int j = 0; j < arr[i].size(); j++) {
			printf("%08x ", arr[i][j]);
			//printf("%d \n", arr[i][j]); 
		}
		printf("\n");
	}
	printf("\n");
}

vector<unsigned long>& ManyBuckets::getBucket(const unsigned int bucket) {
	return arr[bucket];
}

void ManyBuckets::addItem(unsigned long item) {
	// TODO: Put the item in the appropraite bucket.
	//unsigned long range = ULONGMAX / numBuckets + 1;
	
	unsigned long here = (item / (numBuckets == 1 ? ULONGMAX : ((ULONGMAX / numBuckets) + 1)));
		arr[here].push_back(item);

}


// TODO:  For this function, loop over every item and call addItem
void placeIntoBuckets() {
	for (int i = 0; i < listSize; i++) {
		globalBuckets->addItem(list[i]); 
	}
}

// TODO: Have a single thread go through each bucket and sort each bucket using recQuickSort  
void sortEachBucket() {

	for (int i = 0; i < numBuckets; i++) {
		recQuickSort(globalBuckets->getBucket(i), 0, globalBuckets->getNumItemsInABucket(i));
	}

}

// TODO: Set up a mutithreaded consumer model.  The details will be described in class.
// The goal is that the problem has already been partitioned into many small buckets (specifically, numBuckets buckets)
// A thread checks against a global counter which bucket needs work next, that thread then increments 
// the global counter and proceeds to work on that bucket by simply using recQuickSort on that bucket.  
// Note that the global counter should be protected against race conditions...hint hint...
void sortEachBucketMultithreaded() {
	unsigned int mybucket{ 0 };
	do {
		myMutex.lock();
		mybucket = currentBucket;
		currentBucket++;
		myMutex.unlock();

		if (mybucket >= numBuckets) {
			return;
		}

		recQuickSort(globalBuckets->getBucket(mybucket), 0, globalBuckets->getNumItemsInABucket(mybucket));

	} while (true);

} 



// TODO: Copy each bucket back out to the original list[] array
// Loop for the amount of buckets
// Get the individual bucket out of the globalBuckets
// Copy all items from that individual bucket into the list array.
void combineBuckets() {

	int index = 0; 
	for (int i = 0; i < numBuckets; i++) {
			auto bucket{ globalBuckets->getBucket(i) };
			unsigned int limit{ globalBuckets->getNumItemsInABucket(i) };

			for (int f = 0; f < limit; f++) {
				list[index] = bucket[f];
				index++; 
			}
		}
	
}

void bucketSort(bool displayOutput, bool useMultiThreading) {

	//For the upcoming homeowork assignment, it will help you the most to split your work into these three functions.  
	// placeIntobuckets(), sortEachBucket()/sortEachBucketMultithreaded(), combineBuckets()
	placeIntoBuckets();

	if (displayOutput) {
		printf("Displaying each bucket's contents before sorting: \n");
		globalBuckets->printAllBuckets();
	}

	unsigned int numThreadsToUse{ 0 };
	currentWorkUnit = 0; //Reset the currentWorkUnit variable.

	if (useMultiThreading) {
		//Find out how many threads are supported
		unsigned int threadsSupported = std::thread::hardware_concurrency();
		if (threadsSupported == 1 && numBuckets > 1) {
			numThreadsToUse = 2;
		}
		else if (numBuckets < threadsSupported) {
			numThreadsToUse = numBuckets;
		}
		else {
			numThreadsToUse = threadsSupported;
		}
		printf("Starting sort on %d threads\n", numThreadsToUse);
	}
	else {
		numThreadsToUse = 1;
	}

	if (!useMultiThreading) {
		// For single threaded sorts, you don't need to do anything special right here.  
		printf("Calling single threaded sortEachBucket()\n");
		sortEachBucket();

	}
	else {
		// TODO: create threads and launch a function to get your mulithreaded work started.
		// That function should be responsible for calling sortEachBucket
		printf("Calling multi threaded sortEachBucket() with %d threads\n", numThreadsToUse);
		currentBucket = 0;
		thread* myThreads = new thread[numThreadsToUse]; 

		for (int i = 0; i < numThreadsToUse; i++) {
			myThreads[i] = thread(sortEachBucketMultithreaded);
		}
		for (int i = 0; i < numThreadsToUse; i++) {
			myThreads[i].join();
		}

		delete[] myThreads;

	}
	//sortEachBucket();
	combineBuckets();

	if (displayOutput) {
		printf("Displaying each bucket's contents after sorting: \n");
		globalBuckets->printAllBuckets();
		pressAnyKeyToContinue();
		printf("Displaying what is hopefully a sorted array: \n");
		printList(); //See if it's all sorted.
	}
}

//***Functions that help our bucket sort work.***
int partition(vector<unsigned long>& arr, int first, int last) {
	unsigned long pivot;
	int index, smallIndex;

	unsigned long temp;

	//Take the middle value as the pivot.
	//swap(first, (first + last) / 2);
	pivot = arr[first];
	smallIndex = first;
	for (index = first + 1; index < last; index++) {
		if (arr[index] < pivot) {
			smallIndex++;
			//swap the two
			temp = arr[smallIndex];
			arr[smallIndex] = arr[index];
			arr[index] = temp;
		}
	}

	//Move pivot into the sorted location
	temp = arr[first];
	arr[first] = arr[smallIndex];
	arr[smallIndex] = temp;

	//Tell where the pivot is
	return smallIndex;

}

void recQuickSort(vector<unsigned long>& arr, int first, int last) {
	//first is the first index
	//last is the one past the last index (or the size of the array
	//if first is 0)

	if (first < last) {
		//Get this sublist into two other sublists, one smaller and one bigger
		int pivotLocation = partition(arr, first, last);
		recQuickSort(arr, first, pivotLocation);
		recQuickSort(arr, pivotLocation + 1, last);
	}
}

void createList() {
	list = std::make_unique<unsigned long[]>(listSize);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<unsigned long> dis(0, ULONGMAX);

	for (unsigned int i = 0; i < listSize; i++) {
		list[i] = dis(gen);
	}
}

void printList() {
	for (unsigned int i = 0; i < listSize; i++) {
		//cout << list[i] << " ";
		printf("%08x ", list[i]);
		//printf("%d \n", list[i]);
	}
	printf("\n");
}

void verifySort(unique_ptr<unsigned long[]>& arr, unsigned int arraySize, std::chrono::duration<double, std::milli>& diff, const string& sortTest) {
	double val = diff.count();
	for (unsigned int i = 1; i < arraySize; i++) {
		if (arr[i] < arr[i - 1]) {
			printf("------------------------------------------------------\n");
			printf("SORT TEST %s\n", sortTest.c_str());

			if (val != 0.0) {
				printf("Finished bucket sort in %1.16lf milliseconds\n", diff.count());
			}
			printf("ERROR - This list was not sorted correctly.  At index %d is value %08X.  At index %d is value %08X\n", i - 1, arr[i - 1], i, arr[i]);
			printf("------------------------------------------------------\n");


			return;
		}
	}
	printf("------------------------------------------------------\n");
	printf("SORT TEST %s\n", sortTest.c_str());
	if (val != 0.0) {
		printf("Finished bucket sort in %1.16lf milliseconds\n", diff.count());
	}
	printf("PASSED SORT TEST %s - The list was sorted correctly\n", sortTest.c_str());
	printf("------------------------------------------------------\n");
}


void pressAnyKeyToContinue() {
	printf("Press Enter key to continue\n");
	std::cin.get();

}

int main() {

	std::chrono::duration<double, std::milli> diff{ 0 };
	double baselineTime{ 9999999.0 };
	double bestMultiThreadedTime{ 9999999.0 };
	int bestMultiThreadedBuckets{ 0 };
	double bestSingleThreadedTime{ 9999999.0 };
	int bestSingleThreadedBuckets{ 0 };


	//Set the listSize, numBuckets, and numThreads global variables.  
	listSize = 100;

	unsigned int threadsSupported = std::thread::hardware_concurrency();
	if (threadsSupported == 1) {
		printf("You are on a machine that only supports 1 thread!\nThat is too bad, you won't see anything exciting in this assignment regarding better timings.\nSimulating 2 threads instead");
	}
	else {
		printf("Your machine supports up to %d threads\n", threadsSupported);
	}

	bool useMultiThreading{ false };
	for (int i = 0; i <= 1; i++) {
		if (useMultiThreading) {
			printf("Starting multithreading tests on small lists\n");
			pressAnyKeyToContinue();
		}
		else {
			printf("Starting singlethreaded tests on small lists\n");
			pressAnyKeyToContinue();
		}

		numBuckets = 2;
		createList();
		globalBuckets = std::make_unique<ManyBuckets>(numBuckets);
		printf("\nStarting bucket sort for listSize = %d, numBuckets = %d\n", listSize, numBuckets);
		// printf("Displaying the unsorted list array:\n");
		// printList(); //useful for debugging small amounts of numbers.  
		pressAnyKeyToContinue();
		bucketSort(true, useMultiThreading);
		verifySort(list, listSize, diff, "2 buckets");
		pressAnyKeyToContinue();


		numBuckets = 4;
		createList();
		printf("\nStarting bucket sort for listSize = %d, numBuckets = %d\n", listSize, numBuckets);
		globalBuckets = std::make_unique<ManyBuckets>(numBuckets);
		pressAnyKeyToContinue();
		bucketSort(true, useMultiThreading);
		verifySort(list, listSize, diff, "4 buckets");
		pressAnyKeyToContinue();

		printf("\n\n*** Note, remember to run on RELEASE mode, not DEBUG mode.  Release mode is MUCH faster. *** \n\n");
		pressAnyKeyToContinue();
		useMultiThreading = !useMultiThreading; //Flip it from false to true.
	}
	listSize = 4000000;
	numBuckets = 1;
	createList();
	globalBuckets = std::make_unique<ManyBuckets>(numBuckets);
	auto start = std::chrono::high_resolution_clock::now();
	bucketSort(false, false);
	auto end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	baselineTime = diff.count();
	verifySort(list, listSize, diff, "4000000 items in 1 bucket with 1 thread - BASELINE");

	useMultiThreading = false;
	for (int i = 0; i <= 1; i++) {

		if (useMultiThreading) {
			printf("Starting multithreading tests\n");
			pressAnyKeyToContinue();
		}
		else {
			printf("Starting singlethreaded tests\n");
			pressAnyKeyToContinue();
		}
		for (numBuckets = 2; numBuckets <= 1024; numBuckets *= 2) {
			listSize = 4000000;
			createList();
			globalBuckets = std::make_unique<ManyBuckets>(numBuckets);
			start = std::chrono::high_resolution_clock::now();
			bucketSort(false, useMultiThreading);
			end = std::chrono::high_resolution_clock::now();
			diff = end - start;
			if (useMultiThreading && (diff.count() < bestMultiThreadedTime)) {
				bestMultiThreadedTime = diff.count();
				bestMultiThreadedBuckets = numBuckets;
			}
			else if (!useMultiThreading && (diff.count() < bestSingleThreadedTime)) {
				bestSingleThreadedTime = diff.count();
				bestSingleThreadedBuckets = numBuckets;
			}
			stringstream ss;
			ss << listSize << " items in " << numBuckets << " buckets";
			verifySort(list, listSize, diff, ss.str());
		}

		useMultiThreading = !useMultiThreading; //Flip it from false to true.
	}
	printf("\n-----------------------------------------------------------\n");
	printf("              FINAL RESULTS                      \n");
	printf("The baseline (quicksort on 1 thread/1 bucket):  completed in %g ms\n", baselineTime);
	printf("The best singlethreaded result:     %d buckets completed in %g ms\n", bestSingleThreadedBuckets, bestSingleThreadedTime);
	printf("The best multithreaded result:      %d buckets completed in %g ms\n", bestMultiThreadedBuckets, bestMultiThreadedTime);
	printf("\n-----------------------------------------------------------\n");

	pressAnyKeyToContinue();
	return 0;
}
