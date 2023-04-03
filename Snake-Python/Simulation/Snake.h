#pragma once
#include <list>
#include <vector>

struct Coordintes
{
	Coordintes(int x, int y) { this->x = x; this->y = y; }
	Coordintes() { this->x = 0; this->y = 0; }
	int x;
	int y;
};

class Snake
{
public:
	Snake(int boardSize);
	~Snake();
	double caclulateFitness();
	void showBoard();
private:
	static int snakeCounter;
	int numberOfFruits;
	int numberOfSteps;
	int boardSize;
	int** simulationBoard;
	int snakeDirection;
	std::list<Coordintes> snakeBody;
	bool alive;
	int numberOfCurrentSteps;
	int fruitNumber;
	bool punishment;
	int vision[24];
public:
	bool isAlive();
	bool checkConditions();
	void moveSnake(int direction);
	bool checkFood();
	int getSnakeDirection() { return snakeDirection; }
	void getVision();
	int getNumberOfFood() { return numberOfFruits; }
	int getNumberOfMoves() { return numberOfSteps; }
	int** getBoard();
	Coordintes getHead() { return snakeBody.front(); }
	std::vector<float> getVectorVision();
};