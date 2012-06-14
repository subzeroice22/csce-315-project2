#include <string>
#include <sstream>
#include <iostream>

using namespace std;

enum occupiedType {EMPTY, WHITE, BLACK};
enum columnType {a = 0, b, c, d, e, f, g, h};

string intToString(int number) {
	stringstream ss;
	ss << number;
	return ss.str();
}

class Position {

	int row;
	//int column;
	columnType column;
	occupiedType occupied;

public:

	Position() {}
	
	Position(int input_row, columnType input_column) {
		row = input_row;
		column = input_column;
		occupied = EMPTY;
	}
	
	void setOccupied(occupiedType input) {
		occupied = input;
	}
	
	occupiedType getOccupied() {
		return occupied;
	}
};

class Board { 

	Position grid[8][8];
	bool display;
	unsigned int move;

public:
	//constructs the board with all empty spaces
	Board() {
		display = false;
		move = 0;
		for(int i = 0; i < 8; i++) {
			for(int j = 0; j < 8; j++) {
				Position new_spot(i, columnType(j));
				grid[j][i] = new_spot;
			}
		}
	}
	
	void displayOn() {
		display = true;
	}
	
	void displayOff() {
		display = false;
	}
	
	bool isValidMove(columnType column, int row) {
		bool isAdjacent = false;
		
		if( 1 ) {
			//
		}
		
	}
	
	void setElement(columnType column, int row, occupiedType occupy) {
		grid[column][row-1].setOccupied(occupy);
	}
	
	//gives a string representation of the game board
	string stringify() {
		string game_board = "  _ _ _ _ _ _ _ _ \n";
		
		for(int i = 0; i < 8; i++) {
		
			game_board = game_board + intToString(i+1) + "|";
		
			for(int j = 0; j < 8; j++) {
			
				 switch (grid[j][i].getOccupied()) {
					case EMPTY:
						game_board = game_board + "_|";
						break;
					case WHITE:
						game_board = game_board + "O|";
						break;
					case BLACK:
						game_board = game_board + "@|";
						break;
					default:
						game_board = game_board + "!|";
						break;
				 }
			}
			game_board = game_board + "\n";
		}
		
		game_board = game_board + "  a b c d e f g h \n";
	
		return game_board;
	}
	
	string showValidMoves() {
	
		if(move % 2 == 0) {
			//black's move
			
		
		
		
		} else { //white's move
		
			
		
		}
	
	}
	
};

int main() {

	Board game;
	
	cout << game.stringify() << '\n';
	
	game.setElement(d,4,WHITE);
	game.setElement(e,4,BLACK);
	game.setElement(d,5,BLACK);
	game.setElement(e,5,WHITE);
	
	cout << game.stringify() << '\n';
	

	return 0;
}
