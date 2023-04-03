#pragma once
#include <deque>
#include "Mesh.h"
#include "SFML/Graphics.hpp"
#include "RandomNumberGenerator.h"

namespace SnakeSettings
{
	const int maximumSteps = 100;   // How many steps can snake survive without eating
}

enum class Move
{
	UP,
	RIGHT,
	DOWN,
	LEFT
};

class Snake
{
public:
	Snake(Mesh mesh);

	void draw(sf::RenderTarget& target, sf::Vector2f startWin, sf::Vector2f endWin);
	void resetSnake();
	void move(Move direction);
	std::vector<double> getVision();
	bool isAlive() { return alive; }
	double getFitness();
private:
	void spawnFruit();
	std::pair<double,SquareType> getVisionDistance(int xOffset,int yOffset);
private:
	Mesh mesh;
	// SnakeBody
	std::deque<sf::Vector2i> snakeBody;
	// Progress variables
	int fruitNumber;
	int stepNumber;
	int stepsLeft;
	Move headDirection;
	bool alive;
	// Random number for food position placement
	std::unique_ptr<RandomIntGenerator> randGenerator;
};