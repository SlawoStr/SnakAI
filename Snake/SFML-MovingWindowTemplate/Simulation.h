#pragma once
#include <SFML/Graphics.hpp>
#include "Snake.h"
#include "Mesh.h"
#include "GeneticAlgorithm.h"


class Simulation
{
public:
	Simulation(const std::vector<unsigned>& topology,sf::Vector2u windowSize, sf::Vector2i boardSize,int squareSize, sf::Vector2u meshGrid, int spaceDistance = 100);
	void run();
private:
	void pollEvent();
	void update();
	void draw();
	void resetView();
private:
	sf::RenderWindow window;
	// Panning varibales
	float zoomLevel;
	bool panning;
	sf::Vector2i panningAnchor;
	sf::View simulationView;
	// Current window position in global coordinates
	sf::Vector2f windStart;
	sf::Vector2f windEnd;
	// Grid
	std::vector<Snake> snakes;
	GeneticAlgorithm genetics;
	double bestScore;
	int currentGeneration;
};
