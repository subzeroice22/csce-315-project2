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
#include <time.h>
#include <cctype>//for upper to lower
#include "UnitReversi.h"//#includes <vector>
//global variables - created to prepare for port to Java
int boardSize=8,randomMove,isHuman,moveDepth;
bool displayOn=true;
std::string AIlevel = "OFF";
Square player=player1,AIPlayer=player2,humanPlayer=player1; 
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
	Reversi tempValid(boardSize);
	tempValid.SetBoard(game.GetBoard());
	std::vector< std::pair<int,int> > vals = tempValid.GetValidMoves(player);
	int randomMove = rand() % vals.size();
	coordinate.first=vals[randomMove].first;coordinate.second=vals[randomMove].second;
}

/*std::pair<int,int> forcastThenMove(Reversi previousBoard,Square forcastPlayer){
	std::pair<int,int> countFromForcast;
	std::pair<int,int> returnFromForcast;
	std::pair<int,int> forcastCoordinate;
	Square nextPlayer;
	int highestDifferential=0;
	Reversi tempValid(boardSize);
	tempValid.SetBoard(previousBoard.GetBoard());
	std::vector< std::pair<int,int> > vals = tempValid.GetValidMoves(forcastPlayer);
	if (tempValid.Count(empty) == 0){
			countFromForcast.first = tempValid.Count(player1); 
			countFromForcast.second = tempValid.Count(player2); 
			return countFromForcast;
	}
	if(vals.size()==0){
		nextPlayer = (forcastPlayer == player1 ? player2 : player1);
		returnFromForcast = forcastThenMove(tempValid,nextPlayer);
	}
	for(int i=0;i<vals.size();i++){
		tempValid.DoMove(vals[i].first, vals[i].second, forcastPlayer); 
		std::cout<< tempValid;
		nextPlayer = (forcastPlayer == player1 ? player2 : player1);
		moveDepth++;
		if(moveDepth<10)
			returnFromForcast = forcastThenMove(tempValid,nextPlayer);
		if(player==player1){
			if(returnFromForcast.first>returnFromForcast.second){
				if(abs(returnFromForcast.first-returnFromForcast.second)>highestDifferential){
					highestDifferential = abs(returnFromForcast.first-returnFromForcast.second);
					bestMove=vals[i];
				}
				return returnFromForcast;
			}
		}
		if(player==player2){
			if(returnFromForcast.first<returnFromForcast.second){
				if(abs(returnFromForcast.first-returnFromForcast.second)>highestDifferential){
					highestDifferential = abs(returnFromForcast.first-returnFromForcast.second);
					bestMove=vals[i];
				}
				return returnFromForcast;
			}
		}
	}
	return returnFromForcast;

}*/
int checkForMin(Reversi tempGame, Square forcastPlayer){
	Reversi tempValid(boardSize);
	int count=64,minPossibleMove=0;
	Square nextPlayer;
	tempValid.SetBoard(tempGame.GetBoard());
	std::vector< std::pair<int,int> > vals = tempValid.GetValidMoves(forcastPlayer);
	for(int possibleOpponentMove=0;possibleOpponentMove<vals.size();possibleOpponentMove++){
		tempValid.DoMove(vals[possibleOpponentMove].first,vals[possibleOpponentMove].second, forcastPlayer);
		if(tempValid.Count(forcastPlayer)<count){
			count=tempValid.Count(forcastPlayer);
			minPossibleMove=possibleOpponentMove;
		}
	}


	return count;

}
std::pair<int,int> checkForMax(Reversi tempGame, Square forcastPlayer){
	Reversi tempValid(boardSize);
	int countMax=0,countMin=0,maxPossibleMove=0;
	Square nextPlayer;
	tempValid.SetBoard(tempGame.GetBoard());
	std::vector< std::pair<int,int> > vals = tempValid.GetValidMoves(forcastPlayer);
	for(int possibleMove=0;possibleMove<vals.size();possibleMove++){														
		tempValid.DoMove(vals[possibleMove].first, vals[possibleMove].second, forcastPlayer);
		nextPlayer = (forcastPlayer == player1 ? player2 : player1);
		countMin=checkForMin(tempValid,nextPlayer);
		if(countMin>countMax){
			countMax=countMin;
			maxPossibleMove=possibleMove;
		}

	}

	return vals[maxPossibleMove];
}





std::pair<int,int> minMax(Reversi tempGame, Square tempPlayer){
	//check for valid moves first, if none just send turn to next player

	std::pair<int,int> temp;

	return temp;
}






int handleGameInput(){
	while(1){
		std::cout<<"Current Player:"<<player<<"\n";
		if(displayOn==true)
			std::cout<< game;
		if(AIlevel!="OFF"&&player==AIPlayer){
			if(AIlevel=="EASY")
				moveRandomly();
			if(AIlevel=="MEDIUM"){
				moveDepth=0;
				std::pair<int,int> bestMove = checkForMax(game,player);
				coordinate.first=bestMove.first;coordinate.second=bestMove.second;
			}
		}
		else {
			const std::string input = GetInput();
			const bool isValidCoordinate = IsCoordinate(input, coordinate); 
			if (isValidCoordinate == false) 
			{
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
		if (game.IsValidMove(coordinate.first, coordinate.second, player) == false) 
		{ 
		  std::cout << "ILLEGAL\n";
		  continue; 
		}
		//Actually do the move 
		game.DoMove(coordinate.first, coordinate.second, player); 

		//Check if the game has ended 
		if (game.Count(empty) == 0) 
		{ 
		  //No empty squares 
		  const int n1 = game.Count(player1); 
		  const int n2 = game.Count(player2); 
		  std::cout << "The game has ended." << std::endl 
			<< "Player 1 conquered " << n1 << " squares." << std::endl 
			<< "Player 2 conquered " << n2 << " squares." << std::endl 
			<< "The winner is player " << (n1 > n2 ? "1" : "2") << std::endl 
			<< "Congratulations!" << std::endl 
			<< std::endl;
		  return 1;
		} 

		//Check if other player can actually do a move 
		if (game.GetValidMoves(player).empty()==true) 
		{ //if otherPlayer cannot move,
		  std::cout << "Too bad! Player " << player << " is unabled to do a valid move!"; 
		  //player = (player == player1 ? player2 : player1); 
		  std::cout << "\nThe next turn again goes to player " << player << "!" << std::endl; 
		  continue; 
		} 
		
		player = (player == player1 ? player2 : player1); 
		//foreach pair m in getValidMoves; setSquare(m , validMove);
		
	}
	return 0;
}
//Handles user input and display of data 
int api(std::string commandLine)
{
	for(int i=0;i<100;i++)std::cout<<"\n";
	std::cout<< "WELCOME\n";
	if(!handlePregameInput())
		return 0;
	std::cout<<"Player1"<<"BLACK"<<player1<<"\n"<<RESET;
	std::cout<<"Player2"<<"WHITE"<<player2<<"\n"<<RESET;
	if(!handleGameInput())
		return 0;
}

int main() 
{
	api("START");
	return 0;
}



