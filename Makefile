server.o: 	server.cpp
		g++ server.cpp UnitReversi.cpp -o game -lsocket
