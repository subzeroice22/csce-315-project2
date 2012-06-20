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
int boardSize=8,randomMove,isHuman;
bool displayOn=true;
std::string AIlevel = "OFF";
Square player=player1,AIPlayer=player2,humanPlayer=player1; 
Reversi game(boardSize);//or you could prompt for the board size with Reversi r(AskUserForBoardSize());
std::pair<int,int> coordinate;
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

int handlePregameInput(int client){
	while(1){
		//const std::string input = GetInput();
		
		char input[30];
		int blorg = recv(client, input, 30, 0) << '\n';
		
		if(input=="EXIT")
			return 0;
		//if(input=="BLACK"){
		if( strncmp(input, "BLACK", 5) == 0 ) {
			player = player1;
			AIPlayer=player2;
			std::cout<<"BLACK\n";
			send(client, "BLACK\n", 10, 0);
		}
		//if(input=="WHITE"){
		if( strncmp(input, "WHITE", 5) == 0) {
			player = player2;
			AIPlayer=player1;
			std::cout<<"WHITE\n";
			send(client, "WHITE\n", 10, 0);
		}
		//if(input=="EASY"){
		if( strncmp(input, "EASY", 4) == 0) {
			AIlevel="EASY";
			std::cout<<"OK\n";
			send(client, "OK\n", 10, 0);
		}
		//if(input=="MEDIUM"){
		if( strncmp(input, "MEDIUM", 5) == 0) {
			AIlevel="MEDIUM";
			std::cout<<"OK\n";
			send(client, "OK\n", 10, 0);
		}
		//if(input == "HARD") {
		if( strncmp(input, "HARD", 4) == 0) {
			AIlevel="HARD";
			std::cout<<"OK\n";
			send(client, "OK\n", 10, 0);
		}
		//if(input=="DISPLAY_ON" || input =="1"){
		if( strncmp(input, "DISPLAY_ON", 10) == 0 || strncmp(input, "1", 1) == 0) {
			displayOn = true;
			std::cout<<"OK\n";
			send(client, "OK\n", 10, 0);
			break;
		}
		//if(input=="DISPLAY_OFF"){
		if( strncmp(input, "DISPLAY_OFF", 11) == 0) {
			displayOn = false;
			std::cout<<"OK\n";
			send(client, "Ok\n", 10, 0);
			break;
		}
		//if(input=="4X4"){
		if( strncmp(input, "4X4", 3) == 0) {
			std::cout<<"OK\n";
			send(client, "OK\n", 10, 0);
			boardSize=4;
		}
		//if(input=="8X8"){
		if( strncmp(input, "8X8", 3) == 0) {
			std::cout<<"OK\n";
			send(client, "OK\n", 10, 0);
			boardSize=8;
		}
		//if(input=="?"){
		if( strncmp(input, "?", 1) == 0) {
			std::cout<<"WHITE, BLACK, EASY, MEDIUM, HARD, DISPLAY_ON, EXIT\n";
			send(client, "WHITE, BLACK, EASY, MEDIUM, HARD, DISPLAY_ON, EXIT\n", 100, 0);
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

int handleGameInput(int client){
	while(1){
		std::cout<<"Current Player:"<<player<<"\n";
		
		std::ostringstream playerStream;
		playerStream << player;
		std::string playerStr = "Current Player:" + playerStream.str() + "\n";
		const char* playerChr = playerStr.c_str();
		
		send(client, playerChr, 30, 0);
		if(displayOn==true)
			std::cout<<game;
			
			//snippet that converts the game board to a string/c_string
			std::ostringstream boardStream;
			boardStream << game;
			std::string boardStr = boardStream.str();
			const char* boardChr = boardStr.c_str();
			
			//std::cout << "++++++++++++++++++++++\n";
			//std::cout << game.toString();
			//std::cout << "++++++++++++++++++++++\n";
			
			//sends the board to the client
			//send(client, boardChr, 10, 0);
			send(client, game.toString().c_str(), 200, 0);
			
		if(AIlevel!="OFF"&&player==AIPlayer){
			moveRandomly();
		}
		else {
			//const std::string input = GetInput();
			char input[30];
			recv(client, input, 30, 0);
			const bool isValidCoordinate = IsCoordinate(input, coordinate); 
			if (isValidCoordinate == false) 
			{
				if(input=="EXIT")
					return 0;
				else if(input=="?"){
					std::cout<<"Enter coordinates as # alpha values, DISPLAY_OFF, SHOW_NEXT_POS, UNDO, REDO, EXIT\n";
					send(client, "Enter coordinates as # alpha values, DISPLAY_OFF, SHOW_NEXT_POS, UNDO, REDO, EXIT\n", 100, 0);
					continue;
				}
				else if(input=="DISPLAY_OFF"){
					displayOn = false;
					std::cout<<"OK\n";
					send(client, "OK\n", 4, 0);
					continue;
				}
				if(input=="UNDO"){
					if(!game.DoUndo()){
						assert(!"CANNOT UNDO, NOT ENOUGH STATES IN STACK");
						std::cout<<"ILLEGAL\n";
						send(client, "ILLEGAL\n", 10, 0);
						continue;
					}continue;
				}else if(input=="REDO"){
					if(!game.DoRedo()){
						assert(!"CANNOT REDO, NOT ENOUGH STATES IN STACK");
						std::cout<<"ILLEGAL\n";
						send(client, "ILLEGAL\n", 10, 0);
						continue;
					}continue;
				}else if(input=="SHOW_NEXT_POS"){
					showPossibleMoves();
					continue;
				}else if(input=="RAND"){
					moveRandomly();
				}else{
					std::cout << "ILLEGAL\n";
					send(client, "ILLEGAL\n", 10, 0);
					continue;
				} 
			}
		} 
		if (game.IsValidMove(coordinate.first, coordinate.second, player) == false) 
		{ 
		  std::cout << "ILLEGAL\n";
		  send(client, "ILLEGAL\n", 10, 0);
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

class gameStart {

public:

	int api(std::string commandLine, int client)
	{
		for(int i=0;i<100;i++)std::cout<<"\n";
		send(client, "WELCOME\n", 8, 0);
		std::cout<< "WELCOME\n";
		if(!handlePregameInput(client))
			return 0;
		std::cout<<"Player1"<<"BLACK"<<player1<<"\n"<<RESET;
		std::cout<<"Player2"<<"WHITE"<<player2<<"\n"<<RESET;
		
		std::string message = "Player1 BLACK @\n";
		send(client, message.c_str(), 30, 0);
		message = "Player2 WHITE O\n";
		send(client, message.c_str(), 30, 0);
		
		if(!handleGameInput(client))
			return 0;
	}

};
