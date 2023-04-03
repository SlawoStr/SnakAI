#pragma once
#include "Snake.h"
#include <list>
#include <iostream>
#include <vector>

class Simulation2
{
public:
	Simulation2(int populationSize, int boardSize, int generationsNumber);
	Simulation2();
private:
	int populationSize;
	int boardSize;
	int currentgenerationNumber;
	float bestFitness;
	int generationNumber;
	std::vector<Snake*> snakeList;
	int nextGeneration;
	int bestSnake;
public:
	bool isAlive() { return nextGeneration; }
	void makeMove(std::vector<int>movesList);
	std::vector<float> getFitness();
	std::vector<std::vector<float>> getVision();
	void resetPopulation();
	int getBestSnake() { return bestSnake; }
};