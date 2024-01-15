#include "MazeModel.hpp"
#include "MazeCreator.hpp"
#include "MazeSolver.hpp"
#include "MazeView.hpp"

int main()
{
	constexpr int rows = 89;
	constexpr int cols = 57;
	static_assert(
		(rows % 2 == 1) && (cols % 2 == 1),
		"The algorithm works only with odd values");
	constexpr int cellSize = 12;

	// You might want to also adjust the deltaTime for larger mazes
	using namespace std::literals;
	constexpr auto createDelay = 1000ns;
	// in case the solver is going too fast
	constexpr auto solverDelay = 5000ns;

	MazeView mazeApp{ rows, cols };
	mazeApp.SetDelay(createDelay);
	mazeApp.Construct(rows, cols, cellSize, cellSize);

	CreateMazeOpts opts{
		.seed = std::nullopt, // let the algo choose a random one
	};
	std::thread constrThrd(&CreateMaze, std::ref(mazeApp), std::ref(opts));
	std::thread drawingThrd(&MazeView::Start, std::ref(mazeApp));
	constrThrd.join();

	mazeApp.SetDelay(solverDelay);

	// use prioritized BFS to search for a path between them
	Coord2D startPos{ 2, cols - 2 };
	Coord2D endPos{ rows - 3, 1 };
	std::thread solverThrd(&SolveBFS, std::ref(mazeApp), startPos, endPos);
	solverThrd.join();

	drawingThrd.join();
	return 0;
}
