#include <iostream>
#include "UnitReversi.h"
#include <vector>

using namespace std;


#define UtilityValue int
#define Action pair<int,int>
#define State Reversi*


bool TerminalTest(State S, Square player){
	//return S->GetValidMoves(player)==0;
	//need some condsideration for: if no valid moves for player, player loses turn
}

State Result(State S, Action A){
	//return the state after applying Action A to State S
}

UtilityValue Utility(State S, Square player){
	//consider S->Count(player) - S->Count(GetOtherPlayer(player));
	//consider # of edge and corner pieces
	//consider GetValidMoves(GetOtherPlayer(player)).size(); //i.e. number of moves other play can make
	//consider number of GetOtherPlayer(player) pieces that player can flip 
}

vector<Action> Actions(State S, Square player){
	return S->GetValidMoves(player);
}


Action MinmaxDecision(State S, Square player, int MaxDepth){
	vector<Action> possibleMoves=Actions(S,player);
	pair<Action,UtilityValue> MaxPlay = pair<Action,UtilityValue>(pair<int,int>(-1,-1),numeric_limits<UtilityValue>.min()); // set initial MaxPlay UtilityValue to 'negative infinity'
	for(int i=0; i<possibleMoves.size(); i++){
		State s0 = Result(S,possibleMoves[i]);
		UtilityValue s0Util = MinValue(s0,player,1,MaxDepth);
		if(s0Util>MaxPlay.second){ MaxPlay=pair<Action,UtilityValue>(possibleMoves[i],s0Util);}
	}
	return MaxPlay.first;
}


UtilityValue MaxValue(State S, Square player, int CurrDepth, int MaxDepth){
	if(TerminalTest(S,player){
		return Utility(S,player));
	}
	//pair<Action,UtilityValue> MaxPlay = pair<Action,UtilityValue>(pair<int,int>(-1,-1),numeric_limits<UtilityValue>.min()); // set initial MaxPlay UtilityValue to 'negative infinity'
	UtilityValue MaxUtility = numeric_limits<UtilityValue>.min();//// set initial MaxUtility to 'negative infinity'
	vector<Action> possibleMoves = Actions(player);
	for(int i=0;i<possibleMoves.size();i++){
		State s0 = Result(S,possibleMoves[i]);
		UtilityValue s0Util = (currDepth==MaxDepth)?(Utility(s0,player)):(MinValue(s0,player,CurrDepth+1,MaxDepth));
		//if(s0Util>MaxPlay.second){ MaxPlay=pair<Action,UtilityValue>(possibleMoves[i],s0Util);}
		if(s0Util>MaxUtility){MaxUtility=s0Util;}
	}
	//return MinPlay.second;
	return MaxUtility;
}

UtilityValue MinValue(State S, Square player, int CurrDepth, int MaxDepth){
	if(TerminalTest(S) || currDepth){
		return Utility(S,player);
	}
	//pair<Action,UtilityValue> MinPlay = pair<Action,UtilityValue>(pair<int,int>(-1,-1),numeric_limits<UtilityValue>.max()); // set initial MinPlay UtilityValue to 'infinity'
	UtilityValue MinUtility = numeric_limits<UtilityValue>.max();//// set initial MinUtility to 'positive infinity'
	vector<Action> possibleMoves = Actions(player);
	for(int i=0;i<possibleMoves.size();i++){
		State s0 = Result(S,possibleMoves[i]);
		UtilityValue s0Util = (CurrDepth==MaxDepth)?(Utility(s0,player)):(MaxValue(s0,player,CurrDepth+1,MaxDepth));
		//if(s0Util<MinPlay.second){ MinPlay=pair<Action,UtilityValue>(possibleMoves[i],s0Util);}
		if(s0Util<MinUtility){MinUtility=s0Util;}
	}
	//return MinPlay.second;
	return MinUtility;
}


class HardAI{
	Reversi stateP;
	Square player; 
	
	HardAI(Reversi State, Square Player){
		player = Player
		stateP = State;
	}
	
	Action findMax(){
	
	
	}

};


int main(){
	cout<<"Hello World!\n";
	
	
	return 0;
}

