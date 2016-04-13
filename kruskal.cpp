#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <climits>
#include <limits>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

// to be able to hold an integer representation of an edge connecting 2 vertices
#define edge pair<int,int>

// this struct is strictly for figuring out all of the edges in the map
// not allowing for duplicate edges to make it into the undirected graph
// any pos that is not used is set to INT_MAX and ignored
struct Holder
{
	int v1;
	int v2;
};


// function prototypes
double** Read_File(string, map<int,string>&, int&);
int** Kruskal(double**, int, vector< pair<double, edge> >);
int Find_Set(int, int[]);
void Write_File(int**, map<int, string>, int, string);
bool check(Holder[], int, int, int);


int main(int argc, char **argv)
{
	double **adj_matrix;
	int **mst; 
	int num_vert = 0;
	map<int, string> vertices;

	// stores pair of weight and the 2 connected vertices
	vector< pair<double, edge> > graph;

	string in_file;
	string out_file;

	if(argc < 3)
	{
		cout << "Error! Usage: kruskal <input file_name> <output file_name>" << endl;
		exit(1);
	}
	else
	{
		in_file = argv[1];
		out_file = argv[2];
	}

	adj_matrix = Read_File(in_file, vertices, num_vert);

	int poss_edges = num_vert * (num_vert - 1);
	poss_edges = poss_edges / 2;

	// assure enough size for all edges
	Holder holds[poss_edges];

	//init
	int k = 0;

	for(int i = 0; i < poss_edges; i++)
	{
		holds[i].v1 = -1;
		holds[i].v2 = -1;
	}

	for(int i = 0; i < num_vert; i++)
	{
		for(int j = 0; j < num_vert; j++)
		{
			// check that the edge isn't already stored
			bool is_there = check(holds, i, j, poss_edges);

			if(adj_matrix[i][j] != numeric_limits<double>::max() && !is_there)
			{
				// push new edge into a vector of a pair containing the
				// weight of the edge and the pair of the 2 vertices
				graph.push_back(pair< double, edge >(adj_matrix[i][j], edge(i, j)));
				holds[k].v1 = i;
				holds[k].v2 = j;

				k++;
			}
			else
			{
				// store INT_MAX, will be ignored later
				graph.push_back(pair< double, edge >(INT_MAX, edge(INT_MAX, INT_MAX)));
			}	
		}
	}

	mst = Kruskal(adj_matrix, num_vert, graph);

	Write_File(mst, vertices, num_vert, out_file);

	// clean up
	for(int i = 0; i < num_vert; i++)
	{
    	delete[] adj_matrix[i];
    	delete[] mst[i];
	}

	delete[] adj_matrix;
	delete[] mst; 

    return 0;
}


// checks to see if there is already a pair of vertices going either direction
// if true, don't add edge
// if false, add edge to the vector
bool check(Holder holds[], int i, int j, int num_edges)
{
	for(int x = 0; x < num_edges; x++)
	{
		if(holds[x].v1 == i)
		{
			if(holds[x].v2 == j)
			{
				return true;
			}
		}

		if(holds[x].v1 == j)
		{
			if(holds[x].v2 == i)
			{
				return true;
			}
		}
	}
	return false;

}

// read input file
// then populate the adj_matrix for calculating mst
double** Read_File(string file, map<int,string> &vertices, int &num_vert)
{
	ifstream f;
	string s;

	string v;

	int src = 0;
	int x = 0;
	int y = 0;
	int count = 0;

	// for filling the adj_matrix
	map<string, int> holder;
	
	string temp;
	double w;

	f.open(file.c_str());

	// get num of vertices
	getline(f, s, '\n');
	num_vert = atoi(s.c_str());


	string all_edges[num_vert];
	
	// init adj_matrix set to max double
	double **adj_matrix = new double*[num_vert];

	for(int i = 0; i < num_vert; i++)
		adj_matrix[i] = new double[num_vert];

	for(int i = 0; i < num_vert; i++)
	{
		for(int j = 0; j < num_vert; j++)
		{
			adj_matrix[i][j] = numeric_limits<double>::max();
		}
	}


	// store rest of lines
	while(getline(f, all_edges[count], '\n'))
	{
		count++;
	}

	f.close();

	// get all of the vertices
	for(int y = 0; y < num_vert; y++)
	{
		s = all_edges[y];

		v = s.substr(0, s.find(' '));

		vertices[src] = v;

		holder[v] = src;

		src++;
	}

	// get connecting edges and adj_vertices
	for(int itr = 0; itr < num_vert; itr++)
	{
		temp = all_edges[itr];

		temp = temp.substr(temp.find(' ')+1, temp.length());

		// grab all branching vertices from the parent
		while(temp.find(' ') != -1)
		{
			char *next_v = new char[temp.length()];
			sscanf(temp.c_str(), "%s %lf", next_v, &w);

			string dest(next_v);

			x = holder.find(vertices.find(itr)->second)->second;
			y = holder.find(dest)->second;

			adj_matrix[x][y] = w;

			temp = temp.substr(temp.find(' ')+1, temp.length());
			temp = temp.substr(temp.find(' ')+1, temp.length());

			delete []next_v;
		}
	}

	return adj_matrix;
}


// Implementation of Kruskal's algorithm for finding mst
// returns 2D array of ints 0 if edge doesn't exist in MST
// or 1 if edges is in the MST
int** Kruskal(double **adj_matrix, int num_vert, vector< pair<double, edge> > graph)
{
	int parent[num_vert];
	int num_edges = 0;
	int min = INT_MAX;

	int x = 0;
	int y = 0;
	int a;
	int b;

	// init MST
	int **mst = new int*[num_vert];

	for(int i = 0; i < num_vert; i++)
	{
		parent[i] = i;
		mst[i] = new int[num_vert];
	}

	for(int i = 0; i < num_vert; i++)
	{
		for(int j = 0; j < num_vert; j++)
		{
			mst[i][j] = 0;
		}
	}

	// sort the vector of edges by weight
	// weight is the first in pair of weight and edge
	sort(graph.begin(), graph.end());

	int poss_edges = num_vert * (num_vert - 1);
	poss_edges = poss_edges / 2;


	for(int i = 0; i < poss_edges; i++)
	{	
		// once the first INT_MAX is reached, all real edges have been seen
		if(graph[i].second.first == INT_MAX)
			break;

		// find parent of each vertex
		a = Find_Set(graph[i].second.first, parent);
		b = Find_Set(graph[i].second.second, parent);

		// the edge can be added to the MST
		// account for both directions of undirected graph
		if(a != b)
		{
			mst[graph[i].second.first][graph[i].second.second] = 1;
			mst[graph[i].second.second][graph[i].second.first] = 1;
			parent[a] = parent[b];
		}
	}

	return mst;
}


// finds the parent of the specified position (made vertices represented by ints)
int Find_Set(int i, int parent[])
{
	while(i != parent[i])
	{
		i = parent[i];
	}

	return i;
}


// Write results of the MST to the file specified
void Write_File(int **mst, map<int, string> vertices, int num_vert, string file)
{
	ofstream o(file.c_str(), ios::out | ios::trunc);

	o << num_vert << endl;

	for(int i = 0 ; i < num_vert; i++)
	{
		o << vertices.find(i)->second << " ";

		for(int j = 0; j < num_vert; j++)
		{
			if(mst[i][j] == 1)
			{
				o << vertices.find(j)->second << " ";
			}
		}

    	o << endl;	  
	}

	o.close();
}