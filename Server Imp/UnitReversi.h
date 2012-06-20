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
//the following are UBUNTU/LINUX ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"     
#define RED     "\033[31m"     
#define GREEN   "\033[32m"    
#define YELLOW  "\033[33m"     
#define BLUE    "\033[34m"     
#define MAGENTA "\033[35m"    
#define CYAN    "\033[36m"      
#define WHITE   "\033[37m"     
#define BOLDBLACK   "\033[1m\033[30m"     
#define BOLDRED     "\033[1m\033[31m"     
#define BOLDGREEN   "\033[1m\033[32m"      
#define BOLDYELLOW  "\033[1m\033[33m"      
#define BOLDBLUE    "\033[1m\033[34m"     
#define BOLDMAGENTA "\033[1m\033[35m"      
#define BOLDCYAN    "\033[1m\033[36m"     
#define BOLDWHITE   "\033[1m\033[37m"      
//-------------------------------------------------------------------------//
/*the following is for testing in a windows console
#define RESET   ""
#define BLACK   ""      
#define RED     ""      
#define GREEN   ""      
#define YELLOW  ""      
#define BLUE    ""     
#define MAGENTA ""      
#define CYAN    ""      
#define WHITE   ""      
#define BOLDBLACK   ""      
#define BOLDRED     ""     
#define BOLDGREEN   ""      
#define BOLDYELLOW  ""      
#define BOLDBLUE    ""     
#define BOLDMAGENTA ""    
#define BOLDCYAN    ""      
#define BOLDWHITE   ""      
//-------------------------------------------------------------------------*/

struct Reversi 
{ 
  Reversi(const int size); 
  const std::vector<std::vector<Square> >& GetBoard() const; 
  const Square GetSquare(const int x, const int y) const; 
  void DoMove(const int x, const int y, const Square player);   
  void SetSquare(const int x, const int y, const Square player); 
  void setSize(const int newSize);
  const bool IsValidMove(const int x, const int y, const Square player) const; 
  const std::vector< std::pair<int,int> > GetValidMoves(const Square player) const; 
  const int GetSize() const; 
  const int Count(const Square player) const; 

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
	void SetBoard(std::vector<std::vector<Square> > newBoard); 
  private: 
  std::vector<std::vector<Square> > mBoard; 
    std::stack<std::vector< std::vector<Square> >* > boardHistory; 
    std::stack<std::vector< std::vector<Square> >* > boardRedos;
	int size;
	
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


