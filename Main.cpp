#include <stdio.h>	// scanf and printf
#include <time.h>	// wait function
#include <stdlib.h>	// malloc and calloc
#include <conio.h>	// _getch()

#define BLOCK 0
#define N {0, -2}
#define E {2, 0}
#define W {-2, 0}
#define S {0, 2}

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
};

void wait(float seconds)
{
	clock_t endwait = clock() + (clock_t)seconds * CLOCKS_PER_SEC;
	while (clock() < endwait) {}
}

void afisare(int **mat, int height, int width)
{
	int i, j;
	int zid = 219;

	printf("\n");
	for (j = 1; j <= width; j++)
		printf(" %-2d", j);
	printf("\n");
	for (i = 1; i <= height; i++)
	{
		for (j = 1; j <= width; j++)
			switch (mat[i][j])
			{
			case BLOCK: 
				printf("%c%c%c", zid, zid, zid); 
				break;
			//case 1: printf("   "); break;
			//case 2: printf(" v "); break;
			//case 3: printf(" < "); break;
			//case 4: printf(" ^ "); break;
			//case 5: printf(" > "); break;
			default:
				printf("   ");
				break;
			}
		printf(" %d\n", i);
	}
	printf("\n");
}

void solveBFS(int** maze, vec2 start, vec2 exit) {

}

// returneaza true cu o probabilitate de 1/nr
bool cond(int nr) {
	return ((float)rand() / RAND_MAX < (float)1 / nr);
}

int main()
{
	int i, j, n, m, r;
	int id_graph, id_ng; 
	int nrOfGraphs; // nr grafuri
	int rx, ry;
	int k;
	vec2 d[5] = { N, E, S, W };	// directions
	srand((unsigned int)time(NULL));

	n = 17;	// number of lines
	m = 15; // number of columns

	int** maze = (int**)malloc((n + 2) * sizeof(int*));
	for (i = 0; i < n + 2; i++)
		maze[i] = (int*)calloc((m + 2), sizeof(int));

	// creare labirint: conex
	while (true) {
		id_graph = 0;
		for (i = 2; i < n; i += 2) {
			for (j = 2; j < m; j += 2) {
				id_graph++;
				maze[i][j] = id_graph;
			}
		}

		afisare(maze, n, m);
		nrOfGraphs = id_graph; // nr grafuri

		vec2 x, y;
		int nr;
		while (nrOfGraphs > 1) {
			// choose a random graph
			rx = (((rand()) % (n - 4)) / 2) * 2 + 2;	// gives a random odd number between 2 and n - 2
			ry = (((rand()) % (m - 4)) / 2) * 2 + 2;
			id_graph = maze[rx][ry];
			nr = 1;
			for (i = 2; i < n; i += 2) {
				for (j = 2; j < m; j += 2) {
					if (maze[i][j] == id_graph) {
						for (k = 0; k < 4; k++) {	// for each neighbour
							if (maze[i + d[k].x][j + d[k].y] != 0 &&
								maze[i + d[k].x][j + d[k].y] != id_graph) {
								if (cond(nr)) {
									x.x = i + d[k].x;
									x.y = j + d[k].y;
									y.x = i;
									y.y = j;
									nr++;
								}
							}
						}
					}
				}
			}

			maze[(x.x + y.x) / 2][(x.y + y.y) / 2] = 1;

			id_ng = maze[x.x][x.y];

			for (i = 2; i < n; i += 2) {
				for (j = 2; j < m; j += 2) {
					if (maze[i][j] == id_graph) {
						maze[i][j] = id_ng;
					}
				}
			}

			system("cls");
			afisare(maze, n, m);
			//wait((float)0.1);
			nrOfGraphs--;
		}

		maze[n - 1][1] = 1;
		maze[2][m] = 1;

		system("cls");
		afisare(maze, n, m);

		//printf("\n Afisati solutie?\nd\\n\n");
		//if (_getch() == 'n')
		//	return 0;
	
		printf("\n Display another maze?\nd\\n\n");
		if (_getch() == 'n')
			break;

		for (int i = 0; i < n + 2; i++) {
			for (int j = 0; j < m + 2; j++) {
				maze[i][j] = 0;
			}
		}
	}
	

	/* rezolvare labirint */
	solveBFS(maze, vec2(n - 1, 1), vec2(2, m));

	_getch();	// waits any key

	return 0;
}
