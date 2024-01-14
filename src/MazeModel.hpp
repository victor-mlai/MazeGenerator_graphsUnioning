#pragma once

#include "Utils.hpp"
#include <vector>

enum CellType
{
	// used when creating the maze
	BLOCK,
	EMPTY,

	// used when is being solved
	START = 10'000,
	// <- in-between values representing the distance to the start
	MAXDIST = 100'000,
	EXIT = MAXDIST + 1,

	// used when showing the solution
	LEFT,
	DOWN,
	RIGHT,
	UP,
};

struct Grid
{
	Grid(const int rows, const int cols)
		: CELL_COLOUR_MAP(rows* cols, CellType::BLOCK)
		, rows(rows)
		, cols(cols)
	{}

	CellType Get(const Coord2D pos) const {
		return CELL_COLOUR_MAP[pos.x * cols + pos.y];
	}

	CellType& GetMut(const Coord2D pos) {
		return CELL_COLOUR_MAP[pos.x * cols + pos.y];
	}

	std::vector<CellType> CELL_COLOUR_MAP;
	int rows;
	int cols;
};
