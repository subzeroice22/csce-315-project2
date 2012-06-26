#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include "UnitReversiMain.h"


void printUsage(std::string argName="console.o"){
	std::cout<<"\nUsage: "<< argName <<" [-p1 <MODE>] [-p2 <MODE>] [-e #] [-d #]\n";
	std::cout<<"<MODE> = HUMAN | EASY | MEDIUM | HARD\n";
	std::cout<<"-e\tThe number of games to execute\n";
	std::cout<<"-d\tThe depth to search for HARD AI (ignored if HARD AI is not selected)\n\n";
}

int main ( int argc, char *argv[] ){
	
	if(argc==1){
		cout<<"No arguments provided, using default values (-p1 HUMAN -p2 EASY -e 1)\n";
		totalExecutions=1;
		AIlevelP1 = "OFF";
		AIlevelP2 = "EASY";
	}
	else if (argc>9){
		std::cerr<<"Error: Too many input arguments\n";
		printUsage(argv[0]);
		return -1;
	}
	else if (argc==2){
		printUsage(argv[0]);
		return -1;
	}
	for(int i=1; i<argc; i+=2){
		std::string hook=argv[i], val = argv[i+1];
		
		if(hook=="-p1"){
			if(val !="HUMAN" && val!="EASY" && val!="MEDIUM" && val!="HARD"){
				std::cerr<<"Error: -p1 must be followed by <MODE>.\n";
				printUsage(argv[0]);
				return -1;
			}
			AIlevelP1 = ((val=="HUMAN")?("OFF"):(val));
		}
		else if(hook=="-p2"){
			if(val !="HUMAN" && val!="EASY" && val!="MEDIUM" && val!="HARD"){
				std::cerr<<"Error: -p2 must be followed by <MODE>.\n";
				printUsage(argv[0]);
				return -1;
			}
			AIlevelP2 = ((val=="HUMAN")?("OFF"):(val));
		}
		else if(hook=="-e"){
			char * endptr=NULL;
			int exeC = strtol(val.c_str(), &endptr, 10);
			if(endptr==NULL){/* the whole string is garbage - no numbers extracted */
				cerr<<"Error: in command arguments while processing -e . Must be integer\n";
				return -1;
			}
			else if(*endptr==0){/* the whole string was a number - yay! */
				totalExecutions = exeC;
			}
			else{/* strtol extracted a number from the string, but stopped at some invalid character that you can check by looking at the value of endptr */
				cerr<<"Error: in command arguments while processing -e. Invalid Character: "<<endptr<<"\n";
				return -1;
			}
		}
		else if(hook=="-d"){
			char * endptr2=NULL;
			int maxD = strtol(val.c_str(), &endptr2, 10);
			if(endptr2==NULL){/* the whole string is garbage - no numbers extracted */
				cerr<<"Error: in command arguments while processing -d . Must be integer\n";
				return -1;
			}
			else if(*endptr2==0){/* the whole string was a number - yay! */
				maxDepth = maxD;
			}
			else{/* strtol extracted a number from the string, but stopped at some invalid character that you can check by looking at the value of endptr */
				cerr<<"Error: in command arguments while processing -d. Invalid Character: "<<endptr2<<"\n";
				return -1;
			}
		}
		else{
			std::cerr<<"Error: Invalid input arguments\n";
			printUsage(argv[0]);
		}
	}
	gameStart reversi;
	reversi.api("START", 0);
	return 0;
}


