//
//  main.cpp
//  8Puzzle
//
//  Created by Atticus Wright and Patrick Lindsay on 3/5/12.
//  Copyright (c) 2012 University of North Alabama. All rights reserved.
//

#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <functional>
using namespace std;

/*global variables************************************/
/*****************************************************/
#define PUZZLE_DIM 3
#define PUZZLE_SIZE 9

// Can change the above dimensions to create a larger board, but initGoal will
// have to be modified to create a proper goal state


/*boardNode Class*************************************/
/*****************************************************/
class boardNode {
public:
    boardNode();
    boardNode(int size);
    int getfOfN(void) const;
    //bool operater <(boardNode &node);
    
    vector<int> board;
    int fOfN;
    int hOfN;
    int gOfN;
    int lastMove;
    boardNode* parentBoard;
};

/*boardNode Class functions***************************/
/*****************************************************/

//boardNode()
//==============
//Default Constructor
boardNode::boardNode(){
    board.resize(PUZZLE_SIZE+1);
    fOfN = 0;
    hOfN = 0;
    gOfN = 0;
    lastMove = -1;
    parentBoard = NULL;
    
}

//boardNode(int size)
//====================
//Constructor with specified size
boardNode::boardNode(int size){
    board.resize(PUZZLE_SIZE+1);
    fOfN = 0;
    hOfN = 0;
    gOfN = 0;
    lastMove = -1;
    parentBoard = NULL;
}

//getfOfN
//========
//Getter for the boardNode's f(n)
int boardNode::getfOfN(void) const{
        return this->fOfN;
}


/*Comparison class for boardNode pointers*************/
/*****************************************************/


// Based on code found at cplusplus.com in relation to priority queues
class comparison : public std::binary_function<boardNode*, boardNode*, bool>{
public:
    bool operator() (const boardNode* lhs, const boardNode* rhs) const
    {
        
        return (lhs->getfOfN() > rhs->getfOfN() );
    }
};
/*
// Comparison class for boardNodes
class comparison1 {
public:
    bool operator() (const boardNode &lhs, const boardNode &rhs) const
    {
        return (lhs.fOfN > rhs.fOfN);
    }
};*/

/*moves***********************************************/
/*****************************************************/
//Enumerated types for possible moves
enum moves {U, R, D, L};

/*****************************************************/
/*******************End of Headers********************/
/*****************************************************/


/*Function Prototypes********************************/
void readInPuzzle(boardNode& startPuzzle);
vector<int> initGoal();
void printBoard(boardNode* graph);
void aStar(boardNode* rootBoard, vector<int>& goal);
int manhattan(boardNode* current, vector<int>& goal);
int getGoalIndex(vector<int>& goal, int key);
bool isLegalMove(boardNode* current, int move, int blankIndex);
boardNode* createState(boardNode& current, int move, int blankIndex);
void printGoal(vector<int>& goal);
bool checkGoal(boardNode* current, vector<int>& goal);
void printSolution(boardNode* current);
/****************************************************/




int main ()
{
	vector<int> goal(PUZZLE_SIZE+1);
	goal = initGoal();
    
    cout << "Goal state: " << endl;
    printGoal(goal);
    cout << endl;
    
	boardNode startPuzzle(PUZZLE_SIZE+1); // Will hold initial puzzle configuration
    vector<boardNode> closed;
    
    
    // Read in initial puzzle from file
    readInPuzzle(startPuzzle);
    boardNode* startState = &startPuzzle;
    
    cout << "Start state: " << endl;
    printBoard(startState); // Testing purposes
    
    startState->hOfN = manhattan(startState, goal);
    startState->fOfN = startState->gOfN + startState->hOfN;
    cout << "Manhattan distance = " << startState->hOfN << endl << endl;
    
    
    aStar(startState, goal);    
	
    return 0;
}

/*Function Definitions********************************/
/*****************************************************/

//initGoal
//=========
//Creates a goal vector to compare to, and fills it with our goal state.
vector<int> initGoal(){
	
    vector<int> goal(PUZZLE_SIZE+1);
	goal[1]=1;
	goal[2]=2;
	goal[3]=3;
	goal[4]=8;
	goal[5]=0;
	goal[6]=4;
	goal[7]=7;
	goal[8]=6;
	goal[9]=5;
    return goal;
}

//aStar
//======
//Finds the optimal path from the rootBoard to the Goal.
/*******	Our A* algorithm was adapted from the algorithm provided with 
			the textbook. Credit goes to "Artificial Intelligence: A 
			Systems Approach" by M. Tim Jones.							*/
void aStar(boardNode* rootBoard, vector<int>& goal){
    
    // Priority Queue of boardNode pointers for open list
    priority_queue<boardNode*, vector<boardNode*>, comparison > open;
    
    // Vector of boardNodes for closed list
    vector<boardNode*> closed;
    
    // Pointers for aStar algorithm
    boardNode *current, *next, *previous;
    
    bool foundGoal;				// Bool for whether goal state has been found
    bool legalMove;				// Bool for wheter a tile move is legal 
    bool containsNext = false;	// Used as a control flag in aStar
    int  blankIndex;			// Holds position of blank relative to vector index
    int searches = 0;			// Keeps track of number of nodes expanded; used to detect
								// an unsolveable starting puzzle configuration
    
    
    open.push(rootBoard); // Adds start state to the open list
    
    while (open.size() != 0 && searches < 4000) {
        
        // get the best board on open and then remove it
        current = open.top();
        open.pop();
        
        // Place current onto closed
        closed.push_back(current);
        
        // Check to see if current is the goal
        foundGoal = checkGoal(current, goal);
        
        // Expand child nodes if goal not found
        if (foundGoal == false) {
            
            blankIndex = getGoalIndex(current->board, 0); // Get the position of the blank
            
            // For all possible moves
            for (int i = U; i <= L; i++) {
                
                // Check to see if the move is legal
                legalMove = isLegalMove(current, i, blankIndex);
                
                // If it is legal
                if (legalMove == true){
                    
                    //  Create a new node with the legal move
                    next = createState(*current, i, blankIndex);
                    next->hOfN = manhattan(next, goal);          // compute manhattan distance
                    next->fOfN = next->hOfN + next->gOfN;        // compute f(n)
                    searches++; 
        
                    containsNext = false;
                    
                    // Check to see if closed contains next
                    for (int i = 0; i < closed.size(); i++) {
                        if (closed[i]->board == next->board) {
                            containsNext = true;  
                            previous = closed[i]; // Set previous to state in closed matching next
                            if (next->fOfN < previous->fOfN) {
                                closed.erase(closed.begin() + i); // Remove previous from closed
                                open.push(next); // Insert next into open
                            }
                        }
                    }
                    
                    // If closed did not contain next
                    if (containsNext == false) {
                        open.push(next); // Add next to open
                    }
                }
            }
        }
        else
            break;
    }
    if (foundGoal == true) 
        printSolution(current); // Print out the solution
    else
        cout << "Starting puzzle configuration has no solution." << endl;
}

//Manhattan
//==========
//Returns the Manhattan Distance from the current tile to the goal tile
int manhattan(boardNode* current, vector<int>& goal){
    int manhatt = 0; // manhattan distance total
    
    // Indexes for current node and goal state relative to vector positions
    int currIndex = 1; 
    int goalIndex = 0;
    
    // x and y values for current node tile
    int x1 = 0;
    int y1 = 0;
    
    // x and y values for goal node tile
    int x2 = 0;
    int y2 = 0;

    int val1 = 0; // holds difference of x1 and x2
    int val2 = 0; // holds difference of y1 and y2
    
    for (int i = 0; i < PUZZLE_SIZE; i++) {
        if (current->board[currIndex] != goal[currIndex] && current->board[currIndex] != 0) {
            x1 = i % PUZZLE_DIM;  // We mod to get the column value for x1
            y1 = i / PUZZLE_DIM;  // We use integer division to get the row value for y1
            
            goalIndex = getGoalIndex(goal, current->board[currIndex]);
            
            // mod and integer divide as above
            x2 = goalIndex % PUZZLE_DIM;
            y2 = goalIndex / PUZZLE_DIM;
            
            // Next three lines peform manhattan distance calculation and total it
            val1 = x1 - x2;
            val2 = y1 - y2;
            manhatt += abs(val1) + abs(val2);
        }
        currIndex++;
    }
    
    return manhatt;
}

//printSolution
//==============
//Prints every state from start to goal
void printSolution(boardNode* current){
    vector<boardNode*> solution;
    
    while (current->parentBoard != NULL) {
        solution.push_back(current);
        current = current->parentBoard;
        
    }
    solution.push_back(current);
    cout<<"-------------------------------------\n";
    cout << "The optimal solution is: " << endl;
    for (int i = (int)solution.size()-1; i >= 0; i--) {
        printBoard(solution[i]);
        cout << endl;
    }
    
    cout << "Number of moves: " << solution.size()-1 << endl;
}

//isLegalMove
//============
//Analyzes a given move and current position and returns a bool value indicating whether or not
//the move is legal in relation to the board and the last move of the parent node
bool isLegalMove(boardNode* current, int move, int blankIndex){
    bool legal = true;;
    int blankx1 = blankIndex % PUZZLE_DIM;
    int blanky1 = blankIndex / PUZZLE_DIM;
    int lastmove = current->lastMove;
    
    switch (move) {
        case U:
            if (blanky1 == 0 || lastmove == D) {
                legal = false;
            }
            break;
        case R:
            if (blankx1 == (PUZZLE_DIM-1) || lastmove == L) {
                legal = false;
            }
            break;
        case D:
            if (blanky1 == (PUZZLE_DIM-1)  || lastmove == U) {
                legal = false;
            }
            break;
        case L:
            if (blankx1 == 0 || lastmove == R) {
                legal = false;
            }
            break;
        default:
            break;
    }
    
    return legal;
}

//createState
//============
//Creates all possible states, based on the current state and legal moves
boardNode* createState(boardNode& current, int move, int blankIndex){
    boardNode* temp = new boardNode(PUZZLE_SIZE+1);
    temp->board = current.board;
    temp->gOfN = current.gOfN + 1;
    temp->parentBoard = &current;
    temp->lastMove = move;
    
    // Subtracting and adding values may seem odd; due to nature of blankIndex
    switch (move) {
        case U:
            temp->board[blankIndex+1] = temp->board[(blankIndex+1)-PUZZLE_DIM];
            temp->board[(blankIndex+1)-PUZZLE_DIM] = 0;
            break;
        case R:
            temp->board[blankIndex+1] = temp->board[blankIndex+2];
            temp->board[blankIndex+2] = 0;
            break;
        case D:
            temp->board[blankIndex+1] = temp->board[(blankIndex+1)+PUZZLE_DIM];
            temp->board[(blankIndex+1)+PUZZLE_DIM] = 0;
            break;
        case L:
            temp->board[blankIndex+1] = temp->board[blankIndex];
            temp->board[blankIndex] = 0;
            break;
        default:
            break;
    }
    return temp;    
}

//getGoalIndex
//=============
//Returns the index of the goal for any given tile
int getGoalIndex(vector<int>& goal, int key){
    int index = 0;
    
    for (int i = 1; i < PUZZLE_SIZE+1; i++) {
        if (goal[i] == key) {
            index = i;
        }
    }
    return index-1; // subtract by one to account for vector size of 10
}
    
//readInPuzzle
//=============
//Reads a start state from a text file.

void readInPuzzle(boardNode& startPuzzle){
    ifstream fin;
    int val;
    int counter = 1;
    
    fin.open("8puzzle.txt");
    if (fin.fail()) {
        cout << "Input file opening failed.\n";
		exit(1);
    }
    
    while (! fin.eof()) {
        fin >> val;
        startPuzzle.board[counter] = val;
        counter++;
    }
	fin.close();
}

//printBoard
//===========
//Prints the given boardNode pointer
void printBoard(boardNode* graph)
{
    
    for (int i = 0; i < PUZZLE_DIM; i++) {
        if (PUZZLE_DIM > 3) {
            cout << "+----";
        }
        else
            cout << "+---";
    }
    cout << "+" << endl << "|";
	for(int i = 1; i <= PUZZLE_SIZE; i++){
        if (graph->board[i] < 10 && PUZZLE_DIM > 3) { // Handles spacing based on size of value being printed
            cout << "  ";
        }
        else
            cout << " ";
        cout << graph->board[i] << " |";
		if (i % PUZZLE_DIM == 0) { 
			cout << endl;
            for (int i = 0; i < PUZZLE_DIM; i++) {
                if (PUZZLE_DIM > 3) {
                    cout << "+----";
                }
                else
                  cout << "+---";
            }
            cout << "+" << endl;
            if (i != PUZZLE_SIZE)
                cout << "|";
		}
	}
    
	switch(graph->lastMove){
		case U:
			cout<<"Moved: "<<"UP\n"; break;
		case R:
			cout<<"Moved: "<<"RIGHT\n"; break;
		case L:
			cout<<"Moved: "<<"LEFT\n"; break;
		case D:
			cout<<"Moved: "<<"DOWN\n"; break;
		default:
			break;
	}
}


//printGoal
//==========
//Prints the Goal Board
void printGoal(vector<int>& goal){
    for (int i = 0; i < PUZZLE_DIM; i++) {
        if (PUZZLE_DIM > 3) {
            cout << "+----";
        }
        else
            cout << "+---";
    }
    cout << "+" << endl << "|";
	for(int i = 1; i <= PUZZLE_SIZE; i++){
        if (goal[i] < 10 && PUZZLE_DIM > 3) { // Handles spacing based on size of value being printed
            cout << "  ";
        }
        else
            cout << " ";
        cout << goal[i] << " |";
		if (i % PUZZLE_DIM == 0) { 
			cout << endl;
            for (int i = 0; i < PUZZLE_DIM; i++) {
                if (PUZZLE_DIM > 3) {
                    cout << "+----";
                }
                else
                    cout << "+---";
            }
            cout << "+" << endl;
            if (i != PUZZLE_SIZE)
                cout << "|";
		}
	}
}

//checkGoal
//==========
//Checks to see if the current board state is the goal state and returns a bool
bool checkGoal(boardNode* current, vector<int>& goal){
	bool success = false;
	if (current->board == goal)
		success=true;
	return success;
}