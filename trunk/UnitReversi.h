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
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
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
	void SetBoard(std::vector<std::vector<Square> > newBoard); 
  private: 
  std::vector<std::vector<Square> > mBoard; 
    std::stack<std::vector< std::vector<Square> >* > boardHistory; 
    std::stack<std::vector< std::vector<Square> >* > boardRedos;

	
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


