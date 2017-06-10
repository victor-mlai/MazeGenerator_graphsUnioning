#include <stdio.h>	// scanf and printf
#include <time.h>	// wait function
#include <stdlib.h>	// malloc and calloc
#include <conio.h>	// _getch()

typedef struct
{
	int x, y;
}node;

void wait(float seconds)
{
	clock_t endwait;
	endwait = clock() + (clock_t)seconds * CLOCKS_PER_SEC;
	while (clock() < endwait) {}
}

int valid(int **mat, node st[500], int k)
{
	int i;
	if (mat[st[k].x][st[k].y] == 0)
		return 0;

	for (i = 1; i<k; i++)
		if (st[k].x == st[i].x && st[k].y == st[i].y)
			return 0;
	return 1;
}

void afisare(int **mat, int height, int width)
{
	int i, j;
	int zid = 254;

	printf("\n");
	for (j = 1; j <= width; j++)
		printf(" %-2d", j);
	printf("\n");
	for (i = 1; i <= height; i++)
	{
		for (j = 1; j <= width; j++)
			switch (mat[i][j])
			{
			case 0: printf("%c%c%c", zid, zid, zid); break;
			case 1: printf("   "); break;
			case 2: printf(" v "); break;
			case 3: printf(" < "); break;
			case 4: printf(" ^ "); break;
			case 5: printf(" > "); break;
			default: break;
			}
		printf(" %d\n", i);
	}
	printf("\n");
}

void tipar(int **mat, node st[500], int k, int *ok, int *p, int height, int width)
{
	int i;
	*ok = 1;
	mat[height - 1][1] = 5;

	for (i = 1; i<k; i++)
	{
		switch (p[i + 1])
		{
			case 1:  mat[st[i].x][st[i].y] = 5;  break;
			default: mat[st[i].x][st[i].y] = p[i + 1];  break;
		}
		system("cls");
		afisare(mat, height, width);
	}

	switch (p[i])
	{
		case 1:  mat[st[i].x][st[i].y] = 5;  break;
		default: mat[st[i].x][st[i].y] = p[i];  break;
	}

	system("cls");
	afisare(mat, height, width);
}

int verif(int **maze, int Ai, int Aj, int Bi, int Bj, int n, int m)
{
	int suma = 0;
	if (Ai<1 || Aj<1 || Ai>n || Aj>m || Bi<1 || Bj<1 || Bi>n || Bj>m)
		return 0;
	if (maze[Ai][Aj] == 0)
		return 0;
	if (Ai == Bi && Aj == Bj)
		return 1;
	maze[Ai][Aj] = 0;
	suma += verif(maze, Ai, Aj + 1, Bi, Bj, n, m) +
		verif(maze, Ai + 1, Aj, Bi, Bj, n, m) +
		verif(maze, Ai, Aj - 1, Bi, Bj, n, m) +
		verif(maze, Ai - 1, Aj, Bi, Bj, n, m);
	maze[Ai][Aj] = 1;
	return suma;   // returnez nr de drumuri de la A la B
}

int main()
{
	int **maze, i, j, n, m, r;
	srand((unsigned int)time(NULL));

	// number of lines and columns
	n = 21;
	m = 21;

	maze = (int**)malloc((n + 1) * sizeof(int*));
	for (i = 1; i <= n; i++)
		maze[i] = (int*)calloc(m + 1, sizeof(int));

	// creare labirint: conex

	int g = 0; // nr grafuri
	int nr = 0; // nr noduri
	int graph1 = 0, graph2 = 0;

	node **v = (node **)malloc(1000 * sizeof(node*));
	for (i = 0; i<1000; i++)
		v[i] = (node *)calloc(10000, sizeof(node));
	node *a = (node*)calloc(10000, sizeof(node));

	for (i = 2; i<n; i += 2)
		for (j = 2; j<m; j += 2)
		{
			g++;
			v[g][0].x = 1;	// nr de noduri
							// coordonatele nodului
			v[g][1].x = i;
			v[g][1].y = j;
		}

	while (g>1)
	{
		r = (rand()) % g + 1;
		nr = 0;

		for (i = 1; v[r][i].x != 0; i += 2)
		{
			if (v[r][i].x + 2<n)
				if (verif(maze, v[r][i].x, v[r][i].y, v[r][i].x + 2, v[r][i].y, n, m) == 0)
				{
					nr++;
					a[nr].x = v[r][i].x + 1;
					a[nr].y = v[r][i].y;
				}
			if (v[r][i].y + 2<m)
				if (verif(maze, v[r][i].x, v[r][i].y, v[r][i].x, v[r][i].y + 2, n, m) == 0)
				{
					nr++;
					a[nr].x = v[r][i].x;
					a[nr].y = v[r][i].y + 1;
				}
			if (v[r][i].x - 2>1)
				if (verif(maze, v[r][i].x, v[r][i].y, v[r][i].x - 2, v[r][i].y, n, m) == 0)
				{
					nr++;
					a[nr].x = v[r][i].x - 1;
					a[nr].y = v[r][i].y;
				}
			if (v[r][i].y - 2>1)
				if (verif(maze, v[r][i].x, v[r][i].y, v[r][i].x, v[r][i].y - 2, n, m) == 0)
				{
					nr++;
					a[nr].x = v[r][i].x;
					a[nr].y = v[r][i].y - 1;
				}
		}

		r = (rand()) % nr + 1;
		maze[a[r].x][a[r].y] = 1;

		if (a[r].x % 2 == 0)
		{
			maze[a[r].x][a[r].y + 1] = 1;
			maze[a[r].x][a[r].y - 1] = 1;
			for (i = 1; i <= g; i++)
				for (j = 1; v[i][j].x != 0; j++)
				{
					if (a[r].y + 1 == v[i][j].y && a[r].x == v[i][j].x)
						graph1 = i;
					if (a[r].y - 1 == v[i][j].y && a[r].x == v[i][j].x)
						graph2 = i;
				}
		}
		else
		{
			maze[a[r].x + 1][a[r].y] = 1;
			maze[a[r].x - 1][a[r].y] = 1;
			for (i = 1; i <= g; i++)
				for (j = 1; v[i][j].x != 0; j++)
				{
					if (a[r].y == v[i][j].y && a[r].x + 1 == v[i][j].x)
						graph1 = i;
					if (a[r].y == v[i][j].y && a[r].x - 1 == v[i][j].x)
						graph2 = i;
				}
		}

		for (j = 1; v[graph1][j].x != 0; j++) {}
		v[graph1][j].x = 1;
		for (i = 1; v[graph2][i].x != 0; i++)
		{
			v[graph1][j + i].x = v[graph2][i].x;
			v[graph1][j + i].y = v[graph2][i].y;
		}
		v[graph2] = v[g];
		g--;

		system("cls");
		afisare(maze, n, m);
		//wait((float)0.1);
	}

	maze[n - 1][1] = 1;
	maze[2][m] = 1;

	system("cls");
	afisare(maze, n, m);

	printf("\n Afisati solutie?\nd\\n\n");
	if (_getch() == 'n')
		return 0;

	/* rezolvare labirint */
	maze[n - 1][1] = 0;	// inchid intrarea de jos stanga ca sa o pot gasi pe cea de sus stanga

	// "stiva" ce retine solutia
	node *st = (node*)calloc((n*m / 2), sizeof(node));

	// vector ce retine directia
	int *p = (int*)calloc((n*m / 2), sizeof(int));

	// solutia incepe din fata intrarii de jos stanga
	st[1].x = n - 1;
	st[1].y = 2;

	//bt();

	int k, as, ev, ok = 0;
	node d[5] = { { 0,0 }, { 0,1 }, { 1,0 }, { 0,-1 }, { -1,0 } };

	k = 2;
	while (k>1)
	{
		as = 1;	// avem solutie
		ev = 0;	// este valida?
		while (as != 0 && ev != 1)
		{
			if (p[k]<4)
			{
				p[k]++;
				st[k].x = st[k - 1].x + d[p[k]].x;
				st[k].y = st[k - 1].y + d[p[k]].y;
				as = 1;
				ev = valid(maze, st, k);
			}
			else
				as = 0;
		}
		if (as)
			if (st[k].x == 1 || st[k].x == n || st[k].y == 1 || st[k].y == m)
			{
				tipar(maze, st, k, &ok, p, n, m);
				break;
			}
			else
			{
				k++;
				p[k] = 0;
			}
		else k--;
	}

	if (!ok)
		printf("Nu exista solutii\n");

	_getch();	// waits any key

	/* eliberare memorie alocata */
	for (i = 1; i <= n; i++)
		free(maze[i]);
	free(maze);
	return 0;
}
