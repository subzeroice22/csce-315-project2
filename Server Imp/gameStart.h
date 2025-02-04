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
#include "AI.h"
//global variables - created to prepare for port to Java
int boardSize=8,randomMove,isHuman,maxDepth;
bool displayOn=true,test=false,server=true;
const std::string defaultAISetting = "EASY";
Square CurrentPlayer=player1; //Indicates whose turn it currently is. Game always starts with P1, who is always BLACK.
std::string NullString = "NULL";
std::string AIlevelP1 = "OFF";
std::string AIlevelP2 = defaultAISetting;
//std::string AIlevel = "OFF";
Square player=player1,AIPlayer=player2,humanPlayer=player1; 
Reversi game(boardSize);//or you could prompt for the board size with Reversi r(AskUserForBoardSize());
std::pair<int,int> coordinate,bestMove;
//<< override for Reversi object squares

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

std::ostream& operator<<(std::ostream& os, const Square s) { 
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
//Handles all input and drops the newline char
const std::string GetInput(int client) 
{ 
	char buffer[30];
	int a = recv(client, buffer, 30, 0);
	std::cout << buffer << '\n';
	char b;
	std::cin >> b;
	
	char c;
	std::string s; 
	std::getline(std::cin,s,'\n'); 
	for(int i=0;i<s.size();i++){
		c=s[i];
		s[i]=toupper(c);
	}

	return s; 
} 
//Handles all output.  Takes a string and either cout or sends
void PrintOut(std::string inString,int client){
	char charToSend[30];
	strcpy(charToSend,inString.c_str());
	if(server==true){
		send(client,charToSend+'\n',sizeof(charToSend+'\n'),0);
	}else {
		std::cout<<inString;
	}
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
const int AskUserForBoardSize(int client) 
{ 
  //Get the board's size 
  while (1) 
  { 
    std::cout << "Please enter the size of the reversi board" << std::endl; 
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

int handlePregameInput(int client){
	while(1){
		const std::string input = GetInput(client);
		if(input=="EXIT"){
			return 0;
		}
		else if(input=="DISPLAY_ON" ){
			displayOn = true;
			std::cout<<"OK\n";
			break;
		}
		else if(input=="DISPLAY_OFF"){
			displayOn = false;
			std::cout<<"OK\n";
			break;
		}
		else if(input=="BLACK"){
		    AIlevelP2=AIlevelP1;
		    AIlevelP1="OFF";
		}
		else if(input=="TEST_HARD"){
			AIlevelP1="HARD";
			AIlevelP2="HARD";
			std::cout<<"HARD V HARD\n";
		}
		else if(input=="TEST_MEDIUM"){
			AIlevelP1="HARD";
			AIlevelP2="MEDIUM";
			std::cout<<"HARD V MEDIUM\n";
		}
		else if(input=="MEDIUM"){
			AIlevelP1="HARD";
			AIlevelP2="MEDIUM";
			std::cout<<"HARD V MEDIUM\n";
		}
		else if(input=="TEST_EASY"){
			AIlevelP1="HARD";
			AIlevelP2="EASY";
			std::cout<<"HARD V EASY\n";
		}
		else if(input=="BLACK"){
			AIlevelP2=AIlevelP1;
		    AIlevelP1="OFF";
			std::cout<<"BLACK\n";
		}
		else if(input=="WHITE"){
		    AIlevelP1=AIlevelP2;
		    AIlevelP2="OFF";
			std::cout<<"WHITE\n";
		}
		else if(input=="NO_AI"){
		    AIlevelP1="OFF";
		    AIlevelP2="OFF";
		    std::cout<<"OK\n";
		}
		else if(input=="EASY"){
		    Square aiPlayer = ((PlayerIsAI(player2))?(player2):(player1));
		    AIlevel(aiPlayer) ="EASY";
			std::cout<<"OK\n";
		}
		else if(input=="MEDIUM"){
		    Square aiPlayer = ((PlayerIsAI(player2))?(player2):(player1));
		    AIlevel(aiPlayer) ="MEDIUM";
			std::cout<<"OK\n";
		}
		else if(input=="HARD"){
		    Square aiPlayer = ((PlayerIsAI(player2))?(player2):(player1));
		    AIlevel(aiPlayer) ="HARD";
			std::cout<<"OK\n";
		}
		else if(input[0]=='P'){
		    if(input[1]=='1'){
		        AIlevelP1=input.substr(2);
                std::cout<<"OK\n";
		    }
		    else if(input[1]=='2'){
		        AIlevelP2=input.substr(2);
                std::cout<<"OK\n";
		    }
		    else{std::cout<<"ILLEGAL\n";}
		}
		else if(input=="4X4"){
			std::cout<<"OK\n";
			boardSize=4;
		}

		else if(input=="8X8"){
			std::cout<<"OK\n";
			boardSize=8;
		}
		else if(input=="6"){
		    AIlevelP1="MEDIUM";
		    AIlevelP2="OFF";
		    std::cout<<"OK\n";
			std::cout<<"P1:HARD-AI, P2:Human\n";
		    displayOn=true;
			break;
		}
		else if(input=="7"){
		    AIlevelP2="MEDIUM";
		    AIlevelP1="OFF";
		    std::cout<<"OK\n";
			std::cout<<"P1:Human, P2:HARD-AI\n";
		    displayOn=true;
			break;
		}
		else if(input=="8"){
		    AIlevelP1="MEDIUM";
		    AIlevelP2="HARD";
		    std::cout<<"OK\n";
			std::cout<<"P1:"<<AIlevelP1<<"-AI, P2:"<<AIlevelP2<<"-AI\n";
		    displayOn=true;
			break;
		}
		else if(input=="88"){
		    AIlevelP1="MEDIUM_DEBG";
		    AIlevelP2="HARD";
		    std::cout<<"OK\n";
			std::cout<<"P1:"<<AIlevelP1<<"-AI, P2:"<<AIlevelP2<<"-AI\n";
		    displayOn=true;
			break;
		}
		else{
		    if(input!="?"){std::cout<<"ILLEGAL\n";}
			std::cout<<"WHITE, BLACK, EASY, MEDIUM, HARD, DISPLAY_ON, EXIT\n"; //TODO: Add more..
		}
	}
	return 1;
}

//forward decleration
std::pair<int,int> findBestMove(Square forecastPlayer,int depth);

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

//receives all input during the execution of the game
int handleGameInput(int client){
    while(1){
        if(displayOn){
            std::cout<<"\n-------------------\n";
            std::cout<<"Current Player:"<<CurrentPlayer<<"\n";
            std::cout<<((PlayerIsAI(CurrentPlayer))?("Waiting on AI"):("Human's Move"))<<"\n";
            std::cout<< game;
        }

        //Check if the game has ended 
        if (game.Count(empty) == 0){
            //No empty squares 
            const int n1 = game.Count(player1); 
            const int n2 = game.Count(player2); //TODO: ADD IN TIE CASE
            std::string p1Name = (PlayerIsAI(player1))?(AIlevelP1+"-AI"):("Human");
            std::string p2Name = (PlayerIsAI(player2))?(AIlevelP2+"-AI"):("Human");
            std::cout<< "The game has ended\n"
                    <<"Player1 ("<<p1Name<<")["<<player1<<"] conquered "<<n1<<" squares.\n"
                    <<"Player2 ("<<p2Name<<")["<<player2<<"] conquered "<<n2<<" squares.\n"
                    <<"The winner is Player"<<((n1>n2)?("1("+p1Name+")"):("2("+p2Name+")"))
                    <<"\nCongratulations!\n\n";
             return 1;
        }
		if ((game.GetValidMoves(player1).empty()==true)&&
			(game.GetValidMoves(player2).empty()==true)){
			//No available moves for either player 
            const int n1 = game.Count(player1); 
            const int n2 = game.Count(player2); 
            std::string p1Name = (PlayerIsAI(player1))?(AIlevelP1+"-AI"):("Human");
            std::string p2Name = (PlayerIsAI(player2))?(AIlevelP2+"-AI"):("Human");
            std::cout<< "The game has ended\n"
                    <<"Player1 ("<<p1Name<<")["<<player1<<"] conquered "<<n1<<" squares.\n"
                    <<"Player2 ("<<p2Name<<")["<<player2<<"] conquered "<<n2<<" squares.\n"
                    <<"It was a draw!\n";
             return 1;
		}



        //Check if Current Player can actually make a move 
        if (game.GetValidMoves(CurrentPlayer).empty()==true){
            //If Current Player cannot move,
            std::cout<<"Too bad! Player"<<int(CurrentPlayer)<<"("<<CurrentPlayer<<") is unabled to do a valid move!\n"; 
            CurrentPlayer=GetOtherPlayer(CurrentPlayer);
            std::cout<<"The next turn again goes to Player"<<int(CurrentPlayer)<<"("<<CurrentPlayer<<") !\n";
            continue;
        }
        
        {//Input-Gathering Block
            if(PlayerIsAI(CurrentPlayer)){
                //CurrentPlayer is an AI
                if(AIlevel(CurrentPlayer)=="EASY"){
                    moveRandomly();
                }
                else if(AIlevel(CurrentPlayer)=="MEDIUM"){
                    HardAI hAI(game, CurrentPlayer,1,AIlevel(CurrentPlayer).substr(AIlevel(CurrentPlayer).size()-4)=="DEBG");
                    coordinate = hAI.findMax();
                }
                else if(AIlevel(CurrentPlayer).substr(0,4)=="HARD"){
					maxDepth=6;
					std::pair<int,int> bestMove = findBestMove(CurrentPlayer,0);
					coordinate.first=bestMove.first;coordinate.second=bestMove.second;

                }
                std::cout<<AIlevel(CurrentPlayer)<<"-AI Plays @:"<<char('A'+coordinate.first)<<coordinate.second+1<<"\n";
            }
            else if( !(PlayerIsAI(CurrentPlayer)) ) {
                //CurrentPlayer is a Human
                std::cout<<"::";
                const std::string input = GetInput(client);
                const bool isValidCoordinate = IsCoordinate(input, coordinate); 
                if (isValidCoordinate == false){
                    //Input was not a Coordinate
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
                    }
                }
                if(isValidCoordinate){
                   //Valid coordinate was entered
                    if(displayOn){
                        std::cout<<"Human Plays @:"<<char('A'+coordinate.first)<<coordinate.second+1<<"\n";
                    }
                }
            }//End Human Input
        }//End of Input-Gathering Block
        
        //Check that move is valid for Current Player.
        if (game.IsValidMove(coordinate.first, coordinate.second, CurrentPlayer) == false){
            std::cout << "ILLEGAL\n";
            continue; 
        }

        //Actually do the move 
        game.DoMove(coordinate.first, coordinate.second, CurrentPlayer);

        //Switch Player's turns
        CurrentPlayer=GetOtherPlayer(CurrentPlayer); 
        if(displayOn){std::cout<<"-------------------\n\n";}
    }
    return 1;
}

//forward decleration
std::pair<int,int> findBestMove(Square forecastPlayer,int depth);

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

//uses a health-number for the different parts of the board to determine a moves strength
int heuristicWeight(Reversi childBoard, int x, int y, Square moveOwner, int depth){
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
				return 10000;
			else
				return -10000;
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
	else
		if(CurrentPlayer==forecastPlayer)
			return (-10)*childBoard.GetValidMoves(nextPlayer).size();
		else
			return (10)*childBoard.GetValidMoves(nextPlayer).size();
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
		forecastedMoveWeight+=heuristicWeight(	childBoard,
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
			heuristicWeight(childBoard,//here we say that we don't want to bother
							vals[possibleMove].first,//checking further down this move's
							vals[possibleMove].second,//lineage if it involves the
							forecastPlayer, depth)!=-10000){//opponent taking a corner 
				nextPlayer = (forecastPlayer == player1 ? player2 : player1);
				forecastedMoveWeight+=checkForWeightZ(childBoard,nextPlayer,depth+1);
		}
		else{
			return childBoard.Count(CurrentPlayer)*100+endGameEvaluator(childBoard);
		}
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
	return MaxMoveWeight;
}

//takes the game board and creates a tree of moves, passing a game board with
//each immediately available move, to checkForWeightz
std::pair<int,int> findBestMove(Square forecastPlayer,int depth){
	int maxMoveWeight=-99999,immediatePlusForecastWeights=-99999,maxPossibleMove=0;
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
		immediateMoveWeights+=heuristicWeight(	boardForAPrimaryMove,
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
