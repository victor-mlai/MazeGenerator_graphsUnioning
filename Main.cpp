#include <stdio.h>	// scanf and printf
#include <time.h>	// wait function
#include <stdlib.h>	// malloc and calloc
#include <conio.h>	// _getch()
#include <queue>	// used by bfs

#define N 25	// number of lines
#define M 21	// number of columns
#define BLOCK -1
#define MAXDIST 2*(N+M)
#define UP1 {0, -1}
#define RIGHT1 {1, 0}
#define DOWN1 {0, 1}
#define LEFT1 {-1, 0}
#define UP2 {0, -2}
#define RIGHT2 {2, 0}
#define DOWN2 {0, 2}
#define LEFT2 {-2, 0}

struct vec2
{
	int x, y;

	vec2() {}
	vec2(int x, int y) : x(x), y(y) {}

	vec2 operator+ (vec2& other) {
		return { x + other.x, y + other.y };
	}

	void operator= (vec2& other) {
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

void wait(float seconds)
{
	clock_t endwait = clock() + (clock_t)seconds * CLOCKS_PER_SEC;
	while (clock() < endwait) {}
}

void display(int **mat)
{
	int i, j;
	int block = 219;

	printf("\n");
	for (j = 0; j <= M + 1; j++)
		printf(" %-2d", j);
	printf("\n");
	for (i = 0; i <= N + 1; i++)
	{
		for (j = 0; j <= M + 1; j++)
			switch (mat[i][j])
			{
			case BLOCK: 
				printf("%c%c%c", block, block, block); 
				break;
			//case 1: printf("   "); break;
			//case 2: printf(" v "); break;
			//case 3: printf(" < "); break;
			//case 4: printf(" ^ "); break;
			//case 5: printf(" > "); break;
			default:
				printf("%-3d", mat[i][j]);
				break;
			}
		printf(" %d\n", i);
	}
	printf("\n");
}

void solveBFS(int** maze, vec2 start, vec2 exit) {
	for (int i = 2; i < N; i += 2) {
		for (int j = 2; j < M; j += 2) {
			maze[i][j] = MAXDIST;
		}
	}

	system("cls");
	display(maze);

	std::queue<vec2> q;
	vec2 d[] = { UP1, RIGHT1, DOWN1, LEFT1 };	// directions

	q.push(exit);

	vec2 F, V;
	while (!q.empty()) {
		F = q.front();
		q.pop();

		

		for (vec2 dir : d) {	// for each neighbour
			V = F + dir;

			if (maze[V.x][V.y] > maze[F.x][F.y] + 1) {
				maze[V.x][V.y] = maze[F.x][F.y] + 1;
				if (V == start)
					break;
				q.push(V);
			}
		}

		if (V == start)
			break;

		system("cls");
		display(maze);
	}

	system("cls");
	display(maze);
}

// returns true with a probability of 1/nr
bool cond(int nr) {
	return ((float)rand() / RAND_MAX < (float)1 / nr);
}

void createMaze(int** maze) {
	int i, j;
	int id_graph; // current graph id
	int id_ng;	// new graph id
	int nrOfGraphs;
	int rx, ry;
	int k;
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

	display(maze);
	nrOfGraphs = id_graph; // nr grafuri

	vec2 x, y;	// create connection between x and y (delete the block betwwen them)
	int nr;
	while (nrOfGraphs > 1) {
		rx = (((rand()) % (N - 4)) / 2) * 2 + 2;	// gives a random odd number between 2 and n - 2
		ry = (((rand()) % (M - 4)) / 2) * 2 + 2;
		id_graph = maze[rx][ry];	// choose a random graph
		nr = 1;
		for (i = 2; i < N; i += 2) {
			for (j = 2; j < M; j += 2) {
				if (maze[i][j] == id_graph) {
					for (vec2 dir : d) {	// for each neighbour
						if (maze[i + dir.x][j + dir.y] != BLOCK &&
							maze[i + dir.x][j + dir.y] != id_graph) {
							if (cond(nr)) {
								x = vec2(i + dir.x, j + dir.y);
								y = vec2(i, j);
								nr++;
							}
						}
					}
				}
			}
		}

		id_ng = maze[x.x][x.y];

		maze[(x.x + y.x) / 2][(x.y + y.y) / 2] = MAXDIST;

		for (i = 2; i < N; i += 2) {
			for (j = 2; j < M; j += 2) {
				if (maze[i][j] == id_graph) {
					maze[i][j] = id_ng;
				}
			}
		}

		//system("cls");
		//display(maze);
		//wait(0.3f);
		nrOfGraphs--;
	}

	maze[N - 1][1] = MAXDIST;
	maze[2][M] = 0;
}

int main()
{
	srand((unsigned int)time(NULL));

	int** maze = (int**)malloc((N + 2) * sizeof(int*));
	for (int i = 0; i < N + 2; i++)
		maze[i] = (int*)malloc((M + 2) * sizeof(int));

	while (true) {
		createMaze(maze);

		system("cls");
		display(maze);

		printf("\n Afisati solutie?\nd\\n\n");
		if (_getch() == 'd')
			solveBFS(maze, vec2(N - 1, 1), vec2(2, M));
	
		printf("\n Display another maze?\nd\\n\n");
		if (_getch() == 'n')
			break;
	}

	printf("\n Press any key to exit\n");
	_getch();	// waits any key

	return 0;
}
