#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <climits>
#include <limits>
#include <map>

using namespace std;


// function prototypes
double** Read_File(string, map<int,string>&, int&);
int** Prim(double**, int);
void Write_File(int**, map<int, string>, int, string);


int main(int argc, char **argv)
{
	double **adj_matrix;
	int **mst; 
	int num_vert = 0;
	map<int, string> vertices;

	string in_file;
	string out_file;

	if(argc < 3)
	{
		cout << "Error! Usage: prim <input file_name> <output file_name>" << endl;
		exit(1);
	}
	else
	{
		in_file = argv[1];
		out_file = argv[2];
	}

	adj_matrix = Read_File(in_file, vertices, num_vert);

	mst = Prim(adj_matrix, num_vert);

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
	double w = 0.0;

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


// Implementation of Prim's algorithm for finding mst
// returns 2D array of ints 0 if edge doesn't exist in MST
// or 1 if edges is in the MST
int** Prim(double **adj_matrix, int num_vert)
{
	int selected[num_vert];
	int num_edges = 0;
	int min = INT_MAX;

	int x = 0;
	int y = 0;

	// init MST
	int **mst = new int*[num_vert];

	for(int i = 0; i < num_vert; i++)
	{
		selected[i] = 0;
		mst[i] = new int[num_vert];
	}

	for(int i = 0; i < num_vert; i++)
	{
		for(int j = 0; j < num_vert; j++)
		{
			mst[i][j] = 0;
		}
	}

	selected[0] = 1;

	while(num_edges < num_vert-1)
	{
		min = INT_MAX;

		for(int i= 0; i < num_vert; i++)
		{
			if(selected[i] == 1)
			{
				for(int j = 0; j < num_vert; j++)
				{
					if(selected[j] == 0)
					{
						if(adj_matrix[i][j] < min)
						{
							min = adj_matrix[i][j];
							x = i;
							y = j;
						}
					}
				}
			}
		}
		// undirected graph has to account for both directions
		mst[x][y] = 1;
		mst[y][x] = 1;
		selected[y] = 1;
		num_edges++;
	}

	return mst;
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