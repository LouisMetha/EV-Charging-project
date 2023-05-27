/*
 * WeightedGraph.h
 *
 *  Created on: 29 Apr 2023
 *      Author: 2002345
 */

#ifndef H_weightedGraph
#define H_weightedGraph

using namespace std;

class WeightedGraphType {
protected:
	int gSize;      //number of vertices
	list<int> *graph; // Store adjacency list
	double **weights; // Store weights of edges
    double *smallestWeight;

public:
	WeightedGraphType(int size = 0);
	~WeightedGraphType();

	list<int> getAdjancencyList(int index) {
		return graph[index];
	}

	double getWeight(int i, int j) {
		return weights[i][j];
	}
	
	int getSize() {
		return gSize;
	}

	void printAdjacencyList();
	void printAdjacencyMatrix();
	list<int> breadthFirstTraversal();
	double* shortestPath(int vertex);
	stack<int> shortestPath(int vertex,int dest);
	void printShortestDistance(int vertex);

};

WeightedGraphType::WeightedGraphType(int size) {
	gSize = 0;
	ifstream infile;
	char fileName[50] = "Weights.txt";

//	cout << "Enter graph adjacency matrix file name: ";
//	cin >> fileName;
//	cout << endl;

	infile.open(fileName);

	if (!infile) {
		cout << "Cannot open input file." << endl;
		return;
	}

	gSize = size;
	graph = new list<int> [gSize];
	weights = new double*[gSize];

	for (int i = 0; i < gSize; i++)
		weights[i] = new double[gSize];

	for (int i = 0; i < gSize; i++) {
		for (int j = 0; j < gSize; j++) {
			double value;
			infile >> value;
			if (value == 0)
				weights[i][j] = DBL_MAX; //system constant - maximum value of double
			else {
				weights[i][j] = value;
				graph[i].push_back(j);
			}
		}
	}
	infile.close();
}

WeightedGraphType::~WeightedGraphType() {
	for (int i = 0; i < gSize; i++) {
		delete[] weights[i];
	}

	for (int index = 0; index < gSize; index++)
		graph[index].clear();

	delete[] graph;
	delete[] weights;
	delete[] smallestWeight;

}

void WeightedGraphType::printAdjacencyMatrix() { //print adjacency matrix for debug purpose
	cout<<"\nAdjacency Matrix" << endl;
	for (int i = 0; i < gSize; i++) {
		for (int j = 0; j < gSize; j++) {
			cout << setw(8) << (weights[i][j] == DBL_MAX ? 0.0 : weights[i][j]); //as adjacency value, zero means no direct connection
		}
		cout << endl;
	}
}

void WeightedGraphType::printAdjacencyList() { //print adjacency list for debug purpose
	cout<<"\nAdjacency List" << endl;
	for (int index = 0; index < gSize; index++) {
		cout << index << ": ";
		for (int e : graph[index])
			cout << e << " ";
		cout << endl;
	}

	cout << endl;
}

list<int> WeightedGraphType::breadthFirstTraversal() {
	
	queue<int> queue;
	list<int> indices;
	bool *visited;
	visited = new bool[gSize];

	for (int ind = 0; ind < gSize; ind++)
		visited[ind] = false;

	list<int>::iterator graphIt;

	for (int index = 0; index < gSize; index++)
		if (!visited[index]) {
			queue.push(index);
			visited[index] = true;
			indices.push_back(index);

			while (!queue.empty()) {
				int u = queue.front();
				queue.pop();

				for (graphIt = graph[u].begin(); graphIt != graph[u].end(); ++graphIt) {
					int w = *graphIt;
					if (!visited[w]) {
						queue.push(w);
						visited[w] = true;
						indices.push_back(w);
					}
				}
			}
		}

	delete[] visited;
	return indices;
}

double* WeightedGraphType::shortestPath(int vertex)
{
    bool *weightFound = new bool[gSize];

	for (int i = 0; i < gSize; i++)
		smallestWeight = new double[gSize];

    for (int j = 0; j < gSize; j++) {
        smallestWeight[j] = weights[vertex][j];
        weightFound[j] = false;
	}

    weightFound[vertex] = true;
    smallestWeight[vertex] = 0;

    for (int i = 0; i < gSize - 1; i++)
    {
        double minWeight = DBL_MAX;
        int v;

        for (int j = 0; j < gSize; j++)
            if (!weightFound[j])
                if (smallestWeight[j] < minWeight)
                {
                    v = j;
                    minWeight = smallestWeight[v];
                }

        weightFound[v] = true;

        for (int j = 0; j < gSize; j++)
            if (!weightFound[j])
                if (minWeight + weights[v][j] < smallestWeight[j]) {
                    smallestWeight[j] = minWeight + weights[v][j];
				}
    }

	delete[] weightFound;
	return smallestWeight;
}

stack<int> WeightedGraphType::shortestPath(int vertex, int dest)
{
    stack<int> path;
    vector<int> checkedVertex(gSize);

    bool *weightFound = new bool[gSize];
    double *smallestWeight = new double[gSize];

    for (int j = 0; j < gSize; j++) {
        smallestWeight[j] = weights[vertex][j];
        weightFound[j] = false;
        checkedVertex[j] = INT_MAX;
    }

    weightFound[vertex] = true;
    smallestWeight[vertex] = 0;

    for (int i = 0; i < gSize - 1; i++)
    {
        double minWeight = DBL_MAX;
        int v = INT_MAX;

        for (int j = 0; j < gSize; j++)
        {
            if (!weightFound[j] && smallestWeight[j] < minWeight)
            {
                v = j;
                minWeight = smallestWeight[j];
            }
        }

        if (v == INT_MAX || v == dest)
            break;

        weightFound[v] = true;

        for (int j = 0; j < gSize; j++)
        {
            if (!weightFound[j] && minWeight + weights[v][j] < smallestWeight[j])
            {
                smallestWeight[j] = minWeight + weights[v][j];
                checkedVertex[j] = v;
            }
        }
    }

    if (checkedVertex[dest] != INT_MAX)
    {
        int currentVertex = dest;
        while (currentVertex != vertex && currentVertex != INT_MAX)
        {	
			if (currentVertex != dest) {
				path.push(currentVertex);
            	currentVertex = checkedVertex[currentVertex];
			} else {
				currentVertex = checkedVertex[currentVertex];
			}
        }
        path.push(vertex);
    }

    delete[] weightFound;
    return path;
}

void WeightedGraphType::printShortestDistance(int vertex)
{	
	
    cout << "Source Vertex: " << vertex << endl;
    cout << "Shortest distance from source to each vertex."
         << endl;
    cout << "Vertex  Shortest_Distance" << endl;

    for (int j = 0; j < gSize; j++) {
        cout << setw(4) << j << setw(12) << smallestWeight[j] << endl;
	}
		
    cout << endl;
}

#endif
