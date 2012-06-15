/*--------------------------------------------------------------------------- 
	Project2 CSCE 315
	Laramie Goode, Chris Voss, and Curtis Franks
    Based on the Reversi Console by
	Richel Bilderbeek 
	http://www.richelbilderbeek.nl/GameReversiConsoleSource_1_0.htm
---------------------------------------------------------------------------*/
#ifndef UNITREVERSI_H
#define UNITREVERSI_H
//--------------------------------------------------------------------------- 
#include <vector>
#include <string>
#include <stack>
enum Square { empty, player1, player2, validMove }; 
//--------------------------------------------------------------------------- 
struct Reversi 
{ 
  Reversi(const int size); 
  const std::vector<std::vector<Square> >& GetBoard() const; 
  const Square GetSquare(const int x, const int y) const; 
  void DoMove(const int x, const int y, const Square player); 
  void SetSquare(const int x, const int y, const Square player); 
  const bool IsValidMove(const int x, const int y, const Square player) const; 
  const std::vector< std::pair<int,int> > GetValidMoves(const Square player) const; 
  const int GetSize() const; 
  const int Count(const Square player) const; 
  std::string setDifficulty;
  ~Reversi(){
    while(!boardHistory.empty()){
        delete boardHistory.top();
		boardHistory.pop();
    }
    while(!boardRedos.empty()){
        delete boardRedos.top();
		boardRedos.pop();
    }
  }
	const bool DoUndo();
	const bool DoRedo();
  private: 
  std::vector<std::vector<Square> > mBoard; 
    std::stack<std::vector< std::vector<Square> >* > boardHistory; //on every doMove add to this
    std::stack<std::vector< std::vector<Square> >* > boardRedos; //on every doMove, clear this

	
  const bool IsValidMoveUp(const int x, const int y, const Square player) const; 
  const bool IsValidMoveUpLeft(const int x, const int y, const Square player) const; 
  const bool IsValidMoveLeft(const int x, const int y, const Square player) const; 
  const bool IsValidMoveDownLeft(const int x, const int y, const Square player) const; 
  const bool IsValidMoveDown(const int x, const int y, const Square player) const; 
  const bool IsValidMoveDownRight(const int x, const int y, const Square player) const; 
  const bool IsValidMoveRight(const int x, const int y, const Square player) const; 
  const bool IsValidMoveUpRight(const int x, const int y, const Square player) const; 

  void DoMoveUp(const int x, int y, const Square player); 
  void DoMoveUpLeft(int x, int y, const Square player); 
  void DoMoveLeft(int x, const int y, const Square player); 
  void DoMoveDownLeft(int x, int y, const Square player); 
  void DoMoveDown(const int x, int y, const Square player); 
  void DoMoveDownRight(int x, int y, const Square player); 
  void DoMoveRight(int x, const int y, const Square player); 
  void DoMoveUpRight(int x, int y, const Square player); 

}; 

const Square GetSquareXy(const std::vector<std::vector<Square> >& board, const int x, const int y); 
const Square GetOtherPlayer(const Square player); 

#endif 


