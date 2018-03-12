#include <stdio.h>	// scanf and printf
#include <time.h>	// wait function
#include <stdlib.h>	// malloc and calloc
#include <conio.h>	// _getch()
#include <queue>	// used by bfs
#include "olcConsoleGameEngine.h"
#include <condition_variable>
#include <mutex>

#define N 197	// number of lines
#define M 107	// number of columns
#define MAXDIST (N*M)

#define BLOCK -1
#define EMPTY 0
#define START 1
#define EXIT (MAXDIST + 1)

// used to show the path
#define UP    -2
#define RIGHT -3
#define DOWN  -4
#define LEFT  -5

// used by bfs
#define UP1 {0, -1}
#define RIGHT1 {1, 0}
#define DOWN1 {0, 1}
#define LEFT1 {-1, 0}

// used by createMaze function to search for neighbour nodes
#define UP2 {0, -2}
#define RIGHT2 {2, 0}
#define DOWN2 {0, 2}
#define LEFT2 {-2, 0}

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

	int distance(vec2 other) {
		return (other.x - x) * (other.x - x) + (other.y - y) * (other.y - y);
	}

	vec2 operator+ (vec2& other) {
		return { x + other.x, y + other.y };
	}

	void operator= (const vec2& other) {
		x = other.x;
		y = other.y;
	}

	bool operator== (vec2& other) {
		return (x == other.x) && (y == other.y);
	}

	bool operator!= (vec2& other) {
		return (x != other.x) || (y != other.y);
	}
};

void solveBFS(int** maze, vec2 start, vec2 exit) {
	// set the distance to start position for each empty space = MaxDist
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			if (maze[i][j] != BLOCK)
				maze[i][j] = MAXDIST;
		}
	}

	// Mark the Start and the exit
	auto Mz = [maze](vec2& v)->int& { return maze[v.x][v.y]; };
	Mz(exit) = EXIT;
	Mz(start) = START;
	// maze[exit.x][exit.y] = EXIT;
	// maze[start.x][start.y] = START;
		
	auto cmp = [&exit](vec2 v1, vec2 v2) { return v1.distance(exit) > v2.distance(exit); };
	std::priority_queue<vec2, std::vector<vec2>, decltype(cmp)> q(cmp);

	vec2 d[] = { UP1, RIGHT1, DOWN1, LEFT1 };	// directions

	q.push(start);

	vec2 T, V;
	bool print;
	while (!q.empty()) {
		T = q.top();
		q.pop();

		print = false;
		for (vec2 dir : d) {	// for each neighbour
			V = T + dir;

			if (maze[V.x][V.y] > maze[T.x][T.y] + 1) {
				maze[V.x][V.y] = maze[T.x][T.y] + 1;
				if (V == exit)
					break;
				q.push(V);
				print = true;
			}
		}

		// if the look of the maze changed => redraw
		if (print) {
			reDrawMaze.notify_one();	// letting the maze object redraw the maze
			this_thread::sleep_for(35ms);
		}

		if (V == exit)
			break;
	}

	// No solution exists
	if (V != exit) {
		//printf("No solution exists\n");
		return;
	}

	vec2 curr = exit;
	int min = maze[curr.x][curr.y];
	std::vector<vec2> path;
	std::vector<int> dirs;
	path.push_back(exit);
	dirs.push_back(LEFT);

	while (curr != start) {
		for (int i = 0; i < 4; i++) {
			V = curr + d[i];
			if (maze[V.x][V.y] != BLOCK && maze[V.x][V.y] < min) {
				curr = V;
				min = maze[V.x][V.y];
				path.push_back(curr);
				dirs.push_back(i - 5);
			}
		}
	}
	dirs.push_back(LEFT);

	for (int i = 1; i < path.size() - 1; i++) {
		maze[path[i].x][path[i].y] = dirs[i];
		reDrawMaze.notify_one();	// letting the maze object redraw the maze
		this_thread::sleep_for(10ms);
	}

}

// returns true with a probability of 1/nr
bool cond(int nr) {
	return ((float)rand() / RAND_MAX < (float)1 / nr);
}

void createMaze(int** maze) {
	srand((unsigned int)time(NULL));

	int i, j;
	int id_graph; // current graph id
	int id_ng;	// new graph id
	int nrOfGraphs;
	int rx, ry;
	vec2 d[] = { UP2, RIGHT2, DOWN2, LEFT2 };	// directions

	for (int i = 0; i < N + 2; i++) {
		for (int j = 0; j < M + 2; j++) {
			maze[i][j] = BLOCK;
		}
	}

	id_graph = 0;
	for (i = 2; i < N; i += 2) {
		for (j = 2; j < M; j += 2) {
			id_graph++;
			maze[i][j] = id_graph;
		}
	}

	nrOfGraphs = id_graph; // nr grafuri

	vec2 x, y;	// create connection between x and y (delete the block betwwen them)
	int nr;
	while (nrOfGraphs > 1) {
		rx = (((rand()) % (N - 4)) / 2) * 2 + 2;	// gives a random odd number between 2 and n - 2
		ry = (((rand()) % (M - 4)) / 2) * 2 + 2;
		id_graph = maze[rx][ry];	// choose a random graph
		nr = 1;
		for (i = 2; i < N; i += 2) {	// search for all nodes that are in the random graph
			for (j = 2; j < M; j += 2) {
				if (maze[i][j] == id_graph) {
					for (vec2 dir : d) {	// for each neighbour
						// if the neighbour node is from a different graph
						if (maze[i + dir.x][j + dir.y] != BLOCK &&
							maze[i + dir.x][j + dir.y] != id_graph) {
							// there is a chance of 1/nr to choose him to connect the 2 graphs
							if (cond(nr)) {
								x = vec2(i + dir.x, j + dir.y);	// neighbour node
								y = vec2(i, j);	// this node
								nr++;	// number of pairs (x, y) to choose from
							}
						}
					}
				}
			}
		}

		id_ng = maze[x.x][x.y];	// id of a the neighbour graph

		maze[(x.x + y.x) / 2][(x.y + y.y) / 2] = id_graph;	// connect the 2 graphs

		for (i = 2; i < N; i += 2) {
			for (j = 2; j < M; j += 2) {
				if (maze[i][j] == id_ng) {	// convert the neighbour graph into this one
					maze[i][j] = id_graph;
				}
			}
		}

		nrOfGraphs--;

		reDrawMaze.notify_one();	// letting the maze object redraw the maze
		this_thread::sleep_for(1ms);
	}

	// use prioritized BFS to search for a path between them
	solveBFS(maze, vec2(2, M), vec2(N - 1, 1));

	
	this_thread::sleep_for(10ms);
	reDrawMaze.notify_one();	// letting the maze object redraw the maze onne last time
}

class Maze : public olcConsoleGameEngine {
public:
	int** maze;

	Maze(int** maze) : maze(maze) {
		m_sAppName = L"Maze";
	}

	bool OnUserCreate() {

		return true;
	}

	bool OnUserUpdate(float deltaTime) {
		// if (m_keys[L'Q'].bHeld) { return false;	} // idk ... useless ... for now

		// wait until I'm let to redraw the maze
		reDrawMaze.wait(unique_lock<mutex>(mtx));

		// continously draws the maze
		for (int i = 1; i < N + 1; i++) {
			for (int j = 1; j < M + 1; j++) {
				switch (maze[i][j])
				{
				case BLOCK :
					Draw(i, j, PIXEL_SOLID, FG_DARK_BLUE);	// draw a solid block
					break;
				case UP:    Draw(i, j, L'>', FG_YELLOW); break;
				case RIGHT: Draw(i, j, L'^', FG_YELLOW); break;
				case DOWN:  Draw(i, j, L'<', FG_YELLOW); break;
				case LEFT:  Draw(i, j, L'v', FG_YELLOW); break;
				case MAXDIST: Draw(i, j, L' ');	break;	// draw nothing 
				case START: Draw(i, j, PIXEL_SOLID, FG_YELLOW);	break;	// draw nothing 
				case EXIT:  Draw(i, j, PIXEL_SOLID, FG_YELLOW);	break;	// draw nothing 
				default:
					Draw(i, j, PIXEL_SOLID, FG_MAGENTA);
					break;
				}
			}
		}
		return true;
	}

	bool OnUserDestroy() {
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

	std::thread first(&Maze::Start, maze_app);	// continously draws the maze
	std::thread second(createMaze, maze);	// generates and solves the maze

	first.join();
	second.join();

	/*
	while (true) {
		createMaze(maze);

		system("cls");
		DebugDisplay(maze);
		
		printf("\n Show solution?\nd\\n\n");
		if (_getch() == 'd')
			
	
		printf("\n Display another maze?\nd\\n\n");
		if (_getch() == 'n')
			break;
	}

	printf("\n Press any key to exit\n");
	_getch();	// waits any key
	*/
	return 0;
}
