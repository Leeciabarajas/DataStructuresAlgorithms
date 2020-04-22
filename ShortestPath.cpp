// Copyright 2019, Bradley Peterson, Weber State University, all rights reserved.

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>

// To prevent those using g++ from trying to use a library
// they don't have
#ifndef __GNUC__
#include <conio.h>
#endif

using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::ifstream;
using std::vector;
using std::string;
using std::istringstream;
using std::unique_ptr;
using std::make_unique;

struct graphEdge;

// The collection of edge objects
vector<graphEdge> edges;
int numEdges;
int numNodes;

int* smallestWeight{ nullptr }; 
// TODO: Declare three arrays (graphWeights, columns, rows)
int * graphWeights;
int * columns; 
int * rows; 


// Here is where you create your global dynamic arrays
const int LARGE_NUMBER = 99999999;

// The class for an edge object.  
struct graphEdge {
public:
	int sourceNode;
	int destNode;
	int weight;
	graphEdge() {};
	graphEdge(int sourceNode, int destNode, int weight) {
		this->sourceNode = sourceNode;
		this->destNode = destNode;
		this->weight = weight;
	}
};

void pressAnyKeyToContinue() {
	printf("Press any key to continue\n");

	// Linux and Mac users with g++ don't need this
	// But everyone else will see this message.
#ifndef __GNUC__
	_getch();
#else
	int c;
	fflush(stdout);
	do c = getchar(); while ((c != '\n') && (c != EOF));
#endif
}

int getWeight(int sourceIndex, int destIndex) {//check if node 1->node2
	// go through your arrays to see if there is an edge between sourceIndex and endIndex, 
	// and if so, return the cost.  if not, return 
	for (int i = rows[sourceIndex]; i < rows[sourceIndex + 1]; i++ ) {
		if (destIndex == columns[i]) {
			return graphWeights[i]; 
		}	
	}

	return LARGE_NUMBER;


}
void shortestPath(int vertex) {

	// TODO: Fix the book's code so that it works without data

	// The argument is the vertex to search from
	smallestWeight = new int[numNodes];

	for (int j = 0; j < numNodes; j++) {
		// intialization step
		//smallestWeight[j] = weights[vertex][j];
		smallestWeight[j] = getWeight(vertex, j);
	}
	// continue initializing
	bool* weightFound = new bool[numNodes];


	for (int j = 0; j < numNodes; j++) {
		weightFound[j] = false;
	}

	// The node we're at we assume we can get to with a cost of zero.
	weightFound[vertex] = true;
	smallestWeight[vertex] = 0;

	// For every node...
	for (int i = 0; i < numNodes - 1; i++) {
		if (i % 100 == 0) {
			cout << "Looking up shortest path for " << i << " of " << numNodes - 1 << " amount of nodes" << endl;

		}
		else if (i == numNodes - 2) {
			cout << "Looking up the shortest path for the last node" << endl;
		}
		int minWeight = LARGE_NUMBER;
		int v = -1;

		// Of the nodes who haven't been marked as completed,
		// or in other words, nodes which we aren't sure if we've found the
		// smallest path weight
		// Of those ndoes...find the node with the smallest current weight.
		for (int j = 0; j < numNodes; j++) {
			if (!weightFound[j]) {
				if (smallestWeight[j] < minWeight) {
					v = j;
					minWeight = smallestWeight[v];
				}
			}
		}

		// Ignore non connected nodes.
		if (v != -1) {

			//When I drew the red line on my notes.  
			weightFound[v] = true;
			// Now that we've found a new shortest possible weight (at node v)
			// look at all of v's neighborly costs, and see if we can get to v's neighbors
			// using v, at a better weight than what we already know.
			for (int j = 0; j < numNodes; j++) {
				if (!weightFound[j]) { 
					if (minWeight + getWeight(v, j) < smallestWeight[j]) {
						smallestWeight[j] = minWeight + getWeight(v,j);
					}
				}
			}
		}
	}

	// TODO: Clean up the weightFound array, but do not clean up the smallestWeight array (that's needed for later user output)
}


// Some functions I wrote to help the stable sort know what to sort against.  
// You don't need to worry about these or modify them.  
bool compareFirstColumn(const graphEdge& x, const graphEdge& y) {
	if (x.sourceNode < y.sourceNode) {
		return true;
	}
	else {
		return false;
	}
}
bool compareSecondColumn(const graphEdge& x, const graphEdge& y) {
	if (x.destNode < y.destNode) {
		return true;
	}
	else {
		return false;
	}
}

void testRun() {
	// This loads the same values found in the book:
	edges.push_back(graphEdge(0, 1, 16));
	edges.push_back(graphEdge(0, 3, 2));
	edges.push_back(graphEdge(0, 4, 3));
	edges.push_back(graphEdge(1, 2, 5));
	edges.push_back(graphEdge(2, 1, 3));
	edges.push_back(graphEdge(3, 1, 12));
	edges.push_back(graphEdge(3, 4, 7));
	edges.push_back(graphEdge(4, 1, 10));
	edges.push_back(graphEdge(4, 2, 4));
	edges.push_back(graphEdge(4, 3, 5));


	numNodes = 5;
	numEdges = 10;

}


void readFile() {
	ifstream inFile("rome99.gr");
	int counter = 0;
	int largestNode = 0;
	char throwaway;
	if (!inFile.good())
	{
		cerr << "The file wasn't found.  For Visual Studio users, make sure it's in the same directory as your solution's .vcxproj file." << endl;
		pressAnyKeyToContinue();
		exit(-1);
	}
	else {
		string line;
		graphEdge edge;
		while (getline(inFile, line)) {


			if (line.at(0) == 'a' && line.at(1) == ' ') {
				if (counter % 10000 == 0) {
					cout << "Reading edge # " << counter << endl;
				}
				// this line is one we keep, read in the data
				istringstream iss(line);
				iss >> throwaway >> edge.sourceNode >> edge.destNode >> edge.weight;
				if (edge.sourceNode > largestNode) {
					largestNode = edge.sourceNode;
				}

				if (edge.destNode > largestNode) {
					largestNode = edge.destNode;
				}
				edges.push_back(edge);
				counter++;
			}
		}
		numNodes = largestNode;
		numEdges = counter;


		// Create a zero node with an edge that points to itself with a weight of 0.
		// The file node data starts at node #1, so we want to make 
		// everything clean by letting edge 1 take index 1 in our graphWeights array,
		// and this zero node can take index 0.  
		graphEdge zeroEdge(0, 0, 0);
		edges.push_back(zeroEdge);
		numNodes++;
		numEdges++;
		// Because we started a zero node, increase the numNodes by 1., 
		cout << "Finished reading " << numNodes << " nodes and " << numEdges << " edges." << endl;
	}
}

void createCsrArrays() {

	// TODO:
	// Take the collection of graphEdge objects in our edges vector, and convert them into CSR format
	// For example, this graph form figure 12-8 would look like this as a 2-D array:
	// 0         16        99999999  2         3
	// 99999999  0         5         99999999  99999999
	// 99999999  3         0         99999999  99999999
	// 99999999  12        99999999  0         7
	// 99999999  10        4         5         0
	// But we don't have a 2D array in this assignment.  We have a vector/collection object.
	// In this vector object, edges are simply listed one edge at a time, so the 0th
	// object should contain (0, 1, 16) for 0 goes to 1 with a cost of 16.

	// So use the vector and turn the data into CSR format, we will have it look like this:

	// Note: This only takes about 15 lines of code.  But you need to carefully think this out.  Do not
	// be discouraged when your first few attempts don't work.
	// graphWeights: 16 2  3  5  3  12 7 10 4  5
	// columns:      1  3  4  2  1  1  4 1  2  3
	// row:          0  3  4  5  7  10

	// TODO: You should use three global arrays, with names of graphWeights, columns, and row.
	graphWeights = new int[numEdges];
	columns =new int[numEdges];
	rows = new int[numNodes+1];

	rows[0] = 0; 
	for (int i = 0; i < numEdges; i++) {
		graphWeights[i] =edges[i].weight; 
		columns[i] = edges[i].destNode;

		if (i >0 && edges[i].sourceNode != edges[i - 1].sourceNode) {//
			rows[edges[i].sourceNode] = i; //incramention varible 
		}
	}
	rows[numNodes] = numEdges; 
}

void deleteArrays() {
	// TODO:
	// Delete thre three arrays you created in createCsrArrays()
	delete[] graphWeights;
	delete[]columns; 
	delete[]rows; 
	// Also delete the smallestWeight array
	delete[]smallestWeight; 
}

int main() {

	cout << "Would you like to do: " << endl << "1. Test run" << endl << "2. Full run" << endl << "Enter your selection: ";
	int selection;
	cin >> selection;
	if (selection == 1) {
		testRun();
	}
	else if (selection == 2) {
		readFile();

	}

	// The collection of edge objects is just an unsorted collection.  
	// So use a stable sort to sort by first column and second column so 
	// they are in a clean order ready to go into CSR format.
	stable_sort(edges.begin(), edges.end(), compareSecondColumn);
	cout << "Halfway done sorting..." << endl;
	stable_sort(edges.begin(), edges.end(), compareFirstColumn);
	cout << "Finished sorting..." << endl;

	createCsrArrays();

	if (selection == 1) {
		cout << "Test run debugging.  Your CSR arrrays are: " << endl;
		cout << "weights: ";
		for (int i = 0; i < numEdges; i++) {
			cout << graphWeights[i] << " ";
		}
		cout << endl;
		cout << "columns: ";
		for (int i = 0; i < numEdges; i++) {
			cout << columns[i] << " ";
		}
		cout << endl;
		cout << "row: ";
		for (int i = 0; i < numNodes + 1; i++) {
			cout << rows[i] << " ";
		}
		cout << endl;
	}


	int userNode = 0;
	cout << "Which node would you like to search from: ";
	cin >> userNode;

	// call our shortest path algorithm
	shortestPath(userNode);

	do {
		cout << "Which node do you want to see weights for (-1 to exit): ";
		cin >> userNode;

		if (userNode == -1) {
			break;
		}
		if (userNode >= 0 && userNode < numNodes) {
			cout << "For node " << userNode << " the cost is " << smallestWeight[userNode] << endl;
		}
		else {
			cerr << "Error: That's no node with that ID!" << endl;
		}
	} while (true);


	deleteArrays();

	pressAnyKeyToContinue();
}
