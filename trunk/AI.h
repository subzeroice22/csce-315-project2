#include <iostream>
#include "UnitReversi.h"
#include <vector>

using namespace std;


#define UtilityValue int
#define Action pair<int,int>
#define State Reversi


bool TerminalTest(State S, Square player){
	//return S->GetValidMoves(player)==0;
	//need some condsideration for: if no valid moves for player, player loses turn
	return false;
}

State Result(const State S, Action A, Square player){
	State ret = S;
	ret.DoMove(A.first,A.second,player);
	//return the state after applying Action A to State S
}

UtilityValue Utility(const State S, Square player){
	return S.Count(player);
	//consider S->Count(player) - S->Count(GetOtherPlayer(player));
	//consider # of edge and corner pieces
	//consider GetValidMoves(GetOtherPlayer(player)).size(); //i.e. number of moves other play can make
	//consider number of GetOtherPlayer(player) pieces that player can flip 
}

vector<Action> Actions(const State S, Square player){
	return S.GetValidMoves(player);
}




UtilityValue MinValue(const State S, Square player, int CurrDepth, int MaxDepth);

UtilityValue MaxValue(const State S, Square player, int CurrDepth, int MaxDepth){
	cout<<"~~~MaxValue() Called\n";
	if(TerminalTest(S,player)){
		return Utility(S,player);
	}
	//pair<Action,UtilityValue> MaxPlay = pair<Action,UtilityValue>(pair<int,int>(-1,-1),numeric_limits<UtilityValue>.min()); // set initial MaxPlay UtilityValue to 'negative infinity'
	UtilityValue MaxUtility = numeric_limits<UtilityValue>::min();//// set initial MaxUtility to 'negative infinity'
	vector<Action> possibleMoves = Actions(S,player);
	for(int i=0;i<possibleMoves.size();i++){
		State s0 = Result(S,possibleMoves[i],player);
		UtilityValue s0Util = (CurrDepth==MaxDepth)?(Utility(s0,player)):(MinValue(s0,player,CurrDepth+1,MaxDepth));
		//if(s0Util>MaxPlay.second){ MaxPlay=pair<Action,UtilityValue>(possibleMoves[i],s0Util);}
		if(s0Util>MaxUtility){MaxUtility=s0Util;}
	}
	//return MinPlay.second;
	return MaxUtility;
}

UtilityValue MinValue(const State S, Square player, int CurrDepth, int MaxDepth){
	cout<<"~~~MinValue() Called\n";
	if(TerminalTest(S,player)){
		return Utility(S,player);
	}
	//pair<Action,UtilityValue> MinPlay = pair<Action,UtilityValue>(pair<int,int>(-1,-1),numeric_limits<UtilityValue>.max()); // set initial MinPlay UtilityValue to 'infinity'
	UtilityValue MinUtility = numeric_limits<UtilityValue>::max();//// set initial MinUtility to 'positive infinity'
	vector<Action> possibleMoves = Actions(S,player);
	for(int i=0;i<possibleMoves.size();i++){
		State s0 = Result(S,possibleMoves[i],player);
		UtilityValue s0Util = (CurrDepth==MaxDepth)?(Utility(s0,player)):(MaxValue(s0,player,CurrDepth+1,MaxDepth));
		//if(s0Util<MinPlay.second){ MinPlay=pair<Action,UtilityValue>(possibleMoves[i],s0Util);}
		if(s0Util<MinUtility){MinUtility=s0Util;}
	}
	//return MinPlay.second;
	return MinUtility;
}


Action MinmaxDecision(const State S, Square player, int MaxDepth){
	cout<<"~MinmaxDecision() Called\n";
	vector<Action> possibleMoves=Actions(S,player);
	pair<Action,UtilityValue> MaxPlay;
	MaxPlay = pair<Action,UtilityValue>(pair<int,int>(-1,-1),numeric_limits<UtilityValue>::min()); // set initial MaxPlay UtilityValue to 'negative infinity'
	cout<<"~~1\n";
	for(int i=0; i<possibleMoves.size(); i++){
		cout<<"~~2\n";
		State s0 = Result(S,possibleMoves[i],player);
		cout<<"s0.Count(player)="<<s0.Count(player)<<endl;
		cout<<"~~3\n";
		UtilityValue s0Util = MinValue(s0,player,1,MaxDepth);
		cout<<"~~4\n";
		if(s0Util>MaxPlay.second){cout<<"~~5\n"; MaxPlay=pair<Action,UtilityValue>(possibleMoves[i],s0Util);}
		cout<<"~~6\n";
	}
	return MaxPlay.first;
}

struct HardAI{
	Reversi gState;
	Square player; 
	
	HardAI(const Reversi& ss, Square Player){
		player = Player;
		gState = ss;
		cout<<"!1\n";
		cout<<"gState.Count(player)="<<gState.Count(player)<<endl;
	}
	
	Action findMax(){
		//cout<<"~Calling findMax()\n";
		return MinmaxDecision(gState,player,3);
	
	}

};



