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
//Ply->Play


                                        /*   debug.h   */
                                        bool debug=false;
                                        int debugLevel=0;
                                        void printLv(string LevelText="", bool NewLn=false){
                                            for(int i=0; i<debugLevel; i++){cout<<"~";}
                                            cout<<" | "<<LevelText;
                                            if(NewLn){cout<<"\n";}
                                            //else{cout<<". ";}
                                        }
                                        void enterLv(string LvName="", bool NewLn=true){
                                            for(int i=0; i<debugLevel; i++){cout<<"~";}
                                            cout<<" \\Entering: "<<LvName;
                                            if(NewLn){cout<<"\n";}
                                            else{cout<<". ";}
                                            debugLevel++;
                                        }
                                        void leaveLv(string LvName="", bool NewLn=true){
                                            debugLevel--;
                                            for(int i=0; i<debugLevel; i++){cout<<"~";}
                                            cout<<" /Leaving "<<LvName;
                                            if(NewLn){cout<<"\n";}
                                            else{cout<<". ";}
                                        }
                                        void printMoves(vector<Action> possibleMoves, bool NewLn=false){
                                            for(int i=0; i<possibleMoves.size(); i++){
                                                cout<<possibleMoves[i].first<<","<<possibleMoves[i].second<<" : "; 
                                            }
                                            if(NewLn){cout<<"\n";}
                                            else{cout<<". ";}
                                        }
                                        /*   debug.h   */


bool TerminalTest(State S, Square player){
    //TODO: IMPLEMENT!
    return S.GetValidMoves(player).size()==0;
    //need some condsideration for: if no valid moves for player, player loses turn
    //return false;
}

State Result(const State S, Action A, Square player){
    //Return the state produced by applying Action A to State S
    State ret = S;
    ret.DoMove(A.first,A.second,player);
    return ret;
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

pair<Action,UtilityValue> FindMin(const State S, Square player, int CurrDepth, int MaxDepth);

pair<Action,UtilityValue> FindMax(const State S, Square player, int CurrDepth, int MaxDepth){
                                                                                        if(debug){ enterLv("FindMax",false); cout<<"D:"<<CurrDepth<<"\n"; }
    if(CurrDepth==MaxDepth){                                                            if(debug){printLv(); cout<<"-->| MaxDepth. Return UtilVal:"<<(Utility(S,player))<<"\n"; leaveLv("FindMax",false); cout<<"D:"<<CurrDepth<<"\n";}
        return pair<Action,UtilityValue>(Action(-1,-1), Utility(S,player));
    }
    if(TerminalTest(S,player)){                                                         if(debug){printLv(); cout<<"-->| Terminal State. Return UtilVal:"<<Utility(S,player)<<"\n"; leaveLv("FindMax",false); cout<<"D:"<<CurrDepth<<"\n";}
        return pair<Action,UtilityValue>(Action(-1,-1), Utility(S,player));
    }
    //Get all possibleMoves that Player can make
    vector<Action> possibleMoves = Actions(S,player);
                                                                                        if(debug){ printLv(); cout<<possibleMoves.size()<<" possibleMoves:"; printMoves(possibleMoves); cout<<"\n"; }
    //Initialize MaxPlay's UtilityValue to 'negative infinity'
    pair<Action,UtilityValue> MaxPlay = pair<Action,UtilityValue>(pair<int,int>(-1,-1),numeric_limits<UtilityValue>::min()); 

    //Evaluate each possibleMove, Maximize the Minimized UtilityValues
    for(int i=0;i<possibleMoves.size();i++) {                                           if(debug){printLv(); cout<<"D:"<<CurrDepth<<", PossibleMove:"<<i+1<<"/"<<possibleMoves.size()<<":"<<possibleMoves[i].first<<","<<possibleMoves[i].second<<"\n";} 
        Action possibleMove=possibleMoves[i];
        State s0 = Result(S,possibleMove,player); //Get State after applying possibleMove by Player
        Play s0Util = FindMin(s0,player,CurrDepth+1,MaxDepth); //Get Minimized Play (Action,UtilityValue) from said state
        if(s0Util.second>MaxPlay.second){ //Maximize the Minimized UtilityValue
            MaxPlay.first=possibleMove;
            MaxPlay.second=s0Util.second;
        }
    }
                                                                                        if(debug){printLv();cout<<"Found Max! Move:"<<MaxPlay.first.first<<","<<MaxPlay.first.second<<"| UtilVal:"<<MaxPlay.second<<"\n"; leaveLv("FindMax",false); cout<<"D:"<<CurrDepth<<"\n";}
    if(MaxPlay.first.first==-1 || MaxPlay.first.second==-1){ cerr<<"Should not get here. Ref:1233 FindMax() File:AI.h\n"; exit(-1);}
    return MaxPlay;
}

pair<Action,UtilityValue> FindMin(const State S, Square player, int CurrDepth, int MaxDepth){
                                                                                        if(debug){ enterLv("FindMin",false); cout<<"D:"<<CurrDepth<<"\n"; }
    if(CurrDepth==MaxDepth){                                                            if(debug){printLv(); cout<<"-->| MaxDepth. Return UtilVal(:"<<(Utility(S,player))<<"\n"; leaveLv("FindMin",false); cout<<"D:"<<CurrDepth<<"\n";}
        return pair<Action,UtilityValue>(Action(-1,-1), Utility(S,player));
    }
    if(TerminalTest(S,GetOtherPlayer(player))){                                                         if(debug){printLv(); cout<<"--> Terminal State. Return UtilVal:"<<Utility(S,player)<<"\n"; leaveLv("FindMin",false); cout<<"D:"<<CurrDepth<<"\n";}
        return pair<Action,UtilityValue>(Action(-1,-1), Utility(S,player));
    }
    //Get all possibleMoves that OtherPlayer can make
    vector<Action> possibleMoves = Actions(S,GetOtherPlayer(player));
                                                                                        if(debug){ printLv(); cout<<possibleMoves.size()<<" possibleMoves:"; printMoves(possibleMoves); cout<<"\n"; }
    //Initialize MinPlay's UtilityValue to 'positive infinity'
    pair<Action,UtilityValue> MinPlay = pair<Action,UtilityValue>(pair<int,int>(-1,-1),numeric_limits<UtilityValue>::max()); 

    //Evaluate each possibleMove, Minimize the Maximized UtilityValues
    for(int i=0;i<possibleMoves.size();i++){                                            if(debug){printLv(); cout<<"D:"<<CurrDepth<<", PossibleMove:"<<i+1<<"/"<<possibleMoves.size()<<":"<<possibleMoves[i].first<<","<<possibleMoves[i].second<<"\n";} 
        Action possibleMove=possibleMoves[i];
        State s0 = Result(S,possibleMove,GetOtherPlayer(player)); //Get State after applying possibleMove by OtherPlayer
        Play s0Util = FindMax(s0,player,CurrDepth+1,MaxDepth); //Get Maximized Play (Action/UtilityValue)
        if(s0Util.second<MinPlay.second){ //Minimize the Maximized Play's UtilityValue
            MinPlay.first=possibleMove;
            MinPlay.second=s0Util.second;
        }
    }
                                                                                        if(debug){printLv();cout<<"Found Min! Move:"<<MinPlay.first.first<<","<<MinPlay.first.second<<"| UtilVal:"<<MinPlay.second<<"\n"; leaveLv("FindMin",false); cout<<"D:"<<CurrDepth<<"\n";}
    if(MinPlay.first.first==-1 || MinPlay.first.second==-1){ cerr<<"Should not get here. Ref:1234 FindMin() File:AI.h\n"; exit(-1);}
    return MinPlay;
}


Action MinmaxDecision(const State S, Square player, int PlyDepth){ //NOTE: PlyDepth denotes the number of Plys to examine. 
                                                                                        if(debug){ enterLv("MinMaxDecision",false); cout<<"D:0\n"; }
    if(TerminalTest(S,player)){                                                         if(debug){printLv(); cout<<"--> Terminal State. Return UtilVal:"<<Utility(S,player)<<"\n"; leaveLv("MinMaxDecision",false); cout<<"D:0\n";}
        return Action(-1,-1);
    }
    //Get all possibleMoves that Player can make
    vector<Action> possibleMoves = Actions(S,player);
                                                                                        if(debug){ printLv(); cout<<possibleMoves.size()<<" possibleMoves:"; printMoves(possibleMoves); cout<<"\n"; }
    //Initialize MaxPlay's UtilityValue to 'negative infinity'
    pair<Action,UtilityValue> MaxPlay = pair<Action,UtilityValue>(pair<int,int>(-1,-1),numeric_limits<UtilityValue>::min()); 

    //Evaluate each possibleMove, Maximize the Minimized UtilityValues
    for(int i=0;i<possibleMoves.size();i++) {                                           if(debug){printLv(); cout<<"D:0, PossibleMove:"<<i+1<<"/"<<possibleMoves.size()<<":"<<possibleMoves[i].first<<","<<possibleMoves[i].second<<"\n";} 
        Action possibleMove=possibleMoves[i];
        State s0 = Result(S,possibleMove,player); //Get State after applying possibleMove by Player
        Play s0Util = FindMin(s0,player,1,PlyDepth*2); //Get Minimized Play (Action,UtilityValue) from said state //NOTE: PlyDepth*2 is the MaxDepth for FindMin and FindMax
        if(s0Util.second>MaxPlay.second){ //Maximize the Minimized UtilityValue
            MaxPlay.first=possibleMove;
            MaxPlay.second=s0Util.second;
        }
    }
                                                                                        if(debug){printLv();cout<<"Found Max! Move:"<<MaxPlay.first.first<<","<<MaxPlay.first.second<<"| UtilVal:"<<MaxPlay.second<<"\n"; leaveLv("MinMaxDecision",false); cout<<"D:0\n";}
    if((MaxPlay.first.first==-1 || MaxPlay.first.second==-1)){ cerr<<"Should not get here. Ref:1235 MinmaxDecision() File:AI.h\n"; exit(-1);}
    return MaxPlay.first;
}

struct HardAI{
    Reversi gState;
    Square player;
    int plyDepth;

    HardAI(const Reversi& CurrState, Square Player, int PlyDepth=1, bool Debug=false){
        player = Player;
        gState = CurrState;
        plyDepth = PlyDepth;
        debug=Debug;
    }

    Action findMax(){
        Action maxAction = MinmaxDecision(gState, player, plyDepth);
        if((maxAction.first==-1 || maxAction.second==-1)){ throw "Minmax Decision returned -1,-1\n";}
        return maxAction;
    }
};



