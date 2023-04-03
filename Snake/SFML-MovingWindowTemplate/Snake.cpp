#include "Snake.h"
#include <iostream>

Snake::Snake(Mesh mesh) : mesh(mesh)
{
	randGenerator = std::make_unique<RandomIntGenerator>(mesh.boardSize.x - 1, mesh.boardSize.y - 1);
	resetSnake();
}

void Snake::draw(sf::RenderTarget& target,sf::Vector2f startWin, sf::Vector2f endWin)
{
	if (mesh.isInWindow(startWin, endWin))
	{
		mesh.draw(target, startWin, endWin);
	}
}

void Snake::resetSnake()
{
	mesh.resetMesh();

	fruitNumber = 0;
	stepNumber = 0;
	stepsLeft = SnakeSettings::maximumSteps;
	alive = true;

	snakeBody.clear();
	// Set up starting position
	sf::Vector2i startingPos(mesh.boardSize.x / 2, mesh.boardSize.y / 2);
	snakeBody.push_back(startingPos);
	snakeBody.push_back(sf::Vector2i(startingPos.x - 1, startingPos.y));
	snakeBody.push_back(sf::Vector2i(startingPos.x - 2, startingPos.y));

	mesh.squareVec[startingPos.y][startingPos.x] = SquareType::HEAD;
	mesh.squareVec[startingPos.y][startingPos.x - 1] = SquareType::BODY;
	mesh.squareVec[startingPos.y][startingPos.x - 2] = SquareType::TAIL;

	headDirection = Move::RIGHT;

	spawnFruit();
}

void Snake::move(Move direction)
{
	sf::Vector2i headPosition = snakeBody.front();
	// Get new HeadPosition based move direction
	if (direction == Move::UP)
	{
		headPosition.y -= 1;
	}
	else if (direction == Move::DOWN)
	{
		headPosition.y += 1;
	}
	else if (direction == Move::RIGHT)
	{
		headPosition.x += 1;
	}
	else if (direction == Move::LEFT)
	{
		headPosition.x -= 1;
	}
	// If snake hit boundary wall
	if (headPosition.x < 0 || headPosition.x >= mesh.boardSize.x || headPosition.y < 0 || headPosition.y >= mesh.boardSize.y)
	{
		alive = false;
		return;
	}
	// If snake try to move in the opposit direction to current head direction (check illegal moves)
	if (direction == Move::UP && headDirection == Move::DOWN || direction == Move::DOWN && headDirection == Move::UP
	 || direction == Move::LEFT && headDirection == Move::RIGHT || direction == Move::RIGHT && headDirection == Move::LEFT)
	{
		alive = false;
		return;
	}
	// Set up new head direction
	headDirection = direction;

	SquareType nextField = mesh.squareVec[headPosition.y][headPosition.x];
	if (nextField == SquareType::BODY)
	{
		alive = false;
		return;
	}
	else if (nextField == SquareType::FOOD)
	{
		fruitNumber++;
		stepsLeft = SnakeSettings::maximumSteps;
		spawnFruit();
	}
	else
	{
		sf::Vector2i tailPosition = snakeBody.back();
		mesh.squareVec[tailPosition.y][tailPosition.x] = SquareType::EMPTY;
		snakeBody.pop_back();
		mesh.squareVec[snakeBody.back().y][snakeBody.back().x] = SquareType::TAIL;
	}

	// Update head position
	mesh.squareVec[snakeBody.front().y][snakeBody.front().x] = SquareType::BODY;
	mesh.squareVec[headPosition.y][headPosition.x] = SquareType::HEAD;
	snakeBody.push_front(headPosition);

	stepNumber++;
	stepsLeft--;
	// Snake didint find food within maximal step number
	if (stepsLeft == 0)
	{
		alive = false;
	}
}

std::vector<double> Snake::getVision()
{
	std::vector<double>vision;
	std::vector<double>visionFixed;
	vision.resize(28);
	visionFixed.reserve(28);

	int counter = 0;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (i == 0 && j == 0)
				continue;
			std::pair<double, SquareType> result = getVisionDistance(j, i);
			vision[counter++] = result.first;
			if (result.second == SquareType::FOOD)
			{
				vision[counter++] = 1;
				vision[counter++] = 0;
			}
			else if (result.second == SquareType::BODY)
			{
				vision[counter++] = 0;
				vision[counter++] = 1;
			}
			else if (result.second == SquareType::BOTH)
			{
				vision[counter++] = 1;
				vision[counter++] = 1;
			}
			else
			{
				vision[counter++] = 0;
				vision[counter++] = 0;
			}
		}
	}
	float divider = float(std::max(mesh.boardSize.x, mesh.boardSize.y));

	for (int i = 0; i < vision.size(); i += 3)
	{
		vision[i] /= divider;
	}
	// Check where head is pointing and rearrange vision positions based on direction
	for (int i = 24; i < 28; i++)
	{
		vision[i] = 0;
	}
	if (headDirection == Move::UP)
	{
		vision[24] = 1;
	}
	else if (headDirection == Move::RIGHT)
	{
		vision[25] = 1;
	}
	else if (headDirection == Move::DOWN)
	{
		vision[26] = 1;
	}
	else if (headDirection == Move::LEFT)
	{
		vision[27] = 1;
	}

	return vision;
}

double Snake::getFitness()
{
	return stepNumber + (pow(2, fruitNumber) + (pow(fruitNumber, 2.1) * 500)) - (pow(fruitNumber, 1.2) * pow(0.25 * stepNumber, 1.3));
}

void Snake::spawnFruit()
{
	sf::Vector2i fruitPosition;
	fruitPosition.x = (*randGenerator).getRandomXRange();
	fruitPosition.y = (*randGenerator).getRandomYRange();
	while (true)
	{
		if (mesh.squareVec[fruitPosition.y][fruitPosition.x] != SquareType::EMPTY)
		{
			fruitPosition.x = (*randGenerator).getRandomXRange();
			fruitPosition.y = (*randGenerator).getRandomYRange();
		}
		else
		{
			mesh.squareVec[fruitPosition.y][fruitPosition.x] = SquareType::FOOD;
			break;
		}
	}
}

std::pair<double, SquareType> Snake::getVisionDistance(int xOffset, int yOffset)
{
	sf::Vector2i position = snakeBody.front();
	SquareType returnType = SquareType::EMPTY;
	int counter = 0;
	position.x += xOffset;
	position.y += yOffset;
	while (position.x >= 0 && position.x < mesh.boardSize.x && position.y >= 0 && position.y < mesh.boardSize.y)
	{
		SquareType squareType = mesh.squareVec[position.y][position.x];
		if (squareType == SquareType::BODY || squareType == SquareType::TAIL)
		{
			if (returnType == SquareType::EMPTY )
			{
				returnType = SquareType::BODY;
			}
			else if (returnType == SquareType::FOOD)
			{
				returnType = SquareType::BOTH;
			}
		}
		else if (squareType == SquareType::FOOD)
		{
			if (returnType == SquareType::BODY)
			{
				returnType = SquareType::BOTH;
			}
			if (returnType == SquareType::EMPTY)
			{
				returnType = squareType;
			}
		}
		counter++;
		position.x += xOffset;
		position.y += yOffset;
	}
	return std::make_pair(counter, returnType);
}
