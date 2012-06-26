/*--------------------------------------------------------------------------- 
	Project2 CSCE 315
	Laramie Goode, Chris Voss, and Curtis Franks
    Based on the Reversi Console by
	Richel Bilderbeek 
	http://www.richelbilderbeek.nl/GameReversiConsoleSource_1_0.htm
---------------------------------------------------------------------------*/
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <numeric> 
#include <algorithm> 
#include <iostream> 
#include <cassert> 
#include <iterator> 
#include <memory> 
#include <sstream> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>//for seeding srand()
#include <limits>//maxlimit used in cin.ignore.get pause
#include <cctype>//for upper to lower
#include "UnitReversi.h"//#includes <vector>
//#include <WinSock2.h>
#include "alphaBetaAI.h"

//Global Variables //Default case, P1=Black=Human, P2=WHITE=EASYAI. //P1 ALWAYS goes first
int boardSize=8,randomMove,maxDepth=2,testMaxDepth=4,totalExecutions=1,blackWins=0,whiteWins=0;
bool displayOn=true,test=false,server=false;
Square CurrentPlayer=player1; //Indicates whose turn it currently is. Game always starts with P1, who is always BLACK.
const std::string defaultAISetting="EASY";
std::string AIlevelP1 = "OFF"; //Defaults P2 'OFF' i.e. P2==Human.
std::string AIlevelP2 = defaultAISetting; ////Defaults P2 to EASY AI.   EASY, MEDIUM, HARD, HARDdebg, ... { P1EASY, P2HARD, P2HARD3,...}(commands. ie to make player2 a hard level AI that looks ahead 3 plys)
std::string NullString="NULL";
std::pair<int,int> coordinate,bestMove;
Reversi newBoard(boardSize);
Reversi game(boardSize);

//used to determine if the passed player is one of the AIs
bool PlayerIsAI(Square Player){
    if(Player==player1){return (AIlevelP1!="OFF"); }
    else if(Player==player2){return (AIlevelP2!="OFF");}
    else{
        std::cerr<<"Error: Player passed into --bool isPlayerAI(Square Player)-- must be either player1 or player2.\n";
        return false;
    }
} 
bool isPlayerAI(Square Player){return PlayerIsAI(Player);}

//returns the name of the AI as a string, EASY, MEDIUM, HARD
std::string& AIlevel(Square Player){ // Getter/Setter  i.e. void MakeP1Human(){ AIlevel(player1)="OFF";}
    if(!isPlayerAI(Player)){std::cerr<<"Error: Player passed into --std::string& AIlevel(Square Player)-- is not AI.\n"; return NullString;}
    if(Player==player1){return AIlevelP1;}
    else if(Player==player2){return AIlevelP2;}
    else{ std::cerr<<"Error: Player passed into --std::string& AIlevel(Square Player)-- must be either player1 or player2.\n"; return NullString;}
}

//<< override for Reversi object squares
std::ostream& operator<<(std::ostream& os, const Square s) { 
    switch(s){
        case empty  : os <<GREEN<<"_"; break; 
        case validMove: os <<BOLDYELLOW<<"x"<<RESET<<GREEN; break;
        case player1: os <<BOLDBLACK<<"O"<<RESET<<GREEN; break; //char(2); break; 
        case player2: os <<BOLDWHITE<<"@"<<RESET<<GREEN; break; //char(1); break; 
        default: assert(!"Should not get here"); break; 
    }
    return os; 
}

//<< override for Reversi object board outlay
std::ostream& operator<<(std::ostream& os, const Reversi& r) { 
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

const std::string GetInput(int client) {
	if(client != 0) {
		char input[30];
		std::string s = "";
		recv(client, input, 30, 0);
	
		for(int i = 0; input[i] != '\n'; i++) {
			s = s + input[i];
		}
	
		return s;
	} else {
		string input;
		std::cin >> input;
		return input;
	}
	
}

//Handles all input and drops the newline char.  Either recv or getlines.
/*const std::string GetInput(int client){
	char input[30],c;
	std::string s; 
	if(server==true){
		//recv(client, input, sizeof(input),0);
		s=input;
	}else{
		std::getline(std::cin,s,'\n'); 
	}
	for(int i=0;i<s.size();i++){
		c=s[i];
		s[i]=toupper(c);
	}
	return s; 
}*/

//Handles all output.  Takes a string and either cout or sends
void PrintOut(std::string inString,int client){

	char* charToSend = new char[inString.size()];
	//char charToSend[80];
	strcpy(charToSend,inString.c_str());
	if(server==true){
		//send(client,charToSend+'\n',sizeof(charToSend+'\n'),0);
	}else {
		std::cout<<inString;
	}

	delete [] charToSend;
}

//Breaks up the coordinate input for an x and y value
const std::vector<std::string> SeperateString(std::string input, const char seperator){
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
const bool IsCoordinate(const std::string& input, std::pair<int,int>& coordinate){
    if((input.size()!=3 && server) || (input.size()!=2 && !server)) return false;
	//TODO: need error (bounds) checking on x and y
    int x, y;
    x = input[0] - 'A';
    y = int(input[1] - '0')-1;
    coordinate.first=x;
	coordinate.second=y;
    return true;
} 

//Optional menu option allowing for board sizes between 4X4 and 16X16
/*const int AskUserForBoardSize(int client){
  //Get the board's size 
  while (1){
    std::cout << "Please enter the size of the reversi board" << std::endl; 
    const std::string input = GetInput(client); 
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
}*/

const int AskUserForBoardSize(int client){
  //Get the board's size 
  while (1){
    std::cout << "Please enter the size of the reversi board" << std::endl; 
	send(client, "Please enter the size of the reversi board\n", 43, 0);
    const std::string input = GetInput(client); 
    int size = -1; 
    if ( IsInt(input,size) == false) 
    { 
      std::cout << "Please enter an integer value. " << std::endl; 
	  send(client, "Please enter an integer value. \n", 32, 0);
      continue; 
    } 
    if ( size < 4) 
    { 
      std::cout << "Please enter an integer value bigger than 4. " << std::endl; 
	  send(client, "Please enter an integer value bigger than 4. \n", 47, 0);
      continue; 
    } 
    if ( size > 16) 
    { 
      std::cout << "Please enter an integer value less than 16. " << std::endl; 
	  send(client, "Please enter an integer value less than 16. \n", 45, 0);
      continue; 
    } 
    return size; 
  } 
}

//inserts the possibleMove symbol into the proper locations of the board
/*void showPossibleMoves(){
	Reversi tempValid(boardSize);
	tempValid.SetBoard(game.GetBoard());
	std::vector< std::pair<int,int> > vals = tempValid.GetValidMoves(CurrentPlayer);
	for(int i=0; i<vals.size(); i++)
		tempValid.SetSquare(vals[i].first,vals[i].second,validMove);
	std::cout<<tempValid<<"\n";
}*/
//inserts the possibleMove symbol into the proper locations of the board
void showPossibleMoves(int client){
	Reversi tempValid(boardSize);
	tempValid.SetBoard(game.GetBoard());
	std::vector< std::pair<int,int> > vals = tempValid.GetValidMoves(CurrentPlayer);
	for(int i=0; i<vals.size(); i++)
		tempValid.SetSquare(vals[i].first,vals[i].second,validMove);
	std::cout<<tempValid<<"\n";
	
	ostringstream osTempValid;
	osTempValid << tempValid;
	
	std::string sTempValid = osTempValid.str();
	send(client, sTempValid.c_str(), sTempValid.size(), 0);
	
}


//uses the current time as a randomizing seed to select from available moves
void moveRandomly(){
	srand ( time(NULL) );
	Reversi tempValid(boardSize);
	tempValid.SetBoard(game.GetBoard());
	std::vector< std::pair<int,int> > vals = tempValid.GetValidMoves(CurrentPlayer);
	int randomMove = rand() % vals.size();
	coordinate.first=vals[randomMove].first;coordinate.second=vals[randomMove].second;
}

//takes input from the user that will set definations for the game settings
int handlePregameInput(int client){
	while(1){
	
		const std::string input = GetInput(client);

		if( strncmp(input.c_str(), "EXIT", 4) == 0) {
			return 0;
		}

		else if( strncmp(input.c_str(), "DISPLAY_ON", 10) == 0) {
			displayOn = true;
			std::cout<<"OK\n";
			send(client, "OK\n", 3, 0);
			break;
		}

		else if( strncmp(input.c_str(), "DISPLAY_OFF", 11) == 0) {
			displayOn = false;
			std::cout<<"OK\n";
			send(client, "OK\n", 3, 0);
			break;
		}
		
		else if( strncmp(input.c_str(), "BLACK", 5) == 0) {
			AIlevelP2=AIlevelP1;
		    AIlevelP1="OFF";
			std::cout<<"BLACK\n";
			send(client, "BLACK\n", 6, 0);
		}

		else if( strncmp(input.c_str(), "WHITE", 5) == 0) {
		    AIlevelP1=AIlevelP2;
		    AIlevelP2="OFF";
			std::cout<<"WHITE\n";
			send(client, "WHITE\n", 6, 0);
		}
		
		else if( strncmp(input.c_str(), "HARD_V_HARD", 11) == 0) {
			AIlevelP1="HARD";
			AIlevelP2="HARD";
			std::cout<<"HARD V HARD\n";
			send(client, "HARD V HARD\n", 12, 0);
		}

		else if( strncmp(input.c_str(), "HARD_V_MEDIUM", 13) == 0) {
			AIlevelP1="HARD";
			AIlevelP2="MEDIUM";
			std::cout<<"HARD V MEDIUM\n";
			send(client, "HARD V MEDIUM\n", 14, 0);
		}
		
		else if( strncmp(input.c_str(), "HARD_V_EASY", 11) == 0) {
			AIlevelP1="HARD";
			AIlevelP2="EASY";
			std::cout<<"HARD V EASY\n";
			send(client, "HARD V EASY\n", 12, 0);
		}
		
		else if( strncmp(input.c_str(), "MEDIUM_V_MEDIUM", 15) == 0) {
				AIlevelP1="MEDIUM";
				AIlevelP2="MEDIUM";
				std::cout<<"MEDIUM V MEDIUM\n";
				send(client, "MEDIUM V MEDIUM\n", 16, 0);			
		}
		
		else if( strncmp(input.c_str(), "MEDIUM_V_EASY", 13) == 0) {
			AIlevelP1="MEDIUM";
			AIlevelP2="EASY";
			std::cout<<"MEDIUM V EASY\n";
			send(client, "MEDIUM V EASY\n", 14, 0);
		}
		
		else if( strncmp(input.c_str(), "EASY_V_EASY", 11) == 0) {
			AIlevelP1="EASY";
			AIlevelP2="EASY";
			std::cout<<"EASY V EASY\n";
			send(client, "EASY v EASY\n", 12, 0);		
		}
			
		else if( strncmp(input.c_str(), "NO_AI", 5) == 0) {
		    AIlevelP1="OFF";
		    AIlevelP2="OFF";
		    std::cout<<"OK\n";
			send(client, "OK\n", 3, 0);
		}
		
		else if( strncmp(input.c_str(), "EASY", 4) == 0) {
		    Square aiPlayer = ((PlayerIsAI(player2))?(player2):(player1));
		    AIlevel(aiPlayer) ="EASY";
			std::cout<<"OK\n";
			send(client, "OK\n", 3, 0);
		}
		
		else if( strncmp(input.c_str(), "MEDIUM", 6) == 0) {
		    Square aiPlayer = ((PlayerIsAI(player2))?(player2):(player1));
		    AIlevel(aiPlayer) ="MEDIUM";
			std::cout<<"OK\n";
			send(client, "OK\n", 3, 0);
		}
		
		else if( strncmp(input.c_str(), "HARD", 4) == 0) {
		    Square aiPlayer = ((PlayerIsAI(player2))?(player2):(player1));
		    AIlevel(aiPlayer) ="HARD";
			std::cout<<"OK\n";
			send(client, "OK\n", 3, 0);
		}
		else if(input[0]=='P'){
		    if(input[1]=='1'){
		        AIlevelP1=input.substr(2);
                std::cout<<"OK\n";
				send(client, "OK\n", 3, 0);
		    }
		    else if(input[1]=='2'){
		        AIlevelP2=input.substr(2);
                std::cout<<"OK\n";
				send(client, "OK\n", 3, 0);
		    }
		    else{std::cout<<"ILLEGAL\n"; send(client, "ILLEGAL\n", 7, 0);}
		}
		//else if(input=="4X4"){
		else if( strncmp(input.c_str(), "4X4", 3) == 0) {
			std::cout<<"OK\n";
			send(client, "OK\n", 3, 0);
			boardSize=4;
		}

		//else if(input=="8X8"){
		else if( strncmp(input.c_str(), "8X8", 3) == 0) {
			std::cout<<"OK\n";
			send(client, "OK\n", 3, 0);
			boardSize=8;
		}
		//else if(input=="6"){
		else if( strncmp(input.c_str(), "6", 1) == 0) {
		    AIlevelP1="MEDIUM";
		    AIlevelP2="OFF";
		    std::cout<<"OK\n";
			send(client, "OK\n", 3, 0);
			std::cout<<"P1:HARD-AI, P2:Human\n";
			send(client, "P1:HARD-AI, P2:Human\n", 21, 0);
		    displayOn=true;
			break;
		}
		//else if(input=="7"){
		else if( strncmp(input.c_str(), "7", 1) == 0) {
		    AIlevelP2="MEDIUM";
		    AIlevelP1="OFF";
		    std::cout<<"OK\n";
			send(client, "OK\n", 3, 0);
			std::cout<<"P1:Human, P2:HARD-AI\n";
			send(client, "P1:Human, P2:HARD-AI\n", 21, 0);
		    displayOn=true;
			break;
		}
		//else if(input=="8"){
		else if( strncmp(input.c_str(), "8", 1) == 0) {
		    AIlevelP1="MEDIUM";
		    AIlevelP2="HARD";
		    std::cout<<"OK\n";
			std::cout<<"P1:"<<AIlevelP1<<"-AI, P2:"<<AIlevelP2<<"-AI\n";
		    displayOn=true;
			break;
		}
		//else if(input=="88"){
		else if( strncmp( input.c_str(), "88", 2) == 0) {
		    AIlevelP1="MEDIUM_DEBG";
		    AIlevelP2="HARD";
		    std::cout<<"OK\n";
			std::cout<<"P1:"<<AIlevelP1<<"-AI, P2:"<<AIlevelP2<<"-AI\n";
		    displayOn=true;
			break;
		}
		else{
			//if(input!="?"){std::cout<<"ILLEGAL\n";}
			if( strncmp(input.c_str(), "?", 1) != 0) {std::cout<<"ILLEGAL\n"; send(client, "ILLEGAL\n", 8, 0);}
			std::cout<<"WHITE, BLACK, EASY, MEDIUM, HARD, DISPLAY_ON, EXIT\n"; //TODO: Add more..
			send(client, "WHITE, BLACK, EASY, MEDIUM, HARD, DISPLAY_ON, EXIT\n", 51, 0);
		}		
	}
	return 1;
}

//forward decleration
std::pair<int,int> findBestMoveZ(Square forecastPlayer,int depth);

//forward decleration
std::pair<int,int> findBestMoveY(Square forecastPlayer,int depth);

//receives all input during the execution of the game
int handleGameInput(int client){
	int MoveCount=0,numOfGamesCompleted=0,blackWins=0,whiteWins=0;
    while(numOfGamesCompleted<totalExecutions){
		
        if(displayOn){
            std::cout<<"-------------------\n";
			std::cout<<"Move:"<<MoveCount<<"\n";
            std::cout<<"Current Player:"<<CurrentPlayer<<"\n";
            std::cout<<((PlayerIsAI(CurrentPlayer))?("Waiting on AI"):("Human's Move"))<<"\n";
            std::cout<< game;
			
			
			ostringstream osCurrentPlayer;
			ostringstream osGame;
			osCurrentPlayer	<< CurrentPlayer;
			osGame << game;
			
			std::string sCurrentPlayer = "Current Player: " + osCurrentPlayer.str() + '\n';
			std::string sGame = osGame.str();
		
			send(client, "\n-------------------\n", 21, 0);
			send(client, sCurrentPlayer.c_str(), sCurrentPlayer.size(), 0);
			
			if(PlayerIsAI(CurrentPlayer)) {
				send(client, "Waiting on AI\n", 14, 0);
			} else {
				send(client, "Human's Move\n", 13, 0);
			}
			
			send(client, sGame.c_str(), sGame.size(), 0);
			
        }
        //Check if Current Player can actually make a move 
        if (game.GetValidMoves(CurrentPlayer).empty()==true){
			if(game.GetValidMoves(GetOtherPlayer(CurrentPlayer)).empty()==true){
				//no player has a move available - end game
				const int n1 = game.Count(player1); 
				const int n2 = game.Count(player2); //TODO: ADD IN TIE CASE
				std::string p1Name = (PlayerIsAI(player1))?(AIlevelP1+"-AI"):("Human");
				std::string p2Name = (PlayerIsAI(player2))?(AIlevelP2+"-AI"):("Human");
				stringstream ss;
				std::cout<< "The game has ended after "<<MoveCount<<" moves!\n"
						<<"Player1 ("<<p1Name<<")["<<player1<<"] conquered "<<n1<<" squares.\n"
						<<"Player2 ("<<p2Name<<")["<<player2<<"] conquered "<<n2<<" squares.\n";
				if(n1==n2){
					PrintOut("It's a Draw!\n",client);
					send(client, "The game has ended\n", 19, 0);
			
					ostringstream osP1Name;
					ostringstream osP2Name;
					
					ostringstream osPlayer1;
					ostringstream osPlayer2;
					
					ostringstream osN1;
					ostringstream osN2;
					
					osP1Name << p1Name;
					osP2Name << p2Name;
					
					osPlayer1 << player1;
					osPlayer2 << player2;
					
					osN1 << n1;
					osN2 << n2;
					
					std::string p1SquaresConquered = "Player1 (" + osP1Name.str() + ")[" + osPlayer1.str() + "] conquered " + osN1.str() + " squares.\n";
					std::string p2SquaresConquered = "Player2 (" + osP2Name.str() + ")[" + osPlayer2.str() + "] conquered " + osN2.str() + " squares.\n";
					
					send(client, p1SquaresConquered.c_str(), p1SquaresConquered.size(), 0);
					send(client, p2SquaresConquered.c_str(), p2SquaresConquered.size(), 0);
					
					send(client, "It was a draw!\n", 15, 0);
					
					
				}
				else{
					std::cout<<"The winner is Player"<<((n1>n2)?("1("+p1Name+")"):("2("+p2Name+")"))
						<<"\nCongratulations!\n\n";
					if(n1>n2)
						blackWins++;
					else
						whiteWins++;
				
					stringstream ss;
					ss << "The game has ended after "<<MoveCount<<" moves!\n";
					
					send(client, ss.str().c_str(), ss.str().size(), 0);
			
					ostringstream osP1Name;
					ostringstream osP2Name;
					
					ostringstream osPlayer1;
					ostringstream osPlayer2;
					
					ostringstream osN1;
					ostringstream osN2;
					
					osP1Name << p1Name;
					osP2Name << p2Name;
					
					osPlayer1 << player1;
					osPlayer2 << player2;
					
					osN1 << n1;
					osN2 << n2;
					
					std::string p1SquaresConquered = "Player1 (" + osP1Name.str() + ")[" + osPlayer1.str() + "] conquered " + osN1.str() + " squares.\n";
					std::string p2SquaresConquered = "Player2 (" + osP2Name.str() + ")[" + osPlayer2.str() + "] conquered " + osN2.str() + " squares.\n";
					
					send(client, p1SquaresConquered.c_str(), p1SquaresConquered.size(), 0);
					send(client, p2SquaresConquered.c_str(), p2SquaresConquered.size(), 0);
					
					std::string p1Wins = "The winner is Player1(";
					std::string p2Wins = "The winner is Player2(";
					
					if(n1 > n2) {
						p1Wins = p1Wins + p1Name + ")";
						send(client, p1Wins.c_str(), p1Wins.size(), 0);
					} else {
						p2Wins = p2Wins + p2Name + ")";
						send(client, p2Wins.c_str(), p2Wins.size(), 0);
					}
					
					send(client, "\nCongratulations!\n\n", 19, 0);
					
				
				}
				
				
				
				
				if(totalExecutions>1){
					std::stringstream winCountStringStream;
					winCountStringStream<<"Player1 ("<<p1Name<<") won "<<blackWins<<"\n"<<"Player2 ("<<p2Name<<")won "<<whiteWins<<"\n";
					PrintOut(winCountStringStream.str(),client);
				 numOfGamesCompleted++;
				 MoveCount=0;
				game.SetBoard(newBoard.GetBoard());
				 continue;
				}
			}else{
				//If Current Player cannot move, but other player can
				std::cout<<"Too bad! Player"<<int(CurrentPlayer)<<"("<<CurrentPlayer<<") is unabled to do a valid move!\n"; 
				CurrentPlayer=GetOtherPlayer(CurrentPlayer);
				std::cout<<"The next turn again goes to Player"<<int(CurrentPlayer)<<"("<<CurrentPlayer<<") !\n";
				
				ostringstream osMessage1;
				osMessage1 << "Too bad! Player"<<int(CurrentPlayer)<<"("<<CurrentPlayer<<") is unabled to do a valid move!\n"; 
				
				send(client, osMessage1.str().c_str(), osMessage1.str().size(), 0);
				
				CurrentPlayer=GetOtherPlayer(CurrentPlayer);
				ostringstream osMessage2;			
				std::cout<<"The next turn again goes to Player"<<int(CurrentPlayer)<<"("<<CurrentPlayer<<") !\n";
				osMessage2 << "The next turn again goes to Player"<<int(CurrentPlayer)<<"("<<CurrentPlayer<<") !\n";
				send(client, osMessage2.str().c_str(), osMessage2.str().size(), 0);
				
				
				continue;
			}
        }
        MoveCount++;
        {//Input-Gathering Block
            if(PlayerIsAI(CurrentPlayer)){
                //CurrentPlayer is an AI
                if(AIlevel(CurrentPlayer)=="EASY"){
                    moveRandomly();
                }
				else if(AIlevel(CurrentPlayer)=="MEDIUM"){
					AlphaBetaAI ai(game, CurrentPlayer, 3);
					coordinate = ai.findMax();
				}
				/*
                else if(AIlevel(CurrentPlayer)=="MEDIUM"){
					std::pair<int,int> bestMove = findBestMoveZ(CurrentPlayer,0);
					coordinate.first=bestMove.first;coordinate.second=bestMove.second;
                }
				*/
                else if(AIlevel(CurrentPlayer).substr(0,4)=="HARD"){
					maxDepth=2;
					std::pair<int,int> bestMove = findBestMoveY(CurrentPlayer,0);
					coordinate.first=bestMove.first;coordinate.second=bestMove.second;//38,26vsZ

                }
                std::cout<<AIlevel(CurrentPlayer)<<"-AI Plays:"<<char('A'+coordinate.first)<<coordinate.second+1<<"\n";
				
				
				ostringstream osAIlevel;
				ostringstream osCoordinates; 
				
				osAIlevel << AIlevel(CurrentPlayer);
				osCoordinates << char('A'+coordinate.first)<<coordinate.second+1;
				
				std::string message = osAIlevel.str() + "-AI Plays @:" + osCoordinates.str() + '\n';
				
				send(client, message.c_str(), message.size(), 0);
				
				
            }
            else if( !(PlayerIsAI(CurrentPlayer)) ) {
                //CurrentPlayer is a Human
                std::cout<<"::";
				send(client, "::", 2, 0);
                const std::string input = GetInput(client);
                const bool isValidCoordinate = IsCoordinate(input, coordinate); 
                if (isValidCoordinate == false){
                    //Input was not a Coordinate
                    //if(input=="EXIT") return 0;
					if( strncmp(input.c_str(), "EXIT", 4) == 0) return 0;
					// else if(input=="?"){
					else if( strncmp(input.c_str(), "?", 1) == 0) {
                        std::cout<<"Enter coordinates as # alpha values, DISPLAY_OFF, SHOW_NEXT_POS, UNDO, REDO, EXIT\n";
                        send(client, "Enter coordinates as # alpha values, DISPLAY_OFF, SHOW_NEXT_POS, UNDO, REDO, EXIT\n", 82, 0);
						continue;
                    }
                    //else if(input=="DISPLAY_OFF"){
					else if( strncmp(input.c_str(), "DISPLAY_OFF", 11) == 0) {
                        displayOn = false;
                        std::cout<<"OK\n";
						send(client, "OK\n", 3, 0);
                        continue;
                    }
                    //else if(input=="UNDO"){
					else if( strncmp(input.c_str(), "UNDO", 4) == 0) {
                        if(!game.DoUndo()){
                            std::cerr<<"CANNOT UNDO, NOT ENOUGH STATES IN STACK\n";
                            std::cout<<"ILLEGAL\n";
							send(client, "ILLEGAL\n", 8, 0);
                            continue;
                        }
                        continue;
                    }
                    //else if(input=="REDO"){
					else if( strncmp(input.c_str(), "REDO", 4) == 0) {
                        if(!game.DoRedo()){
                            std::cerr<<"CANNOT REDO, NOT ENOUGH STATES IN STACK\n";
                            std::cout<<"ILLEGAL\n";
							send(client, "ILLEGAL\n", 8, 0);
                            continue;
                        }
                        continue;
                    }
                    //else if(input=="SHOW_NEXT_POS"){
					else if( strncmp(input.c_str(), "SHOW_NEXT_POS", 13) == 0) {;
                        showPossibleMoves(client);
                        continue;
                    }
                    //else if(input=="RAND" || input=="R"){
					else if( strncmp(input.c_str(), "RAND", 4) == 0 || strncmp(input.c_str(), "R", 1) == 0 ) {
                        moveRandomly();
                    }
                    else{
                        std::cout << "ILLEGAL\n";
						send(client, "ILLEGAL\n", 8, 0);
                        continue;
                    }
                    
					/*
					if(input=="EXIT") return 0;
                    else if(input=="?"){
                        std::cout<<"Enter coordinates as # alpha values, DISPLAY_OFF, SHOW_NEXT_POS, UNDO, REDO, EXIT\n";
                        continue;
                    }
                    else if(input=="DISPLAY_OFF"){
                        displayOn = false;
                        std::cout<<"OK\n";
                        continue;
                    }
                    else if(input=="UNDO"){
                        if(!game.DoUndo()){
                            std::cerr<<"CANNOT UNDO, NOT ENOUGH STATES IN STACK\n";
                            std::cout<<"ILLEGAL\n";
                            continue;
                        }
                        continue;
                    }
                    else if(input=="REDO"){
                        if(!game.DoRedo()){
                            std::cerr<<"CANNOT REDO, NOT ENOUGH STATES IN STACK\n";
                            std::cout<<"ILLEGAL\n";
                            continue;
                        }
                        continue;
                    }
                    else if(input=="SHOW_NEXT_POS"){
                        showPossibleMoves();
                        continue;
                    }
                    else if(input=="RAND" || input=="R"){
                        moveRandomly();
                    }
                    else{
                        std::cout << "ILLEGAL\n";
                        continue;
                    }*/
                }
                if(isValidCoordinate){
                   //Valid coordinate was entered
                    if(displayOn){
                        std::cout<<"Human Plays :"<<char('A'+coordinate.first)<<coordinate.second+1<<"\n";
						
						ostringstream osCoordinates;
						osCoordinates<< "Human Plays @:"<< char('A'+coordinate.first)<<coordinate.second+1<<"\n";
						send(client, osCoordinates.str().c_str(), osCoordinates.str().size(), 0);
                    }
                }
            }//End Human Input
        }//End of Input-Gathering Block
        
        //Check that move is valid for Current Player.
        if (game.IsValidMove(coordinate.first, coordinate.second, CurrentPlayer) == false){
            std::cout << "ILLEGAL\n";
			send(client, "ILLEGAL\n", 8, 0);
            continue; 
        }

        //Actually do the move 
        game.DoMove(coordinate.first, coordinate.second, CurrentPlayer);

        //Switch Player's turns
        CurrentPlayer=GetOtherPlayer(CurrentPlayer); 
        if(displayOn){std::cout<<"-------------------\n\n"; send(client, "-------------------\n\n", 21, 0);}

    }
    return 1;
}


class gameStart {

public:

//Handles user input and display of data 
	int api(std::string commandLine,int client){
		if(client <= 0) {
			server = false;
		} else {
			server = true;
		}
		for(int i=0;i<40;i++)std::cout<<"\n";
		std::cout<< "WELCOME\n";
		send(client, "WELCOME\n", 8, 0);
		while(1){

			if(!handlePregameInput(client)){return 0;}
	
			std::cout<<"Player1: "<<((PlayerIsAI(player1))?("AI    :"):("Human :"))<<player1<<": BLACK\n"<<RESET;
			std::cout<<"Player2: "<<((PlayerIsAI(player2))?("AI    :"):("Human :"))<<player2<<": WHITE\n"<<RESET;
			
			ostringstream ssPlayer1;
			ostringstream ssPlayer2;
			std::string infoPlayer1;
			std::string infoPlayer2;
			
			if(PlayerIsAI(player1)) {
				ssPlayer1 << player1;
				infoPlayer1 = "Player1: AI\t:" + ssPlayer1.str() + ": BLACK\n";
			} else {
				ssPlayer1 << player1;
				infoPlayer1 = "Player1: Human\t:" + ssPlayer1.str() + ": BLACK\n";
			}
		
			if(PlayerIsAI(player2)) {
				ssPlayer2 << player2;
				infoPlayer2 = "Player2: AI\t:" + ssPlayer2.str() + ": WHITE\n";
			} else {
				ssPlayer2 << player2;
				infoPlayer2 = "Player2: Human\t:" + ssPlayer2.str() + ": WHITE\n";
			}
		
			send(client, infoPlayer1.c_str(), infoPlayer1.size(), 0);
			send(client, infoPlayer2.c_str(), infoPlayer2.size(), 0);
			
			if(!handleGameInput(client))
				return 0;
			std::cout << "Press ENTER to quit to continue.  Use the EXIT command a any time to quit.";
			send(client, "Press ENTER to quit to continue.  Use the EXIT command a any time to quit.\n", 75, 0);
						
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
			game.SetBoard(newBoard.GetBoard());
			//return 0;
		}
		return 0;
	}
};



/*
//Handles user input and display of data 
int api(std::string commandLine,int client){
		for(int i=0;i<40;i++)std::cout<<"\n";
		std::cout<< "WELCOME\n";
	while(1){

		if(!handlePregameInput(client)){return 0;}
	
		std::cout<<"Player1: "<<((PlayerIsAI(player1))?("AI    :"):("Human :"))<<player1<<": BLACK\n"<<RESET;
		std::cout<<"Player2: "<<((PlayerIsAI(player2))?("AI    :"):("Human :"))<<player2<<": WHITE\n"<<RESET;
		if(!handleGameInput(client))
			return 0;
		std::cout << "Press ENTER to continue.  Use the EXIT command at any time to quit.";
		std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
		game.SetBoard(newBoard.GetBoard());
	}
	return 0;
}

int main ( int argc, char *argv[] ){
	if(argc>3){
		// argc should be 1, 2, or 3 for correct execution
		// We print argv[0] assuming it is the program name
		cout<<"usage: "<< argv[0] <<" (<ExecutionCount>) (<SearchDepth>)\n";
		return -1;
	}
	else{
		if(argc==1){
			//no args provided, use default vals (exeCount=1,MaxDepth=4)
		}
		else if(argc>=2){
			//We assume argv[1] is an integer representing the number of executions to run
			char * endptr=NULL;
			int exeC = strtol(argv[1], &endptr, 10);
			if(endptr==NULL){
				cerr<<"Error in command arguments while processing <ExecutionCount>. Must be integer\n";
				return -1;
			}
			else if(*endptr==0){
				//totalExecutions = exeC;
			}
			else{
				   cerr<<"Error in command arguments while processing <ExecutionCount>. Invalid Character: "<<endptr<<"\n";
				   return -1;
			}
		}
		if(argc==3){
			//We assume argv[2] is an integer representing the Search Depth to use for AI
			char * endptr=NULL;
			int dpth = strtol(argv[2], &endptr, 10);
			if(endptr==NULL){
				cerr<<"Error in command arguments while processing <SearchDepth>. Must be integer\n";
				return -1;
			}
			else if(*endptr==0){
				maxDepth = dpth;
			}
			else{
				   cerr<<"Error in command arguments while processing <SearchDepth>. Invalid Character: "<<endptr<<"\n";
				   return -1;
			}
		}
	}
	int client=0;//temp value until implemented with server
	api("START",client);
	return 0;
}
*/

/**      HARD-AI      **/
/* Developed by Cutris */

//uses a health-number for the different parts of the board to determine a moves strength
int heuristicWeightZ(Reversi childBoard, int x, int y, Square moveOwner, int depth){
	int cummulative=0;
	//got a corner
	if(	(x==0&&y==0)||//top left
		(x==boardSize-1&&y==boardSize-1)||//bottom right
		(x==0&&y==boardSize-1)||//bottom left
		(x==boardSize-1&&y==0)){ //top right
		if(moveOwner==CurrentPlayer)
			return  5000;//player wants this bad
		else
			return -5000;//player definately doesn't want oppoent to have
	}
	//edge next to empty corner
	if( (((x==0&&y==1)||(x==1&&y==0))&&childBoard.GetSquare(0,0)==empty)||
		(((x==0&&y==6)||(x==1&&y==7))&&childBoard.GetSquare(0,7)==empty)||
		(((x==6&&y==0)||(x==7&&y==1))&&childBoard.GetSquare(7,0)==empty)||
		(((x==6&&y==7)||(7==1&&y==6))&&childBoard.GetSquare(7,7)==empty)){
			if(moveOwner==CurrentPlayer)
				return -1500;
			else
				return 1500;
	}
	//got an edge
	if( x==0||//left edge
		x==boardSize-1||//right edge
		y==0||//top edge
		y==boardSize-1){//bottom edge
		if(moveOwner==CurrentPlayer)
			return 1500;
		else
			return -1500;
	}
	//avoid these
	if( x==1||x==boardSize-2)
		if(moveOwner==CurrentPlayer)
			cummulative+= -1000;//in this case player doesn't want this move
		else
			cummulative+= 1000;//player would like his opponent to make this move
	if( y==1||y==boardSize-2)
		if(moveOwner==CurrentPlayer)
			cummulative+= -1000;//in this case player doesn't want this move
		else
			cummulative+= 1000;//player would like his opponent to make this move

	return cummulative;
}

//experimental version
int heuristicWeightY(Reversi childBoard, int x, int y, Square moveOwner, int depth){
	int cummulative=0;
	//got a corner
	if(	(x==0&&y==0)||//top left
		(x==boardSize-1&&y==boardSize-1)||//bottom right
		(x==0&&y==boardSize-1)||//bottom left
		(x==boardSize-1&&y==0)){ //top right
		if(moveOwner==CurrentPlayer)
			return  10000/(depth+1);//45,19@5000&10k&11k&15k
		else
			return -10000/(depth+1);//45,19@-5000&-10k&-11k&-15k
	}
	//edge next to empty corner
	if( (((x==0&&y==1)||(x==1&&y==0))&&childBoard.GetSquare(0,0)==empty)||
		(((x==0&&y==6)||(x==1&&y==7))&&childBoard.GetSquare(0,7)==empty)||
		(((x==6&&y==0)||(x==7&&y==1))&&childBoard.GetSquare(7,0)==empty)||
		(((x==6&&y==7)||(7==1&&y==6))&&childBoard.GetSquare(7,7)==empty)){
			if(moveOwner==CurrentPlayer)
				return -5000/(depth+1);//36,28@-3000/33,31@5000&6000/39,25@-7000
			else
				return 5000/(depth+1);//36,28@3000/33,31@5000&6000/39,25@7000
	}
	//safe edge next to corner
	if( (((x==0&&y==1)||(x==1&&y==0))&&childBoard.GetSquare(0,0)==moveOwner)||
		(((x==0&&y==6)||(x==1&&y==7))&&childBoard.GetSquare(0,7)==moveOwner)||
		(((x==6&&y==0)||(x==7&&y==1))&&childBoard.GetSquare(7,0)==moveOwner)||
		(((x==6&&y==7)||(x==7&&y==6))&&childBoard.GetSquare(7,7)==moveOwner)){
			if(moveOwner==CurrentPlayer)
				return 5000/(depth+1);//36,28@-3000/33,31@5000&6000/39,25@-7000
			else
				return -5000/(depth+1);//36,28@3000/33,31@5000&6000/39,25@7000
	}
	//sweet-sixteen corner
	if( ((x==2&&y==2)&&childBoard.GetSquare(0,0)==empty)||
		((x==2&&y==5)&&childBoard.GetSquare(0,7)==empty)||
		((x==5&&y==2)&&childBoard.GetSquare(7,0)==empty)||
		((x==5&&y==5)&&childBoard.GetSquare(7,7)==empty) ){
			if(moveOwner==CurrentPlayer)
				return 2500/(depth+1);
			else
				return -2500/(depth+1);
	}
	//deadman's corner
	if( ((x==1&&y==1)&&childBoard.GetSquare(0,0)==empty)||
		((x==1&&y==6)&&childBoard.GetSquare(0,7)==empty)||
		((x==6&&y==1)&&childBoard.GetSquare(7,0)==empty)||
		((x==6&&y==6)&&childBoard.GetSquare(7,7)==empty) ){
			if(moveOwner==CurrentPlayer)
				return -5000/(depth+1);
			else
				return 5000/(depth+1);
	}
	//got an edge, blocking a straightaway
	if( (x==0&&(y<boardSize-3&&y>2))&&
		childBoard.GetSquare(x+1,y)==GetOtherPlayer(moveOwner))
		if(moveOwner==CurrentPlayer)
			cummulative+= 1500/(depth+1);
		else
			cummulative+= -1500/(depth+1);
	if( (x==boardSize-1&&(y<boardSize-3&&y>2))&&
		childBoard.GetSquare(x-1,y)==GetOtherPlayer(moveOwner))
		if(moveOwner==CurrentPlayer)
			cummulative+= 1500/(depth+1);
		else
			cummulative+= -1500/(depth+1);
	if( (y==0&&(x<boardSize-3&&x>2))&&
		childBoard.GetSquare(x,y+1)==GetOtherPlayer(moveOwner))
		if(moveOwner==CurrentPlayer)
			cummulative+= 1500/(depth+1);
		else
			cummulative+= -1500/(depth+1);
	if( (y==boardSize-1&&(x<boardSize-3&&x>2))&&
		childBoard.GetSquare(x,y-1)==GetOtherPlayer(moveOwner))
		if(moveOwner==CurrentPlayer)
			cummulative+= 1500/(depth+1);
		else
			cummulative+= -1500/(depth+1);
	if( x==0||//left edge
		x==boardSize-1||//right edge
		y==0||//top edge
		y==boardSize-1){//bottom edge
		if(moveOwner==CurrentPlayer)
			cummulative+= 750/(depth+1);
		else
			cummulative-= 750/(depth+1);
	}
	//avoid these
	if( x==1||x==boardSize-2){
		if(moveOwner==CurrentPlayer)
			cummulative-= 1500/(depth+1);//in this case player doesn't want this move
		else
			cummulative+= 1500/(depth+1);//player would like his opponent to make this move
	}
	if( y==1||y==boardSize-2){
		if(moveOwner==CurrentPlayer)
			cummulative-= 1500/(depth+1);//in this case player doesn't want this move
		else
			cummulative+= 1500/(depth+1);//player would like his opponent to make this move
	}

	return cummulative;
}


//simply checks to see if the passed board is "game-over"
int endGameEvaluator(Reversi childBoard){
	Square opponent;
	if	(childBoard.Count(empty) == 0||//No more empty squares or niether player has a move, end game 
		((childBoard.GetValidMoves(player1).empty()==true)&&
		(childBoard.GetValidMoves(player2).empty()==true))){ 
			if(CurrentPlayer==player1)
				opponent=player2;
			else
				opponent=player1;
			if(childBoard.Count(CurrentPlayer)>childBoard.Count(opponent))
				return childBoard.Count(CurrentPlayer)*10000;
			else
				return childBoard.Count(opponent)*(-10000);
	}
	return 0;
}

//returns a positive weight if the move makes the opponent have to pass
//or a negative weight if the opponent move would make you have to pass
int numOfAvailableMovesEvaluator(Reversi childBoard,Square forecastPlayer){
	//evaluating a board that forecastPlayer has just played on
	Square nextPlayer = (forecastPlayer == player1 ? player2 : player1);
	if(childBoard.GetValidMoves(nextPlayer).empty()){
		if(CurrentPlayer==forecastPlayer)
			return 10000;
		else
			return -10000;
	}
	return 0;

}

//recursively builds a tree of moves and then uses heuristicWeightZ to 
//build a chain of weights that can be summed to findBestMove
int checkForWeightZ(Reversi parentBoard, Square forecastPlayer,int depth){
	//MaxMoveWeight is private to this branch of the tree
	//but publicly used to the childBoards (leaf-nodes)
	int MaxMoveWeight=-99999;

	std::vector< std::pair<int,int> >vals=parentBoard.GetValidMoves(forecastPlayer);

	//here we itterate through the moves, creating a private object for each
	//move that contains some private variables and a private boardState that
	//records the addition of the new move to the parentBoard that was passed in
	for(int possibleMove=0;possibleMove<vals.size();possibleMove++){
		int forecastedMoveWeight=0;
		Square nextPlayer;
		//this childBoard is private to this particular move
		//and will record this move as a new state of the board
		//to be passed recursively for examination of new moves
		//that will be available after this move.
		Reversi childBoard(boardSize);				
		childBoard.SetBoard(parentBoard.GetBoard());

		//evaluate and assign a weight to this move
		//forecastedMoveWeights is private to this move
		//and will be replaced when we itterate to the next
		//move of this stage
		forecastedMoveWeight+=heuristicWeightZ(	childBoard,
												vals[possibleMove].first,
												vals[possibleMove].second,
												forecastPlayer, depth);

		//now add the move to this private board
		childBoard.DoMove(	vals[possibleMove].first,
							vals[possibleMove].second,
							forecastPlayer);
		//we will pass a copy of this board recursively
		//here, we attempt to only recurse if necessary 
		if(	depth<maxDepth&&//if not at our maximum allowed recursion
			heuristicWeightZ(childBoard,//here we say that we don't want to bother
							vals[possibleMove].first,//checking further down this move's
							vals[possibleMove].second,//lineage if it involves the
							forecastPlayer, depth)!=-10000){//opponent taking a corner 
				nextPlayer = (forecastPlayer == player1 ? player2 : player1);
				forecastedMoveWeight+=checkForWeightZ(childBoard,nextPlayer,depth+1);
		}
/*		else{
			return childBoard.Count(CurrentPlayer)*100+endGameEvaluator(childBoard);
		}*///found the final board at this depth is not evaluated for each possible move

		//keep track of which move is the best at this stage of the branch
		//MaxMoveWeight is private to this stage of the branch
		//and will be returned as the maximum possible move weight
		if(forecastedMoveWeight>MaxMoveWeight)
			MaxMoveWeight=forecastedMoveWeight;	
		//stop itterating through the for loop looking at the next available
		//move, if we have already found a great move (alpha-beta, pruning)
		//we use a different acceptable weight at the beginning of the game.
		if(childBoard.Count(empty)>53){
			if(MaxMoveWeight>500*maxDepth)
				break;
		}else if(MaxMoveWeight>900*maxDepth)
			break;
		
	}
	return MaxMoveWeight+vals.size();//decided to add the num of avail
}

//experimental version
int checkForWeightY(Reversi parentBoard, Square forecastPlayer,int depth){
	//MaxMoveWeight is private to this branch of the tree
	//but publicly used to the childBoards (leaf-nodes)
	int MaxMoveWeight=-99999;

	std::vector< std::pair<int,int> >vals=parentBoard.GetValidMoves(forecastPlayer);
	std::vector<int> weights;	
//	if(!endGameEvaluator(parentBoard))
//		return endGameEvaluator(parentBoard);
	if(endGameEvaluator(parentBoard)!=0)
			return endGameEvaluator(parentBoard);

	//here we itterate through the moves, creating a private object for each
	//move that contains some private variables and a private boardState that
	//records the addition of the new move to the parentBoard that was passed in
	for(int possibleMove=0;possibleMove<vals.size();possibleMove++){
		int forecastedMoveWeight=0;
		Square nextPlayer;

		//this childBoard is private to this particular move
		//and will record this move as a new state of the board
		//to be passed recursively for examination of new moves
		//that will be available after this move.
		Reversi childBoard(boardSize);				
		childBoard.SetBoard(parentBoard.GetBoard());

		//evaluate and assign a weight to this move
		//forecastedMoveWeights is private to this move
		//and will be replaced when we itterate to the next
		//move of this stage
		forecastedMoveWeight+=heuristicWeightY(	childBoard,
												vals[possibleMove].first,
												vals[possibleMove].second,
												forecastPlayer, depth);

		//now add the move to this private board
		childBoard.DoMove(	vals[possibleMove].first,
							vals[possibleMove].second,
							forecastPlayer);
		if(numOfAvailableMovesEvaluator(childBoard,forecastPlayer)!=0)
			forecastedMoveWeight+=numOfAvailableMovesEvaluator(childBoard,forecastPlayer);
		//we will pass a copy of this board recursively
		//here, we attempt to only recurse if necessary 
		if(	depth<maxDepth&&//if not at our maximum allowed recursion
			heuristicWeightY(childBoard,//here we say that we don't want to bother
							vals[possibleMove].first,//checking further down this move's
							vals[possibleMove].second,//lineage if it involves the
							forecastPlayer, depth)!=-10000/(depth+1)){//opponent taking a corner 
				nextPlayer = (forecastPlayer == player1 ? player2 : player1);
				forecastedMoveWeight+=checkForWeightY(childBoard,nextPlayer,depth+1);

					
		}
/*		else{
			return childBoard.Count(CurrentPlayer)*100+endGameEvaluator(childBoard);
		}*///found the final board at this depth is not evaluated for each possible move

		//keep track of which move is the best at this stage of the branch
		//MaxMoveWeight is private to this stage of the branch
		//and will be returned as the maximum possible move weight
		if(forecastedMoveWeight>MaxMoveWeight)
			MaxMoveWeight=forecastedMoveWeight;	
		//stop itterating through the for loop looking at the next available
		//move, if we have already found a great move (alpha-beta, pruning)
		//we use a different acceptable weight at the beginning of the game.
		if(childBoard.Count(empty)>53){
			if(MaxMoveWeight>500*maxDepth)
				break;
		}else if(MaxMoveWeight>900*maxDepth)
			break;
/*		weights.push_back(forecastedMoveWeight);
		if(weights.size()==vals.size())
			std::cout<<possibleMove+1;*/
		
	}
/*	if(vals.size()==0&&parentBoard.Count(empty)>4)
		return MaxMoveWeight;
	else if(vals.size()==0&&parentBoard.Count(empty)>0)
		return -10000;
	else*/
		return MaxMoveWeight+vals.size()*100;//decided to add the num of avail
}

//takes the game board and creates a tree of moves, passing a game board with
//each immediately available move, to checkForWeightz
std::pair<int,int> findBestMoveZ(Square forecastPlayer,int depth){
	int maxMoveWeight=-99999,immediatePlusForecastWeights=0,maxPossibleMove=0;
	Square nextPlayer;
	//vals will hold the possible moves for the player we want to forcast at this depth
	std::vector< std::pair<int,int> >vals=game.GetValidMoves(forecastPlayer);
	for(int possibleMove=0;possibleMove<vals.size();possibleMove++){	
		int immediateMoveWeights=0;
		Reversi boardForAPrimaryMove(boardSize);
		//new board (branch to tree) for each possibleMove created each itteration
		boardForAPrimaryMove.SetBoard(game.GetBoard());
		//add this possible move to its board
		boardForAPrimaryMove.DoMove(vals[possibleMove].first,
									vals[possibleMove].second,
									forecastPlayer);
		immediateMoveWeights+=heuristicWeightZ(	boardForAPrimaryMove,
										vals[possibleMove].first,
										vals[possibleMove].second,
										forecastPlayer, depth);
		nextPlayer = (forecastPlayer == player1 ? player2 : player1);//figure out who is the next player
		immediatePlusForecastWeights+=checkForWeightZ(boardForAPrimaryMove,nextPlayer,1);//the maximum outcome from the set Depth (checkForWeightZ())
		if(immediatePlusForecastWeights>maxMoveWeight){	//if this possible Move's maximum outcome is bigger
			maxMoveWeight=immediatePlusForecastWeights;	//keep it as the new maximum overall
			maxPossibleMove=possibleMove;	//and remember which move that maximum belongs to
		}
	}
	std::cout<<"Value of this move:"<<maxMoveWeight<<std::endl;
	return vals[maxPossibleMove];//when done checking down each branch of possible Moves send the best move back

}

//experimental version
std::pair<int,int> findBestMoveY(Square forecastPlayer,int depth){
	srand ( time(NULL) );
	int maxMoveWeight=-99999,immediatePlusForecastWeights=0,maxPossibleMove=0;
	Square nextPlayer;
	std::vector< std::pair<int,int> >movesTiedForTop;
	std::vector<int> weights;
	//vals will hold the possible moves for the player we want to forcast at this depth
	std::vector< std::pair<int,int> >vals=game.GetValidMoves(forecastPlayer);
	for(int possibleMove=0;possibleMove<vals.size();possibleMove++){	
		int immediateMoveWeights=0;
		Reversi boardForAPrimaryMove(boardSize);
		//new board (branch to tree) for each possibleMove created each itteration
		boardForAPrimaryMove.SetBoard(game.GetBoard());
		//add this possible move to its board
		boardForAPrimaryMove.DoMove(vals[possibleMove].first,
									vals[possibleMove].second,
									forecastPlayer);
		immediateMoveWeights+=heuristicWeightY(	boardForAPrimaryMove,
										vals[possibleMove].first,
										vals[possibleMove].second,
										forecastPlayer, depth);
		nextPlayer = (forecastPlayer == player1 ? player2 : player1);//figure out who is the next player
		immediateMoveWeights+=checkForWeightY(boardForAPrimaryMove,nextPlayer,1);//the maximum outcome from the set Depth (checkForWeightZ())
		if(immediateMoveWeights>maxMoveWeight){	//if this possible Move's maximum outcome is bigger
//			if(movesTiedForTop.size()>0)
//				movesTiedForTop.clear();
//			movesTiedForTop.push_back(vals[possibleMove]);
			maxMoveWeight=immediateMoveWeights;	//keep it as the new maximum overall
			maxPossibleMove=possibleMove;	//and remember which move that maximum belongs to
		}//else if(immediatePlusForecastWeights==maxMoveWeight){
//			movesTiedForTop.push_back(vals[possibleMove]);
//		}
		weights.push_back(immediateMoveWeights);
	}
	std::cout<<"Value of this move:"<<maxMoveWeight<<std::endl;
//	int randomOfTheTopMoves = rand() % movesTiedForTop.size();
	return vals[maxPossibleMove];//when done checking down each branch of possible Moves send the best move back

}
