#include "Simulation.h"
#include <iostream>
#include <chrono>

Simulation::Simulation(const std::vector<unsigned>& topology, sf::Vector2u windowSize, sf::Vector2i boardSize,int squareSize,sf::Vector2u meshGrid,int spaceDistance) : window(sf::VideoMode(windowSize.x,windowSize.y),"Simulation"),genetics(topology,false)
{
	if (meshGrid.x * meshGrid.y != GeneticSettings::NUMBER_OF_MODELS)
	{
		throw std::invalid_argument("Board Size should be equal to number of models in genetic algorithm");
	}
	// Create Grid of mehes(x*y)
	sf::Vector2i meshPosition(0, 0);
	for (unsigned i = 0; i < meshGrid.x; i++)
	{
		for (unsigned j = 0; j < meshGrid.y; j++)
		{
			snakes.push_back(Snake(Mesh(meshPosition, boardSize, squareSize)));
			meshPosition.x += boardSize.x * squareSize + spaceDistance;
		}
		meshPosition.y += boardSize.y * squareSize + spaceDistance;
		meshPosition.x = 0;
	}
	// Framerate limit
	window.setFramerateLimit(1000);
	this->panning = false;
	// Default zoom equal 1.0 scale
	this->zoomLevel = 1.0f;
	simulationView.setSize(float(windowSize.x), float(windowSize.y));
	simulationView.move(-float(windowSize.x / 2), -float(windowSize.y) / 2);
	simulationView.zoom(zoomLevel);
	bestScore = 0.0f;
	currentGeneration = 1;
	// Reset window position
	resetView();
}

void Simulation::run()
{
	while (window.isOpen())
	{
		//auto t_start = std::chrono::high_resolution_clock::now();
		window.clear(sf::Color(192,192,192));
		draw();
		window.display();
		update();
		pollEvent();
		//auto t_end = std::chrono::high_resolution_clock::now();
		//double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();
		//std::cout << "Drawing" << elapsed_time_ms << std::endl;
	}
}

void Simulation::pollEvent()
{
	sf::Event e;
	while (window.pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
		{
			window.close();
		}
		if (e.type == sf::Event::Resized)
		{
			// Set new size of view basec on resized window
			simulationView.setSize((float)e.size.width, (float)e.size.height);
			simulationView.zoom(zoomLevel);
			// Position range will increase or decrease when minimalized
			resetView();
		}
		if (e.type == sf::Event::MouseMoved)
		{
			if (panning)
			{
				// Get mouse offset and move simulation view according to it and zoom level
				sf::Vector2f pos = sf::Vector2f(sf::Mouse::getPosition(window) - this->panningAnchor);
				simulationView.move(-1.0f * pos * this->zoomLevel);
				panningAnchor = sf::Mouse::getPosition(window);
				resetView();
			}
		}
		if (e.type == sf::Event::MouseButtonPressed)
		{
			if (e.mouseButton.button == sf::Mouse::Middle)
			{
				if (!panning)
				{
					// Set panning flag and panning starting location
					panning = true;
					panningAnchor = sf::Mouse::getPosition(window);
				}
			}
		}
		if (e.type == sf::Event::MouseButtonReleased)
		{
			if (e.mouseButton.button == sf::Mouse::Middle)
			{
				// Release panning
				panning = false;
			}
		}
		// Zooming
		if (e.type == sf::Event::MouseWheelMoved)
		{
			if (e.mouseWheel.delta < 0)
			{
				simulationView.zoom(2.0f);
				zoomLevel *= 2.0f;
			}
			else
			{
				simulationView.zoom(0.5f);
				zoomLevel *= 0.5f;
			}
			resetView();
		}
	}
}

void Simulation::update()
{
	int counter = 0;
	for (int i = 0; i < snakes.size(); i++)
	{
		if (snakes[i].isAlive())
		{
			counter++;
			std::vector<double> result = genetics.predictOne(i, snakes[i].getVision());
			auto it = std::max_element(result.begin(), result.end());
			int direction = static_cast<int>(std::distance(result.begin(), it));
			if (direction == 0)
			{
				snakes[i].move(Move::UP);
			}
			else if (direction == 1)
			{
				snakes[i].move(Move::RIGHT);
			}
			else if (direction == 2)
			{
				snakes[i].move(Move::DOWN);
			}
			else if (direction == 3)
			{
				snakes[i].move(Move::LEFT);
			}
		}
	}
	if (counter==0)
	{
		double bestCurrent = 0.0f;
		std::vector<Fitness> fitness;
		fitness.reserve(GeneticSettings::NUMBER_OF_MODELS);
		for (int i = 0; i < snakes.size(); i++)
		{
			if (snakes[i].getFitness() > bestScore)
				bestScore = snakes[i].getFitness();
			if (snakes[i].getFitness() > bestCurrent)
				bestCurrent = snakes[i].getFitness();
			fitness.push_back(Fitness(snakes[i].getFitness(), i));
			snakes[i].resetSnake();
		}
		std::cout <<"Current Generation:"<<currentGeneration<<" Best score value:" << bestScore<<" Best current value: "<<bestCurrent << std::endl;
		// Start genetic algorithm evolution based on snakes scores
		genetics.createNewPopulation(fitness);
		currentGeneration++;
	}
}

void Simulation::draw()
{
	// Set View on window
	for (int i = 0; i < snakes.size(); i++)
		snakes[i].draw(window, windStart, windEnd);
	window.setView(simulationView);
}

void Simulation::resetView()
{
	// Get new window range
	windStart = sf::Vector2f(simulationView.getCenter().x - simulationView.getSize().x / 2, simulationView.getCenter().y - simulationView.getSize().y / 2);
	windEnd = sf::Vector2f(simulationView.getCenter().x + simulationView.getSize().x / 2, simulationView.getCenter().y + simulationView.getSize().y / 2);
}
