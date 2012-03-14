//
//  main.cpp
//  8Puzzle
//
//  Created by Atticus Wright on 3/5/12.
//  Copyright (c) 2012 University of North Alabama. All rights reserved.
//

//
//  main.cpp
//  8Puzzle
//
//  Created by Patrick Lindsay on 3/5/12.
//  Copyright (c) 2012 Patrick Lindsay. All rights reserved.
//

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <fstream>
#include <functional>
using namespace std;

// global variables
const int puzzleDimen = 3;
const int puzzleSize = 9;


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

// boardNode Class functions
boardNode::boardNode(){
    board.resize(puzzleSize+1);
    fOfN = 0;
    hOfN = 0;
    gOfN = 0;
    lastMove = -1;
    parentBoard = NULL;
    
}

boardNode::boardNode(int size){
    board.resize(puzzleSize+1);
    fOfN = 0;
    hOfN = 0;
    gOfN = 0;
    lastMove = -1;
    parentBoard = NULL;
}

int boardNode::getfOfN(void) const{
        return this->fOfN;
}


// Comparison class for boardNode pointers
class comparison : public std::binary_function<boardNode*, boardNode*, bool>{
public:
    bool operator() (const boardNode* lhs, const boardNode* rhs) const
    {
        
        return (lhs->getfOfN() > rhs->getfOfN() );
    }
};

// Comparison class for boardNodes
class comparison1 {
public:
    bool operator() (const boardNode &lhs, const boardNode &rhs) const
    {
        return (lhs.fOfN > rhs.fOfN);
    }
};

enum moves {U, R, D, L};




/*Function Prototypes********************************/
void readInPuzzle(boardNode& startPuzzle);
void printBoard(boardNode* graph);
void printBoard2(boardNode &graph);
void aStar(boardNode* rootBoard, vector<int>& goal);
int manhattan(boardNode* current, vector<int>& goal);
int getGoalIndex(vector<int>& goal, int key);
bool isLegalMove(boardNode* current, int move, int blankIndex);
boardNode* createState(boardNode& current, int move, int blankIndex);
//void buildBoard(vector<int>& graph);
bool checkGoal(boardNode* current, vector<int>& goal);
void printSolution(boardNode* current);
/****************************************************/




int main ()
{

    vector<int> goal(puzzleSize+1);
	goal[1]=1;
	goal[2]=2;
	goal[3]=3;
	goal[4]=8;
	goal[5]=0;
	goal[6]=4;
	goal[7]=7;
	goal[8]=6;
	goal[9]=5;
    
	boardNode startPuzzle(puzzleSize+1); // Will hold initial puzzle configuration
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



void aStar(boardNode* rootBoard, vector<int>& goal){
    

    priority_queue<boardNode*, vector<boardNode*>, comparison > open;
    vector<boardNode*> closed;
    boardNode *current, *next, *previous;
    bool foundGoal;
    bool legalMove;
    bool containsNext = false;
    int  blankIndex;
    int searches = 0;
    
    
    open.push(rootBoard);
    while (open.size() != 0 && searches < 4000) {
        
        // get the best board on open
        current = open.top();
        open.pop();
        closed.push_back(current);
        
    
        foundGoal = checkGoal(current, goal);
        
        
        if (foundGoal == false) {
            blankIndex = getGoalIndex(current->board, 0);
            
            for (int i = U; i <= L; i++) {
                legalMove = isLegalMove(current, i, blankIndex);
                if (legalMove == true){
                    searches++;
                    next = createState(*current, i, blankIndex);
                    next->hOfN = manhattan(next, goal);
                    next->fOfN = next->hOfN + next->gOfN;
        
                    containsNext = false;
                    for (int i = 0; i < closed.size(); i++) {
                        if (closed[i]->board == next->board) {
                            containsNext = true;
                            previous = closed[i];
                            if (next->fOfN < previous->fOfN) {
                                closed.erase(closed.begin() + i);
                                open.push(next);
                            }
                        }
                    }
                    if (containsNext == false) {
                        open.push(next);
                    }
                }
            }
        }
        else
            break;
    }
    if (foundGoal == true) 
        printSolution(current);
    else
        cout << "Starting puzzle configuration has no solution." << endl;
}


int manhattan(boardNode* current, vector<int>& goal){
    


    int manhatt = 0;
    int currIndex = 1;
    int goalIndex = 0;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int val1 = 0;
    int val2 = 0;
    
    for (int i = 0; i < puzzleSize; i++) {
        if (current->board[currIndex] != goal[currIndex] && current->board[currIndex] != 0) {
            x1 = i % 3;  // We mod to get the column value for x1
            y1 = i / 3;  // We use integer division to get the row value for y1
            goalIndex = getGoalIndex(goal, current->board[currIndex]);
            x2 = goalIndex % 3;
            y2 = goalIndex / 3;
            
            val1 = x1 - x2;
            val2 = y1 - y2;
            manhatt += abs(val1) + abs(val2);
        }
        currIndex++;
    }
    
    return manhatt;
}

void printSolution(boardNode* current){
    vector<boardNode> solution;
    
    while (current->parentBoard != NULL) {
        solution.push_back(*current);
        current = current->parentBoard;
        
    }
    solution.push_back(*current);
    
    cout << "The optimal solution is: " << endl;
    for (int i = (int)solution.size()-1; i >= 0; i--) {
        printBoard2(solution[i]);
        cout << endl;
    }
    
    cout << "Number of moves: " << solution.size()-1 << endl;
}

bool isLegalMove(boardNode* current, int move, int blankIndex){

    bool legal = true;;
    int blankx1 = blankIndex % 3;
    int blanky1 = blankIndex / 3;
    int lastmove = current->lastMove;
    
    
    switch (move) {
        case U:
            if (blanky1 == 0 || lastmove == D) {
                legal = false;
            }
            break;
        case R:
            if (blankx1 == 2 || lastmove == L) {
                legal = false;
            }
            break;
        case D:
            if (blanky1 == 2 || lastmove == U) {
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

boardNode* createState(boardNode& current, int move, int blankIndex){
    boardNode* temp = new boardNode(puzzleSize+1);
    temp->board = current.board;
    temp->gOfN = current.gOfN + 1;
    temp->parentBoard = &current;
    temp->lastMove = move;
    
    switch (move) {
        case U:
            temp->board[blankIndex+1] = temp->board[blankIndex-2];
            temp->board[blankIndex-2] = 0;
            break;
        case R:
            temp->board[blankIndex+1] = temp->board[blankIndex+2];
            temp->board[blankIndex+2] = 0;
            break;
        case D:
            temp->board[blankIndex+1] = temp->board[blankIndex+4];
            temp->board[blankIndex+4] = 0;
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

int getGoalIndex(vector<int>& goal, int key){
    int index = 0;
    
    for (int i = 1; i < puzzleSize+1; i++) {
        if (goal[i] == key) {
            index = i;
        }
    }
    return index-1;
}
    




void readInPuzzle(boardNode& startPuzzle){
    ifstream fin;
    int val;
    int counter = 1;
    
    fin.open("8puzzle.txt");
    if (fin.fail()) {
        cout << "Input file opening failed.\n";
    }
    
    while (! fin.eof()) {
        fin >> val;
        startPuzzle.board[counter] = val;
        counter++;
    }
}

void printBoard(boardNode* graph)
{
    cout << "+---+---+---+" << endl << "|";
	for(int i = 1; i <= puzzleSize; i++){
		cout << " " << graph->board[i] << " |";
		if (i % 3 == 0) { 
			cout << endl;
            cout << "+---+---+---+" << endl;
            if (i != puzzleSize)
                cout << "|";
		}
	}
}

void printBoard2(boardNode &graph)
{
    cout << "+---+---+---+" << endl << "|";
	for(int i = 1; i <= puzzleSize; i++){
		cout << " " << graph.board[i] << " |";
		if (i % 3 == 0) { 
			cout << endl;
            cout << "+---+---+---+" << endl;
            if (i != puzzleSize)
                cout << "|";
		}
	}
}


bool checkGoal(boardNode* current, vector<int>& goal){
	bool success = false;
	if (current->board == goal)
		success=true;
	return success;
}