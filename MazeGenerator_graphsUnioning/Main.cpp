#include <stdio.h>	// scanf and printf
#include <time.h>	// wait function
#include <stdlib.h>	// malloc and calloc
#include <conio.h>	// _getch()
#include <queue>	// used by bfs
#include "olcConsoleGameEngine.h"
#include <condition_variable>
#include <mutex>
#include <array>
#include <unordered_map>

#define N 197	// number of lines
#define M 107	// number of columns

enum ScoreTypes
{
	// used when creating the maze
	BLOCK,
	EMPTY,

	// used when is being solved
	START,
	MAXDIST = (N * M),
	EXIT = MAXDIST + 1,

	// used when showing the solution
	LEFT,
	DOWN,
	RIGHT,
	UP,
};

// used to show the path
struct CellRepresentation
{
	wchar_t shape;
	short color;
};

static const std::unordered_map<ScoreTypes, CellRepresentation> cells = {
	{LEFT    , {L'<'       , FG_YELLOW}},
	{DOWN    , {L'V'       , FG_YELLOW}},
	{RIGHT   , {L'>'       , FG_YELLOW}},
	{UP      , {L'^'       , FG_YELLOW}},

	{BLOCK   , {PIXEL_SOLID, FG_DARK_BLUE}},
	{EMPTY   , {PIXEL_SOLID, FG_MAGENTA}},

	{START   , {PIXEL_SOLID, FG_YELLOW}},
	{MAXDIST , {L' '       , 15}},
	{EXIT    , {PIXEL_SOLID, FG_YELLOW}},
};

// controls when to create another maze and when to solve it
//bool restart = false;
//bool solve = false;

// controls the redrawing of the maze
std::condition_variable reDrawMaze;
std::mutex mtx;	

struct vec2
{
	int x, y;

	vec2() {}
	vec2(int x, int y) : x(x), y(y) {}

	// distance^2
	int distance2(const vec2& other) const 
	{
		const int dx = (other.x - x);
		const int dy = (other.y - y);
		return dx * dx + dy * dy;
	}

	const vec2 operator+ (const vec2& other) const
	{
		return vec2{ x + other.x, y + other.y };
	}

	vec2 operator/ (int devider) const
	{
		return vec2{ x / devider, y / devider };
	}

	vec2& operator= (const vec2& other)
	{
		if (this == &other)
			return *this;

		x = other.x;
		y = other.y;

		return *this;
	}

	bool operator== (const vec2& other) const
	{
		return (x == other.x) && (y == other.y);
	}

	bool operator!= (const vec2& other) const
	{
		return !(*this == other);
	}
};

void solveBFS(int** maze, const vec2& start, const vec2& exit)
{
	// set the distance to start position for each empty space = MaxDist
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= M; j++)
		{
			if (maze[i][j] != BLOCK)
				maze[i][j] = MAXDIST;
		}
	}

	// Mark the Start and the exit
	auto Mz = [maze](const vec2& v)->int& { return maze[v.x][v.y]; };
	Mz(exit) = EXIT;
	Mz(start) = START;

	auto cmp = [&exit](vec2 &v1, vec2 &v2) { return v1.distance2(exit) > v2.distance2(exit); };
	std::priority_queue<vec2, std::vector<vec2>, decltype(cmp)> pq(cmp);

	static const vec2 UP1{ 0, -1 };
	static const vec2 RIGHT1{ 1, 0 };
	static const vec2 DOWN1{ 0, 1 };
	static const vec2 LEFT1{ -1, 0 };

	vec2 dirs[] = { UP1, RIGHT1, DOWN1, LEFT1 };	// directions

	pq.push(start);

	vec2 closestPosInQueue, nextPos;
	bool print;
	while (!pq.empty())
	{
		closestPosInQueue = pq.top();
		pq.pop();

		print = false;
		for (const vec2& dir : dirs)
		{
			nextPos = closestPosInQueue + dir;

			if (maze[nextPos.x][nextPos.y] > maze[closestPosInQueue.x][closestPosInQueue.y] + 1)
			{
				maze[nextPos.x][nextPos.y] = maze[closestPosInQueue.x][closestPosInQueue.y] + 1;
				if (nextPos == exit)
					break;
				pq.push(nextPos);
				print = true;
			}
		}

		// if the look of the maze changed => redraw
		if (print)
		{
			reDrawMaze.notify_one();	// letting the maze object redraw the maze
			this_thread::sleep_for(15ms);
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

	ScoreTypes sdirs[] = { UP, RIGHT, DOWN, LEFT };	// The directions to be drawn
	// Mark the path starting from exit
	vec2 curr = exit;
	while (curr != start)
	{
		const int currDist = Mz(curr);
		for (int i = 0; i < 4; ++i)	// search for the next cell that is closer to the start
		{
			const vec2& dir = dirs[i];
			const vec2 next = curr + dir;
			const int distNext = Mz(next);
			if (Mz(next) != BLOCK && distNext < currDist)
			{
				Mz(curr) = sdirs[i];
				curr = next;
				break;	// there is no other smaller distance to be found
			}
		}

		reDrawMaze.notify_one();	// letting the maze object redraw the maze
		this_thread::sleep_for(10ms);
	}
}

// returns true with a probability of 1/nr
bool rand_true(int nr)
{
	return ((float)rand() / RAND_MAX < (float)1 / nr);
}

void createMaze(int** maze)
{
	srand((unsigned int)time(NULL));

	// used to search for neighbor nodes
	static const vec2 UP2{ 0, -2 };
	static const vec2 RIGHT2{ 2, 0 };
	static const vec2 DOWN2{ 0, 2 };
	static const vec2 LEFT2{ -2, 0 };

	vec2 dirs[] = { UP2, RIGHT2, DOWN2, LEFT2 };	// directions

	// add a Block/Wall on every even position
	for (int i = 0; i < N + 2; i++)
	{
		for (int j = 0; j < M + 2; j++)
		{
			maze[i][j] = BLOCK;
		}
	}

	// assign each non-Block/Wall to a graph
	int id_graph = 0;
	for (int i = 2; i < N; i += 2)
	{
		for (int j = 2; j < M; j += 2)
		{
			id_graph++;
			maze[i][j] = id_graph;
		}
	}

	int nrOfGraphs = id_graph;	// last id found is the number of graphs

	// returns a reference to maze[v.x][v.y]
	auto Mz = [maze](const vec2& v)->int& { return maze[v.x][v.y]; };

	vec2 saved_this_pos, saved_neigh_pos;	// used to create connection between them (delete the block between them)
	while (nrOfGraphs > 1)
	{
		// choose a random graph
		const int randX = (((rand()) % (N - 4)) / 2) * 2 + 2;	// gives a random odd number between 2 and n - 2
		const int randY = (((rand()) % (M - 4)) / 2) * 2 + 2;
		const int rand_graph_Id = maze[randX][randY];
		
		int nrOfNeighboringNodes = 0;	// used for randomly choosing between all neighbors so I can connect the graphs
		// iterate through all the nodes that are in the random graph
		for (int i = 2; i < N; i += 2)
		{
			for (int j = 2; j < M; j += 2)
			{
				vec2 this_pos = vec2(i, j);
				int curr_pos_graph_id = Mz(this_pos);
				if (curr_pos_graph_id == rand_graph_Id)	// if current cell is part of the random graph
				{
					for (const vec2& dir : dirs)	// for each neighbor
					{
						const vec2 neigh_pos(i + dir.x, j + dir.y);
						int neighb_graph_Id = Mz(neigh_pos);
						// if the neighbor node is from a different graph,
						// randomly choose if to connect the rand_Id_graph to this graph
						// by selecting it with a probability of 1/nrOfNeighboringNodes
						if (neighb_graph_Id != BLOCK
							&& neighb_graph_Id != rand_graph_Id)
						{
							// found a neighboring node that is not part of this graph => increment nrOfNeighboringNodes
							nrOfNeighboringNodes++;

							// randomly choose if to connect the neighb_graph_Id to this graph
							if (rand_true(nrOfNeighboringNodes))
							{
								saved_this_pos = this_pos;
								saved_neigh_pos = neigh_pos;
							}
						}
					}
				}
			}
		}

		int id_ng = Mz(saved_neigh_pos);	// id of a the neighbor graph

		Mz((saved_this_pos + saved_neigh_pos) / 2) = rand_graph_Id;	// the block between them = id_graph

		for (int i = 2; i < N; i += 2)
		{
			for (int j = 2; j < M; j += 2)
			{
				if (maze[i][j] == id_ng)	// convert the neighbor graph into this one
				{
					maze[i][j] = rand_graph_Id;
				}
			}
		}

		nrOfGraphs--;

		reDrawMaze.notify_one();	// letting the maze object redraw the maze
		this_thread::sleep_for(1ms);
	}

	// use prioritized BFS to search for a path between them
	vec2 startPos(2, M);
	vec2 endPos(N - 1, 1);
	solveBFS(maze, startPos, endPos);

	
	this_thread::sleep_for(10ms);
	reDrawMaze.notify_one();	// letting the maze object redraw the maze onne last time
}

class Maze 
	: public olcConsoleGameEngine
{
public:
	int** maze;

	Maze(int** maze) : maze(maze)
	{
		m_sAppName = L"Maze";
	}

	bool OnUserCreate()
	{

		return true;
	}

	bool OnUserUpdate(float deltaTime)
	{
		// if (m_keys[L'Q'].bHeld) { return false;	} // idk ... useless ... for now

		// wait until I'm let to redraw the maze
		reDrawMaze.wait(unique_lock<mutex>(mtx));

		// continuously draws the maze
		for (int i = 1; i < N + 1; i++)
		{
			for (int j = 1; j < M + 1; j++)
			{
				auto it = cells.find(static_cast<ScoreTypes>(maze[i][j]));
				if (it == cells.end())	// if it's the dist betw Start and this position => colour it as empty
					it = cells.find(EMPTY);

				Draw(i, j, it->second.shape, it->second.color);
			}
		}

		return true;
	}

	bool OnUserDestroy()
	{
		return false;
	}
};

int main()
{
	int** maze = (int**)malloc((N + 2) * sizeof(int*));
	for (int i = 0; i < N + 2; i++)
		maze[i] = (int*)malloc((M + 2) * sizeof(int));

	Maze maze_app = Maze(maze);
	maze_app.ConstructConsole(200, 110, 6, 6);

	std::thread first(&Maze::Start, maze_app);	// continuously draws the maze
	std::thread second(createMaze, maze);	// generates and solves the maze

	first.join();
	second.join();

	for (int i = 0; i < N + 2; i++)
		free(maze[i]);
	free(maze);

	return 0;
}
