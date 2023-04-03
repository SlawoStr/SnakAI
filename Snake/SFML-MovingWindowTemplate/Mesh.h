#pragma once
#include "SFML/Graphics.hpp"

enum class SquareType
{
	EMPTY,
	HEAD,
	TAIL,
	BODY,
	FOOD,
	BOTH,
};

class Mesh
{
public:
	Mesh(sf::Vector2i position, sf::Vector2i boardSize, int squareSize);

	void draw(sf::RenderTarget& target, sf::Vector2f startWin, sf::Vector2f endWin);
	// Check if this Mesh is currently visible on main window
	bool isInWindow(sf::Vector2f startWin, sf::Vector2f endWin);
	// Set square types to default value
	void resetMesh();
private:
	void drawLines(sf::RenderTarget& target, sf::Vector2f startWin, sf::Vector2f endWin);
private:
	// Starting location of this grid
	sf::Vector2i position;
	// Number of cells in board
	sf::Vector2i boardSize;
	// Size of each cell
	int squareSize;
	std::vector<std::vector<SquareType>> squareVec;
	sf::VertexArray quads;

	friend class Snake;
};