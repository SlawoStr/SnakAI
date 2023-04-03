#include "Simulation.h"


int main()
{
	std::vector<unsigned> topology{ 28,20,12,4 };
	sf::Vector2u screenSize(500, 500);
	sf::Vector2i boardSize(10, 10);
	sf::Vector2u meshGrid(40, 25);
	int squareSize = 10;
	Simulation sim(topology,screenSize, boardSize,squareSize,meshGrid);
	sim.run();
	return 0;
}