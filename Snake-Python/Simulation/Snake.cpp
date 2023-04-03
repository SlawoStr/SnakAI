#include "Snake.h"
#include <cmath>
#include <iostream>
#include <time.h>
#include <string>

int Snake::snakeCounter = 0;

Snake::Snake(int boardSize)
{
	snakeCounter++;
	if (snakeCounter == 1)
		srand(time(NULL));
	///Create Dynamic board that will keep snake position
	this->boardSize = boardSize;
	simulationBoard = new int* [boardSize];
	for (int i = 0; i < boardSize; i++)
	{
		simulationBoard[i] = new int[boardSize];
	}

	// Fill array with zeros (0 = empty space 1 = snake body 2 = fruit)
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			simulationBoard[i][j] = 0;

	// Initialize values 
	numberOfFruits = 0;
	numberOfSteps = 0;
	alive = true;
	punishment = false;
	numberOfCurrentSteps = 0;

	//Initialize starting position of Snake

	Coordintes headCoordinates;
	headCoordinates.x = 3;
	headCoordinates.y = 3;

	snakeDirection = 2;
	snakeBody.push_back(Coordintes(headCoordinates.x, headCoordinates.y));
	snakeBody.push_back(Coordintes(headCoordinates.x - 1, headCoordinates.y));
	snakeBody.push_back(Coordintes(headCoordinates.x - 2, headCoordinates.y));

	for (auto it = snakeBody.begin(); it != snakeBody.end(); it++)
	{
		simulationBoard[(*it).y][(*it).x] = 1;
	}

	for (int i = 0; i < 1; i++)
	{
		Coordintes cords;
		cords.x = rand() % boardSize;
		cords.y = rand() % boardSize;
		while (simulationBoard[cords.y][cords.x] == 1 || simulationBoard[cords.y][cords.x] == 2 || cords.y == snakeBody.front().y || cords.x == snakeBody.front().x)
		{
			cords.x = rand() % boardSize;
			cords.y = rand() % boardSize;
		}
		simulationBoard[cords.y][cords.x] = 2;
	}
}

Snake::~Snake()
{
	for (int i = 0; i < boardSize; i++)
		delete[]simulationBoard[i];
	delete[] simulationBoard;
}

double Snake::caclulateFitness()
{

	if (punishment)
	{
		return numberOfSteps + (pow(2, numberOfFruits) + (pow(numberOfFruits, 2.1) * 500)) - (pow(numberOfFruits, 1.2) * pow(0.25 * numberOfSteps, 1.3));
		//return pow(numberOfFruits + 1, 3) * numberOfSteps ;
	}
	else
	{
		return numberOfSteps + (pow(2, numberOfFruits) + (pow(numberOfFruits, 2.1) * 500)) - (pow(numberOfFruits, 1.2) * pow(0.25 * numberOfSteps, 1.3));
		//return pow(numberOfFruits + 1, 3) * numberOfSteps;
	}
}

void Snake::showBoard()
{
	for (int i = 0; i < boardSize; i++)
	{
		for (int j = 0; j < boardSize; j++)
			std::cout << simulationBoard[i][j] << "  ";
		std::cout << std::endl;
	}
}

bool Snake::isAlive()
{
	return alive;
}

bool Snake::checkConditions()
{
	Coordintes faceCoordinates(snakeBody.front().x, snakeBody.front().y);
	//Check Collisiion with Wall
	if (faceCoordinates.x < 0 || faceCoordinates.x >= boardSize || faceCoordinates.y < 0 || faceCoordinates.y >= boardSize)
		return false;
	//CheckCollision with Body
	if (simulationBoard[faceCoordinates.y][faceCoordinates.x] == 1)
		return false;
	return true;
}

void Snake::moveSnake(int direction)
{
	int blockedDirection = snakeDirection > 2 ? snakeDirection - 2 : snakeDirection + 2;
	if (blockedDirection != direction)
	{
		snakeDirection = direction;
	}
	switch (snakeDirection)
	{
	case 1:
		snakeBody.push_front(Coordintes(snakeBody.front().x, snakeBody.front().y - 1));
		break;
	case 2:
		snakeBody.push_front(Coordintes(snakeBody.front().x + 1, snakeBody.front().y));
		break;
	case 3:
		snakeBody.push_front(Coordintes(snakeBody.front().x, snakeBody.front().y + 1));
		break;
	case 4:
		snakeBody.push_front(Coordintes(snakeBody.front().x - 1, snakeBody.front().y));
		break;
	}
	alive = checkConditions();
	if (!alive)
	{
		return;
	}
	bool food = checkFood();
	simulationBoard[snakeBody.front().y][snakeBody.front().x] = 1;
	if (!food)
	{
		Coordintes lastElement = snakeBody.back();
		simulationBoard[lastElement.y][lastElement.x] = 0;
		snakeBody.pop_back();
	}
	else
	{
		numberOfCurrentSteps = 0;
		fruitNumber--;

		if (fruitNumber <= 0 && numberOfFruits < 10)
		{
			Coordintes cords;
			cords.x = rand() % boardSize;
			cords.y = rand() % boardSize;
			while (simulationBoard[cords.y][cords.x] == 1 || simulationBoard[cords.y][cords.x] == 2 || cords.y == snakeBody.front().y || cords.x == snakeBody.front().x)
			{
				cords.x = rand() % boardSize;
				cords.y = rand() % boardSize;
			}
			simulationBoard[cords.y][cords.x] = 2;
		}
		else
		{
			Coordintes cords;
			cords.x = rand() % boardSize;
			cords.y = rand() % boardSize;
			while (simulationBoard[cords.y][cords.x] == 1 || simulationBoard[cords.y][cords.x] == 2)
			{
				cords.x = rand() % boardSize;
				cords.y = rand() % boardSize;
			}
			simulationBoard[cords.y][cords.x] = 2;
		}
		numberOfFruits += 1;
	}
	Coordintes lastElement = snakeBody.back();
	snakeBody.pop_back();
	Coordintes secondLastElement = snakeBody.back();
	snakeBody.push_back(lastElement);
	numberOfSteps += 1;
	numberOfCurrentSteps += 1;
	if (numberOfCurrentSteps > 100)
	{
		alive = false;
		punishment = true;
	}
}

bool Snake::checkFood()
{
	return simulationBoard[snakeBody.front().y][snakeBody.front().x] == 2 ? true : false;
}


void Snake::getVision()
{

	auto it = snakeBody.begin();
	Coordintes coordinates = (*it);
	int direction = this->getSnakeDirection();
	vision[0] = coordinates.y;

	bool body = false;
	int fruitDistance = 0;
	int bodyDistance = 0;
	for (int i = 0; i < coordinates.y; i++)
	{
		if (simulationBoard[i][coordinates.x] == 2)
		{
			fruitDistance = coordinates.y - i;
		}
		if (simulationBoard[i][coordinates.x] == 1)
		{
			bodyDistance = coordinates.y - i;
		}
	}

	vision[1] = fruitDistance;
	vision[2] = bodyDistance;
	body = false;
	Coordintes snakeCoordinates = coordinates;
	int distance = 0;
	fruitDistance = 0;
	bodyDistance = 0;

	while (snakeCoordinates.x + 1 < boardSize && snakeCoordinates.y - 1 >= 0)
	{
		distance += 1;
		snakeCoordinates.x += 1;
		snakeCoordinates.y -= 1;
		if (simulationBoard[snakeCoordinates.y][snakeCoordinates.x] == 2)
		{
			fruitDistance = distance;
		}
		else if (simulationBoard[snakeCoordinates.y][snakeCoordinates.x] == 1 && !body)
		{
			body = true;
			bodyDistance = distance;
		}
	}
	vision[3] = distance;
	vision[4] = fruitDistance;
	vision[5] = bodyDistance;
	fruitDistance = 0;
	bodyDistance = 0;
	vision[6] = boardSize - coordinates.x - 1;

	body = false;
	for (int i = coordinates.x + 1; i < boardSize; i++)
	{
		if (simulationBoard[coordinates.y][i] == 2)
		{
			fruitDistance = i - coordinates.x;
		}
		if (simulationBoard[coordinates.y][i] == 1 && !body)
		{
			body = true;
			bodyDistance = i - coordinates.x;
		}
	}
	vision[7] = fruitDistance;
	vision[8] = bodyDistance;
	fruitDistance = 0;
	bodyDistance = 0;

	body = false;
	distance = 0;
	snakeCoordinates = coordinates;
	while (snakeCoordinates.x + 1 < boardSize && snakeCoordinates.y + 1 < boardSize)
	{
		distance += 1;
		snakeCoordinates.x += 1;
		snakeCoordinates.y += 1;
		if (simulationBoard[snakeCoordinates.y][snakeCoordinates.x] == 2)
		{
			fruitDistance = distance;
		}
		else if (simulationBoard[snakeCoordinates.y][snakeCoordinates.x] == 1 && !body)
		{
			body = true;
			bodyDistance = distance;
		}
	}

	vision[9] = distance;
	vision[10] = fruitDistance;
	vision[11] = bodyDistance;
	fruitDistance = 0;
	bodyDistance = 0;

	vision[12] = boardSize - coordinates.y - 1;

	body = false;
	for (int i = coordinates.y + 1; i < boardSize; i++)
	{
		if (simulationBoard[i][coordinates.x] == 2)
		{
			fruitDistance = i - coordinates.y;
		}
		if (simulationBoard[i][coordinates.x] == 1 && !body)
		{
			body = true;
			bodyDistance = i - coordinates.y;
		}
	}

	vision[13] = fruitDistance;
	vision[14] = bodyDistance;

	body = false;
	distance = 0;
	fruitDistance = 0;
	bodyDistance = 0;

	snakeCoordinates = coordinates;
	while (snakeCoordinates.x - 1 >= 0 && snakeCoordinates.y + 1 < boardSize)
	{
		distance += 1;
		snakeCoordinates.x -= 1;
		snakeCoordinates.y += 1;
		if (simulationBoard[snakeCoordinates.y][snakeCoordinates.x] == 2)
		{
			fruitDistance = distance;
		}
		else if (simulationBoard[snakeCoordinates.y][snakeCoordinates.x] == 1 && !body)
		{
			body = true;
			bodyDistance = distance;
		}
	}

	vision[15] = distance;
	vision[16] = fruitDistance;
	vision[17] = bodyDistance;


	vision[18] = coordinates.x;

	body = false;
	fruitDistance = 0;
	bodyDistance = 0;

	for (int i = 0; i < coordinates.x; i++)
	{
		if (simulationBoard[coordinates.y][i] == 2)
		{
			fruitDistance = coordinates.x - i;
		}
		if (simulationBoard[coordinates.y][i] == 1)
		{
			bodyDistance = coordinates.x - i;
			body = true;
		}
	}
	vision[19] = fruitDistance;
	vision[20] = bodyDistance;

	body = false;
	fruitDistance = 0;
	bodyDistance = 0;
	distance = 0;
	snakeCoordinates = coordinates;
	while (snakeCoordinates.x - 1 >= 0 && snakeCoordinates.y - 1 >= 0)
	{
		distance += 1;
		snakeCoordinates.x -= 1;
		snakeCoordinates.y -= 1;
		if (simulationBoard[snakeCoordinates.y][snakeCoordinates.x] == 2)
		{
			fruitDistance = distance;
		}
		else if (simulationBoard[snakeCoordinates.y][snakeCoordinates.x] == 1 && !body)
		{
			body = true;
			bodyDistance = distance;
		}
	}

	vision[21] = distance;
	vision[22] = fruitDistance;
	vision[23] = bodyDistance;

	int newVision[24];
	int startingPoint;
	if (direction == 1)
		startingPoint = 0;
	else if (direction == 2)
		startingPoint = 6;
	else if (direction == 3)
		startingPoint = 12;
	else if (direction == 4)
		startingPoint = 18;
	for (int i = 0; i < 24; i++)
	{
		newVision[i] = vision[startingPoint++];
		if (startingPoint > 23)
			startingPoint = 0;
	}
	for (int i = 0; i < 24; i++)
	{
		vision[i] = newVision[i];
	}
}

int** Snake::getBoard()
{
	return simulationBoard;
}

std::vector<float> Snake::getVectorVision()
{
	std::vector<float> visionVec;
	int counter = 0;
	for (int i = 0; i < *(&vision + 1) - vision; i++)
	{
		counter++;
		if (counter % 2 == 0)
		{
			if (vision[i] == 0)
			{
				visionVec.push_back(0);
			}
			else
			{
				visionVec.push_back(1);
			}
		}
		else  if (counter % 3 == 0)
		{
			if (vision[i] == 0)
			{
				visionVec.push_back(0);
			}
			else
			{
				visionVec.push_back(1);
			}
			counter = 0;
		}
		else
		{
			visionVec.push_back(float(vision[i]) / (boardSize - 1));
		}
	}
	int tab[4];
	for (int i = 0; i < 4; i++)
		tab[i] = 0;
	tab[getSnakeDirection() - 1] = 1;
	for (int i = 0; i < 4; i++)
	{
		visionVec.push_back(tab[i]);
	}
	for (int i = 0; i < visionVec.size(); i++)
	{
		if (visionVec[i] > 1 || visionVec[i] < 0)
		{
			std::cout << "ERROR" << std::endl;
		}
	}
	return visionVec;
}

