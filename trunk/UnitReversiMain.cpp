/*--------------------------------------------------------------------------- 
	Project2 CSCE 315
	Laramie Goode, Chris Voss, and Curtis Franks
    Based on the Reversi Console by
	Richel Bilderbeek 
	http://www.richelbilderbeek.nl/GameReversiConsoleSource_1_0.htm
---------------------------------------------------------------------------*/
//#include <vector> 
#include <numeric> 
#include <algorithm> 
#include <iostream> 
#include <cassert> 
#include <iterator> 
#include <memory> 
#include <sstream> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits>
#include <cctype>//for upper to lower
#include "UnitReversi.h"//#includes <vector>
//#include "AI.h"
//global variables - created to prepare for port to Java

int boardSize=8,randomMove,isHuman,maxDepth;
bool displayOn=true,test=false;
std::string AIlevel = "OFF";
Square player=player1,AIPlayer=player2,humanPlayer=player1; 
Reversi newBoard(boardSize);
Reversi game(boardSize);//or you could prompt for the board size with Reversi r(AskUserForBoardSize());
std::pair<int,int> coordinate,bestMove;
//<< override for Reversi object squares
std::ostream& operator<<(std::ostream& os, const Square s) 
{ 
  switch (s) 
  { 
    case empty  : os <<GREEN<<"_"; break; 
	case validMove: os <<BOLDYELLOW<<"x"<<RESET<<GREEN; break;
    case player1: os <<BOLDBLACK<<"O"<<RESET<<GREEN; break; //char(2); break; 
    case player2: os <<BOLDWHITE<<"@"<<RESET<<GREEN; break; //char(1); break; 
    default: assert(!"Should not get here"); break; 
  } 
  return os; 
} 
//<< override for Reversi object board outlay
std::ostream& operator<<(std::ostream& os, const Reversi& r) 
{ 
  { //Show the indices horizontally 
    const int size = r.GetSize(); 
    os << "  "; 
    for (int i=0; i!=size; ++i) 
    { 
        os<<GREEN<<"_ "<<RESET;
      //os << (i%10)<<" "; 
    } 
    os << std::endl; 
  } 
  { 
    int i = 0; 
    const std::vector<std::vector<Square> >& board = r.GetBoard(); 
    const std::vector<std::vector<Square> >::const_iterator lastRow = board.end(); 
    for (std::vector<std::vector<Square> >::const_iterator row = board.begin(); 
      row!=lastRow; 
      ++row, ++i) 
    { 
      os << WHITE<<(i+1) <<GREEN<<"|"<<RESET; 
      std::copy( (*row).begin(), (*row).end(),std::ostream_iterator<Square>(os,"|")); 
      os<<"\n"<<RESET;
      //os << " " << (i%10) << '\n'; 
    } 
  } 

  { //Show the indices horizontally 
    //os << std::endl; 
    os << "  "; 
    const int size = r.GetSize(); 
    for (int i=0; i!=size; ++i) 
    { 
      os<<WHITE<< char( (int('a')+i) )<<RESET<< " ";
      //os << i%10; 
    } 
    os << std::endl; 
  }
  return os; 
} 
//Returns true if the string can be converted to integers
const bool IsInt(const std::string& s, int& rInt) 
{ 
  std::istringstream i(s); 
  if (!(i >> rInt)) 
  { 
    rInt = 0; 
    return false; 
  } 
  return true; 
} 
//Handles all input and drops the newline char
const std::string GetInput() 
{ 
	char c;
	std::string s; 
	std::getline(std::cin,s,'\n'); 
	for(int i=0;i<s.size();i++){
		c=s[i];
		s[i]=toupper(c);
	}

	return s; 
} 
//Breaks up the coordinate input for an x and y value
const std::vector<std::string> SeperateString(std::string input, const char seperator) 
{ 
  assert(input.empty()==false); 
  assert(input[0]!=seperator); 
  assert(input[input.size()-1]!=seperator); 

  std::vector<std::string> result; 
  int pos = 0; 
  while(pos<static_cast<int>(input.size())) 
  { 
    if (input[pos]==seperator) 
    { 
      const std::string found = input.substr(0,pos); 
      result.push_back(found); 
      input = input.substr(pos+1,input.size()-pos); 
      pos = 0; 
    } 
    ++pos; 
  } 
  result.push_back(input); 
  return result; 
} 
//Up front determination whether the users coordinate is a valid input type 
const bool IsCoordinate(const std::string& input, std::pair<int,int>& coordinate) {
    if(input.size()!=2) return false;
	//TODO: need error (bounds) checking on x and y
    int x, y;
    x = input[0] - 'A';
    y = int(input[1] - '0')-1;
    coordinate.first=x;
	coordinate.second=y;
    return true;
} 
//Optional menu option allowing for board sizes between 4X4 and 16X16
const int AskUserForBoardSize() 
{ 
  //Get the board's size 
  while (1) 
  { 
    std::cout << "Please enter the size of the reversi board" << std::endl; 
    const std::string input = GetInput(); 
    int size = -1; 
    if ( IsInt(input,size) == false) 
    { 
      std::cout << "Please enter an integer value. " << std::endl; 
      continue; 
    } 
    if ( size < 4) 
    { 
      std::cout << "Please enter an integer value bigger than 4. " << std::endl; 
      continue; 
    } 
    if ( size > 16) 
    { 
      std::cout << "Please enter an integer value less than 16. " << std::endl; 
      continue; 
    } 
    return size; 
  } 
} 

int handlePregameInput(){
	while(1){
		const std::string input = GetInput();
		if(input=="EXIT")
			return 0;
		if(input=="TEST_HARD"){
			AIlevel="HARD";
			test=true;
			std::cout<<"HARD V HARD\n";
		}
		if(input=="TEST_MEDIUM"){
			AIlevel="MEDIUM";
			test=true;
			std::cout<<"HARD V MEDIUM\n";
		}
		if(input=="TEST_EASY"){
			AIlevel="EASY";
			test=true;
			std::cout<<"HARD V EASY\n";
		}
		if(input=="BLACK"){
			player = player1;
			AIPlayer=player2;
			std::cout<<"BLACK\n";
		}
		if(input=="WHITE"){
			player = player2;
			AIPlayer=player1;
			std::cout<<"WHITE\n";
		}
		if(input=="EASY"){
			AIlevel="EASY";
			std::cout<<"OK\n";
		}
		if(input=="MEDIUM"){
			AIlevel="MEDIUM";
			std::cout<<"OK\n";
		}
		if(input=="HARD"){
			AIlevel="HARD";
			std::cout<<"OK\n";
		}
		if(input=="DISPLAY_ON" || input =="1"){
			displayOn = true;
			std::cout<<"OK\n";
			break;
		}
		if(input=="DISPLAY_OFF"){
			displayOn = false;
			std::cout<<"OK\n";
			break;
		}
		if(input=="4X4"){
			std::cout<<"OK\n";
			boardSize=4;
		}
		if(input=="8X8"){
			std::cout<<"OK\n";
			boardSize=8;
		}
		if(input=="?"){
			std::cout<<"WHITE, BLACK, EASY, MEDIUM, HARD, DISPLAY_ON, EXIT\n";
		}
	}
	return 1;
}

void showPossibleMoves(){
	Reversi tempValid(boardSize);
	tempValid.SetBoard(game.GetBoard());
	std::vector< std::pair<int,int> > vals = tempValid.GetValidMoves(player);
	for(int i=0; i<vals.size(); i++)
		tempValid.SetSquare(vals[i].first,vals[i].second,validMove);
	std::cout<<tempValid<<"\n";
}

void moveRandomly(){
	srand ( time(NULL) );
	Reversi tempValid(boardSize);
	tempValid.SetBoard(game.GetBoard());
	std::vector< std::pair<int,int> > vals = tempValid.GetValidMoves(player);
	int randomMove = rand() % vals.size();
	coordinate.first=vals[randomMove].first;coordinate.second=vals[randomMove].second;
}

int checkForWeight(Reversi parentBoard, Square forecastPlayer,int depth){
	
	int endDepthScore=-99999,minCount=64,minPossibleMove=0,maxPossibleMove=0;
	Square nextPlayer;
	if(forecastPlayer!=player&&parentBoard.GetValidMoves(forecastPlayer).empty()==true)
		return 10000;
	else if(forecastPlayer==player&&parentBoard.GetValidMoves(forecastPlayer).empty()==true)
		return -10000;
	std::vector< std::pair<int,int> >vals=parentBoard.GetValidMoves(forecastPlayer);
	for(int possibleMove=0;possibleMove<vals.size();possibleMove++){
		int maxCount=0;
		Square opponent;
		Reversi childBoard(boardSize);				//making one childBoard for each 
		childBoard.SetBoard(parentBoard.GetBoard());//possible Move from the parentBoard
		childBoard.DoMove(vals[possibleMove].first,vals[possibleMove].second,forecastPlayer);//add this move to its board
		if(depth<maxDepth){
			nextPlayer = (forecastPlayer == player1 ? player2 : player1);
			maxCount=maxCount+checkForWeight(childBoard,nextPlayer,depth+1);
		}
		else{
			return childBoard.Count(forecastPlayer);
		}

		if	(childBoard.Count(empty) == 0||//No more empty squares or niether player has a move, end game 
			((childBoard.GetValidMoves(player1).empty()==true)&&
			(childBoard.GetValidMoves(player2).empty()==true))){ 
				if(player==player1)
					opponent=player2;
				else
					opponent=player1;
				if(childBoard.Count(player)>childBoard.Count(opponent))
					maxCount=maxCount+10000;
				else
					maxCount=maxCount-10000;
		}else if(forecastPlayer!=player&&childBoard.GetValidMoves(forecastPlayer).empty()==true){
			maxCount=maxCount+10000;
		}else if(forecastPlayer==player){
			if((vals[possibleMove].first==1&&
				(vals[possibleMove].second!=0&&vals[possibleMove].second!=7))||
				(vals[possibleMove].first==6&&
				(vals[possibleMove].second!=0&&vals[possibleMove].second!=7)))
					maxCount=maxCount-1000;
			if((vals[possibleMove].second==1&&
				(vals[possibleMove].first!=0&&vals[possibleMove].first!=7))||
				(vals[possibleMove].second==6&&
				(vals[possibleMove].first!=0&&vals[possibleMove].first!=7)))
					maxCount=maxCount-1000;
			if(vals[possibleMove].first==0||vals[possibleMove].first==7)
				maxCount=maxCount+1000;
			if(vals[possibleMove].second==0||vals[possibleMove].second==7)
				maxCount=maxCount+1000;
			if((vals[possibleMove].first==0&&vals[possibleMove].second==0)||
				(vals[possibleMove].first==7&&vals[possibleMove].second==7)||
				(vals[possibleMove].first==0&&vals[possibleMove].second==7)||
				(vals[possibleMove].first==7&&vals[possibleMove].second==0))
					maxCount=maxCount+5000;
		}else{
			if((vals[possibleMove].first==1&&
				(vals[possibleMove].second!=0&&vals[possibleMove].second!=7))||
				(vals[possibleMove].first==6&&
				(vals[possibleMove].second!=0&&vals[possibleMove].second!=7)))
					maxCount=maxCount+100;
			if((vals[possibleMove].second==1&&
				(vals[possibleMove].first!=0&&vals[possibleMove].first!=7))||
				(vals[possibleMove].second==6&&
				(vals[possibleMove].first!=0&&vals[possibleMove].first!=7)))
					maxCount=maxCount+100;
			if(vals[possibleMove].first==0||vals[possibleMove].first==7)
				maxCount=maxCount-1000;
			if(vals[possibleMove].second==0||vals[possibleMove].second==7)
				maxCount=maxCount-1000;
			if((vals[possibleMove].first==0&&vals[possibleMove].second==0)||
				(vals[possibleMove].first==7&&vals[possibleMove].second==7)||
				(vals[possibleMove].first==0&&vals[possibleMove].second==7)||
				(vals[possibleMove].first==7&&vals[possibleMove].second==0))
					maxCount=maxCount-5000;
			if(childBoard.GetValidMoves(forecastPlayer).empty()==true)
				maxCount=maxCount+1000;
		}


		if(maxCount>endDepthScore)
			endDepthScore=maxCount;
	}
	return endDepthScore;
}

std::pair<int,int> findBestMove(Square forecastPlayer){
	int maxCountForAll=-99999,maxCountAtDepth=-99999,maxPossibleMove=0;
	Square nextPlayer;
	std::vector< std::pair<int,int> >vals=game.GetValidMoves(forecastPlayer);//possible moves for current player
	for(int possibleMove=0;possibleMove<vals.size();possibleMove++){	
		int primaryWeight=0;
		Reversi boardForAPrimaryMove(boardSize);
		boardForAPrimaryMove.SetBoard(game.GetBoard());//new board (branch to tree) for each possibleMove created each itteration
		boardForAPrimaryMove.DoMove(vals[possibleMove].first, vals[possibleMove].second, forecastPlayer);//add this possible move to its board
		if(	(vals[possibleMove].first==1&&
			(vals[possibleMove].second!=0&&vals[possibleMove].second!=7))||
			(vals[possibleMove].first==6&&
			(vals[possibleMove].second!=0&&vals[possibleMove].second!=7)))
				primaryWeight=primaryWeight-5000;
		if(	(vals[possibleMove].second==1&&
			(vals[possibleMove].first!=0&&vals[possibleMove].first!=7))||
			(vals[possibleMove].second==6&&
			(vals[possibleMove].first!=0&&vals[possibleMove].first!=7)))
				primaryWeight=primaryWeight-5000;
		if(vals[possibleMove].first==0||vals[possibleMove].first==7)
				primaryWeight=primaryWeight+2000;
		if(vals[possibleMove].second==0||vals[possibleMove].second==7)
				primaryWeight=primaryWeight+2000;
		if(	(vals[possibleMove].first==0&&vals[possibleMove].second==0)||
			(vals[possibleMove].first==7&&vals[possibleMove].second==7)||
			(vals[possibleMove].first==0&&vals[possibleMove].second==7)||
			(vals[possibleMove].first==7&&vals[possibleMove].second==0))
				primaryWeight=primaryWeight+5000;
		
		
		nextPlayer = (forecastPlayer == player1 ? player2 : player1);//figure out who is the next player
		maxCountAtDepth=primaryWeight+checkForWeight(boardForAPrimaryMove,nextPlayer,0);//the maximum outcome from the set Depth (checkForWeight())
		if(maxCountAtDepth>maxCountForAll){	//if this possible Move's maximum outcome is bigger
			maxCountForAll=maxCountAtDepth;	//keep it as the new maximum overall
			maxPossibleMove=possibleMove;	//and remember which move that maximum belongs to
		}
	}
	std::cout<<"Value of this move:"<<maxCountForAll<<std::endl;
	return vals[maxPossibleMove];//when done checking down each branch of possible Moves send the best move back

}

int handleGameInput(){
	while(1){
		if(displayOn==true)
			std::cout<< game;
		if	(game.Count(empty) == 0||//No more empty squares or niether player has a move, end game 
			((game.GetValidMoves(player1).empty()==true)&&(game.GetValidMoves(player2).empty()==true))){ 
		  const int n1 = game.Count(player1); 
		  const int n2 = game.Count(player2); 
		  std::cout << "The game has ended." << std::endl 
			<< "Player 1 conquered " << n1 << " squares." << std::endl 
			<< "Player 2 conquered " << n2 << " squares." << std::endl;
			if(n1!=n2){
			std::cout<< "The winner is player " << (n1 > n2 ? "1" : "2") << std::endl 
			<< "Congratulations!" << std::endl;
			}
			else
				std::cout<<"It's a draw!\n";
		    std::cout << "Press ENTER to continue... ";
			std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
			test=false;
			AIlevel="OFF";
			game.SetBoard(newBoard.GetBoard());
		  return 1;
		} 
		if (game.GetValidMoves(player).empty()==true){ //Check if current player can move 
		  std::cout << "Too bad! Player " << player << " is unabled to do a valid move!"; 
		  player = (player == player1 ? player2 : player1); 
		  std::cout << "\nThe next turn again goes to player " << player << "!" << std::endl; 
		  continue; 
		} 
		std::cout<<"Current Player:"<<player<<"\n";//Who's turn is it?
		if(AIlevel!="OFF"&&player==AIPlayer){//Which AI to use
			if(AIlevel=="EASY")
				moveRandomly();
			if(AIlevel=="MEDIUM"){
				maxDepth=2;
				std::pair<int,int> bestMove = findBestMove(player);
				coordinate.first=bestMove.first;coordinate.second=bestMove.second;
			}
			if(AIlevel=="HARD"){
				maxDepth=3;
				std::pair<int,int> bestMove = findBestMove(player);
				coordinate.first=bestMove.first;coordinate.second=bestMove.second;
				//HardAI hAI(game, player);
				//coordinate = hAI.findMax();
				//cout<<"MaxFound:"<<coordinate.first<<","<<coordinate.second<<endl;
			}
		}
		if(test==true&&player==player1){
			maxDepth=3;
			std::pair<int,int> bestMove = findBestMove(player);
			coordinate.first=bestMove.first;coordinate.second=bestMove.second;
		}
		else if(AIlevel!="OFF"&&player!=AIPlayer&&test==false) {
			const std::string input = GetInput();
			const bool isValidCoordinate = IsCoordinate(input, coordinate); 
			if (isValidCoordinate == false){
				if(input=="EXIT")
					return 0;
				else if(input=="?"){
					std::cout<<"Enter coordinates as # alpha values, DISPLAY_OFF, SHOW_NEXT_POS, UNDO, REDO, EXIT\n";
					continue;
				}
				else if(input=="DISPLAY_OFF"){
					displayOn = false;
					std::cout<<"OK\n";
					continue;
				}
				if(input=="UNDO"){
					if(!game.DoUndo()){
						assert(!"CANNOT UNDO, NOT ENOUGH STATES IN STACK");
						std::cout<<"ILLEGAL\n";
						continue;
					}continue;
				}else if(input=="REDO"){
					if(!game.DoRedo()){
						assert(!"CANNOT REDO, NOT ENOUGH STATES IN STACK");
						std::cout<<"ILLEGAL\n";
						continue;
					}continue;
				}else if(input=="SHOW_NEXT_POS"){
					showPossibleMoves();
					continue;
				}else if(input=="RAND"){
					moveRandomly();
				}else{
					std::cout << "ILLEGAL\n";
					continue;
				} 
			}
		} 
		if (game.IsValidMove(coordinate.first, coordinate.second, player) == false){ 
		  std::cout << "ILLEGAL\n";
		  continue; 
		}
		game.DoMove(coordinate.first, coordinate.second, player); //Actually do the move 
		player = (player == player1 ? player2 : player1);//switch to next player
	}
	return 1;
}
//Handles user input and display of data 
int api(std::string commandLine)
{
	while(1){
		for(int i=0;i<100;i++)std::cout<<"\n";
		std::cout<< "WELCOME\n";
		if(!handlePregameInput())
			return 0;
		std::cout<<"Player1"<<"BLACK"<<player1<<"\n"<<RESET;
		std::cout<<"Player2"<<"WHITE"<<player2<<"\n"<<RESET;
		if(!handleGameInput())
			return 0;
	}
}

int main() 
{
	api("START");
	return 0;
}



