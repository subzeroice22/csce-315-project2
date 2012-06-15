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
#include "UnitReversi.h"//#includes <vector>
//--------------------------------------------------------------------------- 
std::ostream& operator<<(std::ostream& os, const Square s) 
{ 
  switch (s) 
  { 
    case empty  : os <<GREEN<<"_"<<GREEN; break; 
	case validMove: os <<BOLDYELLOW<<"x"<<RESET<<GREEN; break;
    case player1: os <<BOLDBLACK<<"O"<<RESET<<GREEN; break; //char(2); break; 
    case player2: os <<BOLDWHITE<<"@"<<RESET<<GREEN; break; //char(1); break; 
    default: assert(!"Should not get here"); break; 
  } 
  return os; 
} 
//--------------------------------------------------------------------------- 
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
      os << WHITE<<(i+1) <<GREEN<<"|"; 
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
//Checks whether a std::string can be converted to an integer. 
//Returns true if possible, also returning this integer by referencing. 
//Returns false otherwise, setting the referenced integer to zero. 
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
//--------------------------------------------------------------------------- 
const std::string GetInput() 
{ 
  std::string s; 
  std::getline(std::cin,s,'\n'); 
  return s; 
} 
//--------------------------------------------------------------------------- 
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
//--------------------------------------------------------------------------- 
const bool IsCoordinate(const std::string& input, std::pair<int,int>& coordinate) {
    if(input.size()!=2) return false;
	//TODO: need error (bounds) checking on x and y
    int x, y;
    x = input[0] - 'a';
    y = int(input[1] - '0')-1;
    coordinate.first=x;
	coordinate.second=y;
    return true;
    /*
  if ( std::count(input.begin(), input.end(), ',') != 1) return false; 
  if ( *(input.begin()) == ',' || *(input.end() - 1) == ',') return false; 
  const std::string::const_iterator i = std::find(input.begin(), input.end(), ','); 
  assert(i != input.end() ); //Comma must be in! Checked above! 

  const std::vector<std::string> v(SeperateString(input,',')); 
  if (v.size() != 2) return false; 
  if (IsInt(v[0],coordinate.first)==false) return false; 
  if (IsInt(v[1],coordinate.second)==false) return false; 
 return true; 
*/
} 
//--------------------------------------------------------------------------- 
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
//--------------------------------------------------------------------------- 
int api(std::string commandLine)
{
	Square player=player1; 
	std::pair<int,int> coordinate;
	int boardSize=8;
	std::string difficulty="EASY";
	std::cout<< "WELCOME\n";
	while(1){
		const std::string input = GetInput();
		if(input=="EXIT")
			return 0;
		if(input=="BLACK"){
			player = player1; 
			std::cout<<"BLACK\n";
		}
		if(input=="WHITE"){
			player = player2;
			std::cout<<"WHITE\n";
		}
		if(input=="EASY"){
			difficulty="EASY";
			std::cout<<"OK\n";
		}
		if(input=="MEDIUM"){
			difficulty="MEDIUM";
			std::cout<<"OK\n";
		}
		if(input=="HARD"){
			difficulty="HARD";
			std::cout<<"OK\n";
		}
		if(input=="DISPLAY_ON" || input =="1"){
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
			std::cout<<"WHITE, BLACK, EASY, MEDIUM, HARD, DISPLAY_ON, 4X4, 8X8, EXIT\n";
		}
	}
	
	std::cout<<"Player1"<<"BLACK"<<player1<<"\n";
	std::cout<<"Player2"<<"WHITE"<<player2<<"\n";
	
	Reversi game(boardSize);//or you could prompt for the board size with Reversi r(AskUserForBoardSize());
	game.setDifficulty=difficulty;
	while(1){
		std::cout<<"Current Player:"<<player<<"\n";
		std::cout<< game;
		const std::string input = GetInput();
		if(input=="EXIT")
			return 0;
		if(input=="?")
			std::cout<<"Enter coordinates as # alpha values, EXIT\n";
		
		const bool isValidCoordinate = IsCoordinate(input, coordinate); 
		if (isValidCoordinate == false) 
		{
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
		    }else{
				std::cout << "ILLEGAL\n";
				continue;
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
}


int main() 
{
	api("START");
	return 0;
}



