#include "Simulator.h"

Simulation2::Simulation2(int populationSize=1000, int boardSize=5, int generationNumber=100)
{
	this->populationSize = populationSize;
	this->boardSize = boardSize;
	this->currentgenerationNumber = -1;
	this->bestFitness = 0;
	this->generationNumber = generationNumber;
	this->nextGeneration = true;
	this->bestSnake = 1;
}

Simulation2::Simulation2()
{
	this->populationSize = 1000;
	this->boardSize = 5;
	this->currentgenerationNumber = -1;
	this->bestFitness = 0;
	this->generationNumber = 100;
	this->nextGeneration = true;
	this->bestSnake = 1;
}

void Simulation2::makeMove(std::vector<int> movesList)
{
	int counter = -1;
	nextGeneration = false;
	for (auto it = snakeList.begin(); it != snakeList.end(); it++)
	{
		counter++;
		if ((*it)->isAlive())
		{
			if (counter == bestSnake)
			{
				(*it)->showBoard();
				std::cout << "Prediction: " << movesList[counter] + 1 << std::endl;
			}
			(*it)->moveSnake(movesList[counter] + 1);
			nextGeneration = true;
		}
	}
}

std::vector<float> Simulation2::getFitness()
{
	float bestCurrentFitness = 0;
	int counter = -1;
	std::vector<float> fitnessVec;
	for (auto it = snakeList.begin(); it != snakeList.end(); it++)
	{
		counter++;
		float fitnessVal = (*it)->caclulateFitness();
		fitnessVec.push_back(fitnessVal);
		if (fitnessVal > bestFitness)
			bestFitness = fitnessVal;
		if (fitnessVal > bestCurrentFitness)
		{
			bestCurrentFitness = fitnessVal;
			bestSnake = counter;
		}
	}
	std::cout << "Current best fitness: " << bestCurrentFitness << std::endl;
	std::cout << "Best fitness: " << bestFitness << std::endl;
	return fitnessVec;
}

std::vector<std::vector<float>> Simulation2::getVision()
{
	std::vector<std::vector<float>> visionVec;
	for (auto it = snakeList.begin(); it != snakeList.end(); it++)
	{
		if ((*it)->isAlive())
		{
			(*it)->getVision();
			visionVec.push_back((*it)->getVectorVision());
		}
		else
		{
			visionVec.push_back(std::vector<float>(1, 0));
		}
	}
	return visionVec;
}

void Simulation2::resetPopulation()
{
	generationNumber++;
	for (int i = 0; i < snakeList.size(); i++)
	{
		delete snakeList[i];
	}
	snakeList.clear();

	for (int i = 0; i < populationSize; i++)
	{
		snakeList.push_back(new Snake(boardSize));
	}
	this->nextGeneration = true;
}