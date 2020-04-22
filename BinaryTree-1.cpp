// Copyright 2019, Bradley Peterson, Weber State University, all rights reserved. (4/2019)

#include <iostream>
#include <string>
#include <stack>
#include <sstream>
#include <cmath>
#include <cctype> 

//To prevent those using g++ from trying to use a library
//they don't have
#ifndef __GNUC__
#include <conio.h>
#else
#include <stdio.h>
#endif

using std::stack;
using std::istringstream;
using std::ostringstream;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::pow;

struct Node{//our node exists and it's data is only allowed to be a string 
	string data{};
	Node* llink{ nullptr };
	Node* rlink{ nullptr };
}; 


class TreeParser {
private:
	stack<string> mathStack;
	double castStrToDouble(string const &s); 
	string castDoubleToStr(const double d);
	void initialize();
	bool isDigit(char c);
	bool isOperator(char c);
	void processExpression(Node* ptr); 
	void computeAnswer(Node* ptr);
	void postOrderTraversal(Node*ptr); 
	void inOrderTraversal(Node*ptr); 
	Node*root{ nullptr };


protected: 
	string expression;
	int position;


public: 
	TreeParser();
	void displayParseTree();
	void processExpression(string &expression); 
	string computeAnswer();
	void inOrderTraversal(); 
	void postOrderTraversal();
	

};

bool TreeParser::isDigit(char c) {//checking if a number 
	if (isdigit(c)) {
		return true;//yay a digit, it's usable 
	}
	else {
		return false; //not a digit 
	}
}

bool TreeParser::isOperator(char c) {//use this to check if we have an operator symbole 
	if (c== '+'|| c=='-'||c=='/'||c=='*'||c=='^') {
		return true; //cool it is 
	}
	else {
		return false; //nope
	}
}

//public 
void TreeParser::processExpression(string &expression) {

	if (!expression.empty()) {//make sure there is soemting to work with
		this->expression = expression;//load in stuff sent in as our offical expression 
		this->position = 0;//starting at 0 
		Node * eh = new Node(); //make us a new node 
		eh->llink = nullptr; 
		eh->rlink = nullptr; 
		root = eh;//our root is now eh 
		processExpression(root);//call private expression using root 
	}
}

//private 
void TreeParser::processExpression(Node* ptr) {

	while (position < expression.length()) {//keep doing this for our whole string length

		
		if (expression.at(position)=='(') {

			Node *child = new Node(); //create node
			child->data = expression.at(position);//so no emptyness later 
			child->llink = nullptr; 
			child->rlink = nullptr;  
			ptr->llink = child; //make the node passed in point to our new node 
			position++;
			processExpression(ptr->llink); //recursion, aka repeat and jump back up 
		}

		else if(isdigit(expression.at(position))){//if we are dealing with a digit 
			string tempString; //make temp
			while (isdigit(expression.at(position))) {//if the digit is more than 1 in length
				tempString += expression.at(position);//add each digit on to our "number stirng" 
				position++;
			}
			ptr->data = tempString; //insert data 
			return; //bye
		}

		else if (isOperator(expression.at(position))) {//math will be done here
			ptr->data = expression.at(position);//just so it isnt empty later 
			Node *child = new Node(); //make a node
			child->llink = nullptr;
			child->rlink = nullptr;
			ptr->rlink = child; //make our new child be on our right 
			position++; 
			processExpression(ptr->rlink); //go again in case we have another # we might have to math 
		}
		else if (expression.at(position)==')') {//that's all folks 
			position++;
			return; 
		}
		else if (expression.at(position) == ' ') {//ignore space and just keep going 
			position++; 
			continue; 
		}
	}
}


void TreeParser::computeAnswer(Node* ptr) {
	if (ptr) {
		//postfix
		computeAnswer(ptr->llink);
		computeAnswer(ptr->rlink);

		//If the node's info is a digit
		if (isDigit(ptr->data.at(0))) {
			mathStack.push(ptr->data); 
		}

		//If the node's info is an operator at first position
		if (isOperator(ptr->data.at(0))) {
			double answer = 0; //safty= to 0

			double left = castStrToDouble(mathStack.top());
			mathStack.pop();
			double right = castStrToDouble(mathStack.top()); 
			mathStack.pop();


			if (ptr->data.at(0) == '+'){
				answer = left + right; 
			}
			else if (ptr->data.at(0) == '-') {
				answer = right - left; 
			}
			else if (ptr->data.at(0) == '/') {
				answer = right / left; 
			}
			else if (ptr->data.at(0) == '*') {
				answer = right * left; 
			}
			else if (ptr->data.at(0) == '^') {
				answer = pow(right, left); 
			}

			mathStack.push(castDoubleToStr(answer)); 

		}

	}

}

string TreeParser::computeAnswer() {
	computeAnswer(root);
	return mathStack.top();
}


TreeParser::TreeParser() {
	initialize();
}


void TreeParser:: inOrderTraversal() {
	inOrderTraversal(root); 
	cout << endl; 
}

//self explanitory 
void TreeParser::inOrderTraversal(Node*ptr) {
	if (ptr) {
		inOrderTraversal(ptr->llink);
		cout << ptr->data << " ";
		inOrderTraversal(ptr->rlink);
	}
}
//private 
void TreeParser::postOrderTraversal() {
	postOrderTraversal(root); 
	cout << endl; 
}
//self explanitory 
void TreeParser::postOrderTraversal(Node*ptr) {
	if (ptr) {
		postOrderTraversal(ptr->llink);
		postOrderTraversal(ptr->rlink);
		cout << ptr->data << " ";
	}
}





void TreeParser::initialize() {
	expression = "";
	position = 0;
	while (!mathStack.empty()) {
		mathStack.pop();
	}
}

double TreeParser::castStrToDouble(const string &s) {
	istringstream iss(s);
	double x;
	iss >> x;
	return x;
}

string TreeParser::castDoubleToStr(const double d) {
	ostringstream oss;
	oss << d;
	return oss.str();

}





void TreeParser::displayParseTree() {
	cout << "The expression seen using in-order traversal: ";
	inOrderTraversal();
	cout << endl;
	cout << "The expression seen using post-order traversal: ";
	postOrderTraversal();
	cout << endl;

}

void pressAnyKeyToContinue() {
	printf("Press any key to continue\n");

	//Linux and Mac users with g++ don't need this
	//But everyone else will see this message.
#ifndef __GNUC__
	_getch();
#else
	int c;
	fflush(stdout);
	do c = getchar(); while ((c != '\n') && (c != EOF));
#endif

}

// Copyright 2019, Bradley Peterson, Weber State University, all rights reserved. (4/2019)

int main() {

	TreeParser *tp = new TreeParser;

	string expression = "(4+7)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 11 as a double output

	expression = "(7-4)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 3 as a double output

	expression = "(9*5)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 45 as a double output

	expression = "(4^3)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 64 as a double output

	expression = "((2-5)-5)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display -8 as a double output

	expression = "(5*(6/2))";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 15 as a double output

	expression = "((1 + 2) * (3 + 4))";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 21 as a double output

	expression = "(543+321)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 864 as a double output

	expression = "((5*(3+2))+(7*(4+6)))";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 95 as a double output

	expression = "(((((3+12)-7)*120)/(2+3))^3)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display close to 7077888 as a double output 
															  //NOTE, it won't be exact, it will display as scientific notation!

	expression = "(((((9+(2*(110-(30/2))))*8)+1000)/2)+(((3^4)+1)/2))";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display close to 1337 as a double/decimal output

	pressAnyKeyToContinue();
	return 0;
}
