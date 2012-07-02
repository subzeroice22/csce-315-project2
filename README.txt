*****************
* Chris Voss	*
* Curtis Franks	*
* Laramie Goode	*
*****************

-----------------------------
=Reversi/Othello Game Readme=
-----------------------------

Our Reversi game program can run from a server and be connected through telnet or it can be run from just the console (console application). In order to run either the console or the server, a compiled version of the code is needed.


+++++++++++++++
| COMPILATION |
+++++++++++++++

===============================================================
The makefile can compile the files needed to play the game.

To compile the server application with the makefile use:
	make server

To compile the console application with the makefile use:
	make console_game

To manually compile the server application, type the following:
	g++ server.cpp UnitReversi.cpp -o server -lsocket

To manually compile the console application, type the folowing:
	g++ console.cpp UnitReversi.cpp -o console -lsocket


The makefile can also be used to run the server application.

===============================================================


+++++++++++++
| EXECUTION |
+++++++++++++

====================================================================================
To run the server with the makefile use:
	make run_server

The server can also be manually launched by ./server.
Once the server is running, the telnet client can be used to connect to it like so:
	telnet unix.cs.tamu.edu 5500

The console application can be manually launched by ./console.
The console also accepts arguments when being launched. 
The usage for the console application arguments is:	

	console.o [-p1 <MODE>] [-p2 <MODE>] [-e #] [--d #]
	<MODE> = HUMAN | EASY | MEDIUM | HARD
	-e	The number of games to execute
	-d	The depth to search for HARD AI (ignored if HARD AI is not selected)
====================================================================================
