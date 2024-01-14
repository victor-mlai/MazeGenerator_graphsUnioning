#pragma once

#include "Utils.hpp"
#include "MazeView.hpp"

#include <random>
#include <optional>

struct CreateMazeOpts {
	std::optional<std::random_device::result_type> seed = std::nullopt;
};

void CreateMaze(MazeView& app, const CreateMazeOpts& opts = {})
{
	std::random_device device;
	std::mt19937 gen(opts.seed.value_or(device()));

	Grid& grid = app.GetGrid();

	// add a Block/Wall on every even position
	for (int i = 0; i < grid.rows; i++)
	{
		for (int j = 0; j < grid.cols; j++)
		{
			grid.GetMut({ i, j }) = BLOCK;
		}
	}

	/*co_await*/ app.TriggerDraw();

	// assign each non-Block/Wall to a graph
	int graphId = 0;
	for (int i = 2; i < grid.rows - 2; i += 2)
	{
		for (int j = 2; j < grid.cols - 2; j += 2)
		{
			graphId++;
			grid.GetMut({ i, j }) = (CellType)graphId;
		}
	}

	/*co_await*/ app.TriggerDraw();

	std::uniform_int_distribution distr;

	// returns true with a probability of 1/nr
	auto RandTrue = [&](int nr) -> bool
	{
		const float lhs = (float)distr(device) / std::numeric_limits<int>::max();
		const float rhs = 1.f / nr;
		return lhs < rhs;
	};

	auto FindGraphCells = [&](const int randGraphId) {
		std::vector<Coord2D> neighboringCells;
		for (int i = 2; i < grid.rows - 2; i += 2)
		{
			for (int j = 2; j < grid.cols - 2; j += 2)
			{
				const int currGraphId = grid.Get({ i, j });
				if (currGraphId == randGraphId)
				{
					neighboringCells.push_back({ i, j });
				}
			}
		}
		return neighboringCells;
	};

	// last id found is the number of graphs
	int nrOfGraphs = graphId;
	while (nrOfGraphs > 1) // unite graphs until 1 remains
	{
		// Choose a random graph to be united to a neighboring graph
		const int randGraphId = [&] {
			// this formula gives a random odd number between 2 and n - 2
			const int randX = ((distr(device) % (grid.rows / 2 - 1)) + 1) * 2;
			const int randY = ((distr(device) % (grid.cols / 2 - 1)) + 1) * 2;
			return grid.Get({ randX, randY });
		}();
		const std::vector<Coord2D> randGraphCells = FindGraphCells(randGraphId);

		// used to create connection between them (delete the block between them)
		Coord2D toUniteRandomGraphCoord, toUniteNeighboringGraphCoord;
		int nrOfNeighboringNodes = 0;
		for (const Coord2D cell : randGraphCells)
		{
			// directions: up, right, down, left
			static const Coord2D dirs[] = { { 0, -2 }, { 2, 0 }, { 0, 2 }, { -2, 0 } };
			for (const Coord2D& dir : dirs)	// for each neighbor
			{
				const Coord2D neighCoord = cell + dir;
				const int neighGraphId = grid.Get(neighCoord);
				// if the neighbor node is from a different graph,
				// randomly choose if to connect the randGraphId to this graph
				// by selecting it with a probability of 1/nrOfNeighboringNodes
				if (neighGraphId != BLOCK
					&& neighGraphId != randGraphId
					&& RandTrue(++nrOfNeighboringNodes))
				{
					toUniteRandomGraphCoord = cell;
					toUniteNeighboringGraphCoord = neighCoord;
				}
			}
		}

		// convert the block in-between to randGraphId
		grid.GetMut((toUniteRandomGraphCoord + toUniteNeighboringGraphCoord) / 2) = (CellType)randGraphId;
		// convert the neighbor graph into randGraph
		const int neighGraphId = grid.Get(toUniteNeighboringGraphCoord);
		const std::vector<Coord2D> neighGraphCells = FindGraphCells(neighGraphId);
		for (const Coord2D cell : neighGraphCells)
		{
			grid.GetMut(cell) = (CellType)randGraphId;
		}

		nrOfGraphs--;
		/*co_await*/ app.TriggerDraw();
	}
}
