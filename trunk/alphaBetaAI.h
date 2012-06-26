#include <iostream>
#include "UnitReversi.h"
#include <vector>
#include <limits>
#include <cstdlib>

using namespace std;


#define UtilityValue int
#define Action pair<int,int>
#define State Reversi
#define Play pair<Action,UtilityValue> 

int bSize=8;
Square MaxPlayer;
int MaxDepth;

//uses a health-number for the different parts of the board to determine a moves strength
int heuristicWeightZ(Reversi childBoard, Action newMove, Square moveOwner, int depth){
	int x= newMove.first;
	int y= newMove.second;
	int cummulative=0;
	//got a corner
	if(	(x==0&&y==0)||//top left
		(x==bSize-1&&y==bSize-1)||//bottom right
		(x==0&&y==bSize-1)||//bottom left
		(x==bSize-1&&y==0)){ //top right
			//return 5000;
		if(moveOwner==MaxPlayer)
			return  5000;//player wants this bad
		else
			return -5000;//player definately doesn't want oppoent to have
			
		
	}
	//edge next to empty corner
	if( (((x==0&&y==1)||(x==1&&y==0))&&childBoard.GetSquare(0,0)==empty)||
		(((x==0&&y==6)||(x==1&&y==7))&&childBoard.GetSquare(0,7)==empty)||
		(((x==6&&y==0)||(x==7&&y==1))&&childBoard.GetSquare(7,0)==empty)||
		(((x==6&&y==7)||(7==1&&y==6))&&childBoard.GetSquare(7,7)==empty)){
			//return -1500;
			if(moveOwner==MaxPlayer)
				return -1500;
			else
				return 1500;
	}
	//got an edge
	if( x==0||//left edge
		x==bSize-1||//right edge
		y==0||//top edge
		y==bSize-1){//bottom edge
		//	return 1500;
		if(moveOwner==MaxPlayer)
			return 1500;
		else
			return -1500;
	}
	//avoid these
	if( x==1||x==bSize-2){
		//cummulative+= -1000;//in this case player doesn't want this move
		
		if(moveOwner==MaxPlayer)
			cummulative+= -1000;//in this case player doesn't want this move
		else
			cummulative+= 1000;//player would like his opponent to make this move
		
	}
	if( y==1||y==bSize-2){
		//cummulative+= -1000;//in this case player doesn't want this move
		
		if(moveOwner==MaxPlayer)
			cummulative+= -1000;//in this case player doesn't want this move
		else
			cummulative+= 1000;//player would like his opponent to make this move
		
	}
	return cummulative;
}

//experimental version
int heuristicWeightY(Reversi childBoard, Action newMove, Square moveOwner, int depth){
	int x= newMove.first;
	int y= newMove.second;
	int cummulative=0;
	depth = MaxDepth - depth;
	//got a corner
	if(	(x==0&&y==0)||//top left
		(x==bSize-1&&y==bSize-1)||//bottom right
		(x==0&&y==bSize-1)||//bottom left
		(x==bSize-1&&y==0)){ //top right
		if(moveOwner==MaxPlayer)
			return  10000/(depth+1);//45,19@5000&10k&11k&15k
		else
			return -10000/(depth+1);//45,19@-5000&-10k&-11k&-15k
	}
	//edge next to empty corner
	if( (((x==0&&y==1)||(x==1&&y==0))&&childBoard.GetSquare(0,0)==empty)||
		(((x==0&&y==6)||(x==1&&y==7))&&childBoard.GetSquare(0,7)==empty)||
		(((x==6&&y==0)||(x==7&&y==1))&&childBoard.GetSquare(7,0)==empty)||
		(((x==6&&y==7)||(7==1&&y==6))&&childBoard.GetSquare(7,7)==empty)){
			if(moveOwner==MaxPlayer)
				return -5000/(depth+1);//36,28@-3000/33,31@5000&6000/39,25@-7000
			else
				return 5000/(depth+1);//36,28@3000/33,31@5000&6000/39,25@7000
	}
	//safe edge next to corner
	if( (((x==0&&y==1)||(x==1&&y==0))&&childBoard.GetSquare(0,0)==moveOwner)||
		(((x==0&&y==6)||(x==1&&y==7))&&childBoard.GetSquare(0,7)==moveOwner)||
		(((x==6&&y==0)||(x==7&&y==1))&&childBoard.GetSquare(7,0)==moveOwner)||
		(((x==6&&y==7)||(x==7&&y==6))&&childBoard.GetSquare(7,7)==moveOwner)){
			if(moveOwner==MaxPlayer)
				return 5000/(depth+1);//36,28@-3000/33,31@5000&6000/39,25@-7000
			else
				return -5000/(depth+1);//36,28@3000/33,31@5000&6000/39,25@7000
	}
	//sweet-sixteen corner
	if( ((x==2&&y==2)&&childBoard.GetSquare(0,0)==empty)||
		((x==2&&y==5)&&childBoard.GetSquare(0,7)==empty)||
		((x==5&&y==2)&&childBoard.GetSquare(7,0)==empty)||
		((x==5&&y==5)&&childBoard.GetSquare(7,7)==empty) ){
			if(moveOwner==MaxPlayer)
				return 2500/(depth+1);
			else
				return -2500/(depth+1);
	}
	//deadman's corner
	if( ((x==1&&y==1)&&childBoard.GetSquare(0,0)==empty)||
		((x==1&&y==6)&&childBoard.GetSquare(0,7)==empty)||
		((x==6&&y==1)&&childBoard.GetSquare(7,0)==empty)||
		((x==6&&y==6)&&childBoard.GetSquare(7,7)==empty) ){
			if(moveOwner==MaxPlayer)
				return -5000/(depth+1);
			else
				return 5000/(depth+1);
	}
	//got an edge, blocking a straightaway
	if( (x==0&&(y<bSize-3&&y>2))&&
		childBoard.GetSquare(x+1,y)==GetOtherPlayer(moveOwner))
		if(moveOwner==MaxPlayer)
			cummulative+= 1500/(depth+1);
		else
			cummulative+= -1500/(depth+1);
	if( (x==bSize-1&&(y<bSize-3&&y>2))&&
		childBoard.GetSquare(x-1,y)==GetOtherPlayer(moveOwner))
		if(moveOwner==MaxPlayer)
			cummulative+= 1500/(depth+1);
		else
			cummulative+= -1500/(depth+1);
	if( (y==0&&(x<bSize-3&&x>2))&&
		childBoard.GetSquare(x,y+1)==GetOtherPlayer(moveOwner))
		if(moveOwner==MaxPlayer)
			cummulative+= 1500/(depth+1);
		else
			cummulative+= -1500/(depth+1);
	if( (y==bSize-1&&(x<bSize-3&&x>2))&&
		childBoard.GetSquare(x,y-1)==GetOtherPlayer(moveOwner))
		if(moveOwner==MaxPlayer)
			cummulative+= 1500/(depth+1);
		else
			cummulative+= -1500/(depth+1);
	if( x==0||//left edge
		x==bSize-1||//right edge
		y==0||//top edge
		y==bSize-1){//bottom edge
		if(moveOwner==MaxPlayer)
			cummulative+= 750/(depth+1);
		else
			cummulative-= 750/(depth+1);
	}
	//avoid these
	if( x==1||x==bSize-2){
		if(moveOwner==MaxPlayer)
			cummulative-= 1500/(depth+1);//in this case player doesn't want this move
		else
			cummulative+= 1500/(depth+1);//player would like his opponent to make this move
	}
	if( y==1||y==bSize-2){
		if(moveOwner==MaxPlayer)
			cummulative-= 1500/(depth+1);//in this case player doesn't want this move
		else
			cummulative+= 1500/(depth+1);//player would like his opponent to make this move
	}

	return cummulative;
}


bool TerminalTest(State S, Square player){
    return S.GetValidMoves(player).size()==0;
}

State Result(const State S, Action A, Square player){
    //Return the state produced by applying Action A to State S
    State ret = S;
    ret.DoMove(A.first,A.second,player);
    return ret;
}

UtilityValue Utility(const State S, Action newMove, Square player, int Depth){
	UtilityValue v = heuristicWeightZ(S, newMove, player, Depth);
	//cout<<"Returning Utility of: "<<v<<" for move:"<<newMove.first<<","<<newMove.second<<"\n";
	return v;
}

vector<Action> Actions(const State S, Square player){
    return S.GetValidMoves(player);
}

Play AlphaBeta(const State S, Square player, int CurrDepth, int alpha, int beta, Action newMove){
    if(CurrDepth==0){
        return Play(newMove, Utility(S,newMove,player,CurrDepth));
    }
    if(TerminalTest(S,player)){
        return Play(newMove, Utility(S,newMove,player,CurrDepth));
    }
    if(player==MaxPlayer){
        Play maxPlay = Play(Action(-2,-2),alpha);
        //Get all possibleMoves that Player can make
        vector<Action> possibleMoves = Actions(S,player);
        //cout<<"Enter MAX D:"<<CurrDepth<<" A:"<<alpha<<" B:"<<beta<<" PosMovs:"<<possibleMoves.size()<<"\n";
		for(int i=0; i<possibleMoves.size(); i++){
            Action possibleMove=possibleMoves[i];
            State s0 = Result(S,possibleMove,player); //Get State after applying possibleMove by Player
            //cout<<"Move["<<i<<"]:"<<possibleMove.first<<","<<possibleMove.second<<"\n";
			Play play0 = AlphaBeta(s0, GetOtherPlayer(player), CurrDepth-1, maxPlay.second, beta, possibleMove);
            if(play0.second>maxPlay.second){
                maxPlay=play0;
				maxPlay.first = possibleMove;
            }
            if(beta<=maxPlay.second){
                //beta cut-off
				//cout<<"Beta Cutoff \n";
                break;
            }
        }
		//if(maxPlay.first.first==-1){ cerr<<"!!unexpected -1-1 action. MaxPlayer, Depth:"<<CurrDepth<<" \n"; }
		//if(maxPlay.first.first==-2){ cerr<<"!!unexpected -2-2 action. MaxPlayer, Depth:"<<CurrDepth<<" \n"; }
        return maxPlay;
    }
    else{ //player==MinPlayer
        Play minPlay = Play(Action(-2,-2),beta);
        //Get all possibleMoves that Player can make
        vector<Action> possibleMoves = Actions(S,player);
		//cout<<"Enter MIN D:"<<CurrDepth<<" A:"<<alpha<<" B:"<<beta<<" PosMovs:"<<possibleMoves.size()<<"\n";
		for(int i=0; i<possibleMoves.size(); i++){
            Action possibleMove=possibleMoves[i];
            State s0 = Result(S,possibleMove,player); //Get State after applying possibleMove by Player
			//cout<<"Move["<<i<<"]:"<<possibleMove.first<<","<<possibleMove.second<<"\n";
            Play play0 = AlphaBeta(s0,GetOtherPlayer(player),CurrDepth-1,alpha,minPlay.second, possibleMove);
            if(play0.second<minPlay.second){
                minPlay=play0;
				minPlay.first=possibleMove;
            }
            if(minPlay.second<=alpha){
                //alpha cut-off
				//cout<<"Alpha Cutoff \n";
                break;
            }
        }
        //if(minPlay.first.first==-1){ cerr<<"!!unexpected -1-1 action. MinPlayer, Depth:"<<CurrDepth<<" \n"; }
		//if(minPlay.first.first==-2){ cerr<<"!!unexpected -2-2 action. MinPlayer, Depth:"<<CurrDepth<<" \n"; }
        return minPlay;
    }
}

struct AlphaBetaAI{
    Reversi gState;
    Square player;
    int depth;
	bool debug;

    AlphaBetaAI(const Reversi& CurrState, Square Player, int Depth=2, bool Debug=false){
        player = Player;
        gState = CurrState;
        depth = Depth;
		MaxDepth = Depth;
        debug=Debug;
        MaxPlayer=Player;
    }

    Action findMax(){
		int negInf = numeric_limits<UtilityValue>::min();
		int posInf = numeric_limits<UtilityValue>::max();
        Play maxPlay = AlphaBeta(gState, player, depth, negInf, posInf, Action(-1,-1));
        //if((maxPlay.first.first==-1 || maxPlay.first.second==-1)){ cerr<<"AlphaBetaAI.findMax() returned -1,-1\n";}
        //cout<<"Value of this move is:"<<maxPlay.second<<"\n";
		return maxPlay.first;
    }
};

