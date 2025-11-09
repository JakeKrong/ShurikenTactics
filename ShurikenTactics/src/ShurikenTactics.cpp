// ShurikenTactics.cpp : Defines the entry point for the application.

#include "Game.h"
#include <iostream>

int main()
{
	try {
		Game game;
		game.Run();
	}
	catch (const std::exception& e){
		std::cerr << "Error: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
