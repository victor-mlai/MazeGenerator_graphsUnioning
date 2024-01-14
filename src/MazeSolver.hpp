#pragma once

#include "MazeView.hpp"
#include <queue>

void SolveBFS(MazeView& app, const Coord2D start, const Coord2D exit)
{
	Grid& grid = app.GetGrid();

	// set the distance to start position for each empty space = MaxDist
	for (int i = 1; i < grid.rows - 1; i++)
	{
		for (int j = 1; j < grid.cols - 1; j++)
		{
			if (grid.Get({ i, j }) != CellType::BLOCK)
			{
				grid.GetMut({ i, j }) = CellType::MAXDIST;
			}
		}
	}

	grid.GetMut(exit) = CellType::EXIT;
	grid.GetMut(start) = CellType::START;

	/*co_await*/ app.TriggerDraw();

	auto cmp = [&exit](Coord2D& v1, Coord2D& v2) { return v1.distance2(exit) > v2.distance2(exit); };
	std::priority_queue<Coord2D, std::vector<Coord2D>, decltype(cmp)> pq(cmp);

	// directions: up, right, down, left
	static const Coord2D dirs[] = { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };

	pq.push(start);

	Coord2D nextPos;
	bool print;
	while (!pq.empty())
	{
		Coord2D closestPosInQueue = pq.top();
		pq.pop();

		print = false;
		for (const Coord2D& dir : dirs)
		{
			nextPos = closestPosInQueue + dir;

			const int dist = grid.Get(closestPosInQueue) + 1;
			if (grid.Get(nextPos) > dist)
			{
				grid.GetMut(nextPos) = (CellType)dist;
				if (nextPos == exit)
					break;
				pq.push(nextPos);
				print = true;
			}
		}

		// if the look of the maze changed => redraw
		if (print)
		{
			/*co_await*/ app.TriggerDraw();
		}

		if (nextPos == exit)
			break;
	}

	// No solution exists
	if (nextPos != exit)
	{
		//printf("No solution exists\n");
		return;
	}

	// The directions to be drawn
	static const CellType sdirs[] = {
		CellType::UP, CellType::RIGHT, CellType::DOWN, CellType::LEFT };
	// Mark the path starting from exit
	Coord2D curr = exit;
	while (curr != start)
	{
		const int currDist = grid.Get(curr);
		for (int i = 0; i < 4; ++i)	// search for the next cell that is closer to the start
		{
			const Coord2D& dir = dirs[i];
			const Coord2D next = curr + dir;
			const int distNext = grid.Get(next);
			if (grid.Get(next) != CellType::BLOCK && distNext < currDist)
			{
				grid.GetMut(curr) = sdirs[i];
				curr = next;
				break;	// there is no other smaller distance to be found
			}
		}

		/*co_await*/ app.TriggerDraw();
	}
}
