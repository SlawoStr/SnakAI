#include "Mesh.h"

Mesh::Mesh(sf::Vector2i position, sf::Vector2i boardSize, int squareSize) : quads(sf::Quads)
{
	this->position = position;
	this->boardSize = boardSize;
	this->squareSize = squareSize;
	squareVec.resize(boardSize.y);
	for (int i = 0; i < boardSize.y; i++)
	{
		squareVec[i].resize(boardSize.x);
	}
	resetMesh();
}

void Mesh::draw(sf::RenderTarget& target, sf::Vector2f startWin, sf::Vector2f endWin)
{
	int startY = int((startWin.y - position.y)) / squareSize;
	int endY = int((endWin.y - position.y)) / squareSize;
	int startX = int((startWin.x - position.x)) / squareSize;
	int endX = int((endWin.x - position.x)) / squareSize;
	//                                Fix out of bounds Ranges                              
	if (startY < 0)
	{
		startY = 0;
	}
	if (startX < 0)
	{
		startX = 0;
	}
	if (endY >= squareVec.size())
	{
		endY = int(squareVec.size() - 1);
	}
	if (endX >= squareVec[0].size())
	{
		endX = int(squareVec[0].size() - 1);
	}
	quads.resize(4 * ((endY - startY) + 1) * ((endX - startX) + 1));
	sf::Vector2f rectPosition;
	int counter = 0;
	for (int i = startY; i <= endY; i++)
	{
		for (int j = startX; j <= endX; j++)
		{
			rectPosition.y = float(position.y + i * squareSize);
			rectPosition.x = float(position.x + j * squareSize);

			sf::Vertex* quad = &quads[unsigned(counter++) * 4];

			quad[0].position = sf::Vector2f(rectPosition);
			quad[1].position = sf::Vector2f(rectPosition.x + squareSize, rectPosition.y);
			quad[2].position = sf::Vector2f(rectPosition.x + squareSize, rectPosition.y + squareSize);
			quad[3].position = sf::Vector2f(rectPosition.x, rectPosition.y + squareSize);
			if (squareVec[i][j] == SquareType::EMPTY)
			{
				quad[0].color = sf::Color::White;
				quad[1].color = sf::Color::White;
				quad[2].color = sf::Color::White;
				quad[3].color = sf::Color::White;
			}
			else if (squareVec[i][j] == SquareType::BODY)
			{
				quad[0].color = sf::Color::Green;
				quad[1].color = sf::Color::Green;
				quad[2].color = sf::Color::Green;
				quad[3].color = sf::Color::Green;
			}
			else if (squareVec[i][j] == SquareType::FOOD)
			{
				quad[0].color = sf::Color::Red;
				quad[1].color = sf::Color::Red;
				quad[2].color = sf::Color::Red;
				quad[3].color = sf::Color::Red;
			}
			else if (squareVec[i][j] == SquareType::HEAD)
			{
				quad[0].color = sf::Color::Yellow;
				quad[1].color = sf::Color::Yellow;
				quad[2].color = sf::Color::Yellow;
				quad[3].color = sf::Color::Yellow;
			}
			else if (squareVec[i][j] == SquareType::TAIL)
			{
				quad[0].color = sf::Color::Blue;
				quad[1].color = sf::Color::Blue;
				quad[2].color = sf::Color::Blue;
				quad[3].color = sf::Color::Blue;
			}
		}
	}
	target.draw(quads);

	drawLines(target, startWin, endWin);
}

bool Mesh::isInWindow(sf::Vector2f startWin, sf::Vector2f endWin)
{
	// Check if grid range is contained in window range
	if (startWin.x < position.x + boardSize.x * squareSize
		&& startWin.x + (endWin.x - startWin.x) > position.x
		&& startWin.y < position.y + boardSize.y * squareSize
		&& startWin.y + (endWin.y - startWin.y) > position.y)
	{
		return true;
	}
	return false;
}

void Mesh::resetMesh()
{
	for(int i=0;i<squareVec.size();i++)
		for (int j = 0; j < squareVec[0].size(); j++)
		{
			squareVec[i][j] = SquareType::EMPTY;
		}
}

void Mesh::drawLines(sf::RenderTarget& target, sf::Vector2f startWin, sf::Vector2f endWin)
{
	// DRAW VERTICAL LINES                             
	sf::VertexArray lines(sf::Lines);

	sf::Vector2f startLine((float)int(startWin.x), (float)int(startWin.y));

	// Set up starting location for line
	if (startLine.x < float(position.x))
		startLine.x = float(position.x);
	if (startLine.y = float(position.y))
		startLine.y = float(position.y);
	// Check if first line is in correct position
	int divideRest = int(startLine.x) % squareSize;
	if (divideRest != 0)
	{
		// Move startLine to correct position (depend on squareSize)
		startLine.x = (float)int(startLine.x + (squareSize - divideRest));
	}

	sf::Vector2f endLine;
	endLine.x = startLine.x;
	endLine.y = endWin.y;

	// Check if endLine is in range
	if (endLine.y > position.y + boardSize.y * squareSize)
		endLine.y = float(position.y + boardSize.y * squareSize);
	if (startLine.y > position.y + boardSize.y * squareSize || endLine.y < position.y)
	{
		// Out of Range
	}
	else
	{
		while (startLine.x <= endWin.x && startLine.x <= position.x + boardSize.x * squareSize)
		{
			lines.append(sf::Vertex(startLine, sf::Color::Black));
			lines.append(sf::Vertex(endLine, sf::Color::Black));
			startLine.x += squareSize;
			endLine.x += squareSize;
		}
	}
	target.draw(lines);
	lines.clear();
	
	// DRAW HORIZONTAL LINES (almost the same as Vertical but with changed x-axis with y-axis)                             
	startLine.x = (float)int(startWin.x);
	startLine.y = (float)int(startWin.y);
	if (startLine.x < float(position.x))
		startLine.x = float(position.x);
	if (startLine.y < float(position.y))
		startLine.y = float(position.y);

	divideRest = int(startLine.y) % squareSize;
	if (divideRest != 0)
	{
		startLine.y = (float)int(startLine.y + (squareSize - divideRest));
	}

	endLine.y = startLine.y;
	endLine.x = endWin.x;

	if (endLine.x > position.x + boardSize.x * squareSize)
		endLine.x = float(position.x + boardSize.x * squareSize);

	if (startLine.x > position.x + boardSize.x * squareSize || endLine.x < position.x)
	{
		// Out of Range
	}
	else
	{
		while (startLine.y <= endWin.y && startLine.y <= position.y + boardSize.y * squareSize)
		{
			lines.append(sf::Vertex(startLine, sf::Color::Black));
			lines.append(sf::Vertex(endLine, sf::Color::Black));
			startLine.y += squareSize;
			endLine.y += squareSize;
		}
	}
	target.draw(lines);
	
}
