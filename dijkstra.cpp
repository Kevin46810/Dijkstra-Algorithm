#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <climits>
#include <algorithm>
#include <iomanip>

using namespace std;

#include "queue.h"
#include "graph.h"

struct DijkstraTable {

	string vertex;
	bool mark;
	int weight;
	string prev;
};

void setVertices(string fileName, Graph<string> &graph, vector<DijkstraTable> &table);
void enterStart(vector<DijkstraTable> &table, string &start, vector<string> &unqVertices);
void setEdges(string fileName, Graph<string> &graph, vector<DijkstraTable> &table, bool &error);
void dijkstra (Graph<string> &graph, string &start, vector<DijkstraTable> &table, Queue<string> &vertices, vector<string> &vertexList, bool &error);
int numVertices(vector<DijkstraTable> &table);
vector<string> listOfVertices (vector<DijkstraTable> &table, vector<string> &list);
int findIndex (string &vertex, string &prev, vector<DijkstraTable> &table);
void finishIt (vector<DijkstraTable> &table, map<string, bool> &marked, vector<string> &vertexList, int &i);
void buildGraph (string fileName, Graph<string> &graph, vector<DijkstraTable> &table, bool &error);
bool isCycle (Graph<string> &graph, vector<string> &cycleVs, string &start, string &end, map<string, bool> &marked);
bool containsNonNumber(string &s);
void fetchTableData (Graph<string> &graph, vector<DijkstraTable> &table, Queue<string> &vertices, vector<string> &vertexList, string &start);
void checkCycle(Graph<string> &graph, vector<DijkstraTable> &table, Queue<string> &vertices, vector<string> &vertexList);
void startAlgorithm(string fileName);

const int WIDTH = 30;

int main(int argLength, const char* args[]) {


	if (argLength != 2) {
		cout << "Invalid args." << endl;
		return 0;
	}

	ifstream sourceFile(args[1]);

	if (!sourceFile) {
		cout << "File not found." << endl;
		return 0;
	}

	string fileName = args[1];

	startAlgorithm(fileName);

}

void startAlgorithm (string fileName) {

	Graph<string> graph;
	vector<DijkstraTable> table;

	Queue<string> vertices(50);
	bool error = false;

	buildGraph(fileName, graph, table, error);

	if (error)
		return;

	string start;
	vector<string> vertexList;
	enterStart(table, start, vertexList);

	dijkstra(graph, start, table, vertices, vertexList, error);
}

void buildGraph (string fileName, Graph<string> &graph, vector<DijkstraTable> &table, bool &error) {

	setVertices(fileName, graph, table);
	setEdges(fileName, graph, table, error);
}

void setVertices(string fileName, Graph<string> &graph, vector<DijkstraTable> &table) {

	ifstream sourceFile(fileName);
	string line;

	map<string, bool> duplicate;
	vector<string> vertexCollection;

	//Process the input file lines
	for (int i = 0; getline(sourceFile, line); i++) {
		int found = 0;
		int count = 0;
		int temp = -1;
		string s;
		table.push_back({"", false, INT_MAX, ""});
		while (count < 2) {
			found = line.find(';', temp+1);
			s = line.substr(temp+1, found-(temp+1));
			if (count == 0)
				table[i].prev = s;

			else
				table[i].vertex = s;
			temp = found;
			count++;
			duplicate[s] = false;
			vertexCollection.push_back(s);
		}
	}

	//Add each unique vertex to the graph
	for (int i = 0; i < vertexCollection.size(); i++) {
		if (!duplicate[vertexCollection[i]]) {
			duplicate[vertexCollection[i]] = true;
			graph.AddVertex(vertexCollection[i]);
		}
	}

}

void enterStart(vector<DijkstraTable> &table, string &start, vector<string> &unqVertices) {

	cout << "Here are your vertices: \n" << endl;

	unqVertices = listOfVertices(table, unqVertices);

	for (int i = 0; i < unqVertices.size(); i++)
		cout << unqVertices[i] << endl;

	bool perfectMatch = false;

	//Only accepts vertices in the graph to avoid errors
	while (!perfectMatch) {
		cout << "Please select a starting vertex: ";
		getline(cin, start);
		cout << endl;

		for (int i = 0; i < unqVertices.size(); i++)
			if (start == unqVertices[i])
				perfectMatch = true;

		if (!perfectMatch) {
			cout << "Invalid vertex." << endl;
			start = "";
			cout << "Here are your vertices:\n" << endl;
			for (int i = 0; i < unqVertices.size(); i++) cout << unqVertices[i] << endl;
		}

	}
	cout << endl;
}

void setEdges(string fileName, Graph<string> &graph, vector<DijkstraTable> &table, bool &error) {

	ifstream sourceFile(fileName);

	string line;

	//Only process the weights at the end of each line
	for (int i = 0; getline(sourceFile, line); i++) {
		int temp = -1;
		int count = 0;
		int found = -1;
		int weight = 0;
		while (count < 3) {
			temp = found;
			found = line.find(';', temp+1);
			string s = line.substr(temp+1, found-(temp+1));
			if (count == 2)
				if (containsNonNumber(s)) { //Weight field must contain all integers
					cout << "Invalid weight. Terminating program." << endl;
					error = true;
				}
				else
					weight = stoi(s);

			count++;
		}
		graph.AddEdge(table[i].prev, table[i].vertex, weight);
	}
}

void dijkstra (Graph<string> &graph, string &start, vector<DijkstraTable> &table, Queue<string> &vertices, vector<string> &vertexList, bool &error) {

	for (int i = 0; i < WIDTH * 4; i++)
		cout << "-";        

	cout << endl;

	cout << setw(WIDTH) << "Vertex" << setw(WIDTH) << "Distance" << setw(WIDTH) << "Previous" << endl;
	cout << endl;

	//Initialize the start vertex
	for (int i = 0; i < table.size(); i++) {
		if (start == table[i].vertex) {
			table[i].mark = true;
			table[i].weight = 0;
			table[i].prev = "N/A";
		}
	}
	cout << setw(WIDTH) << start << setw(WIDTH) << "0" << setw(WIDTH) << "N/A" << endl;

	fetchTableData(graph, table, vertices, vertexList, start);
	checkCycle(graph, table, vertices, vertexList);
}

void fetchTableData (Graph<string> &graph, vector<DijkstraTable> &table, Queue<string> &vertices, vector<string> &vertexList, string &start) {

	int previousMin = 0;
	int currWeight = 0;
	string currVertex = start;
	string newVertex;

	bool notOnPath = false;

	map<string, bool> marked;

	int numOfVertices = numVertices(table);

	//Map important for the notOnPath test case
	for (int i = 0; i < vertexList.size(); i++) {
		string vertex = vertexList[i];
		marked[vertex] = false;
	}
	marked[start] = true;


	for (int i = 1; i < numOfVertices; i++) {
		int minWeight = INT_MAX;
		int minIndex = -1;

		graph.GetToVertices(currVertex, vertices);

		//No adjacent vertices?
		if (vertices.isEmpty()) { 
			notOnPath = true;                
		}
		//Processes all adjacent vertices and assigns appropriate weights to them
		while (!vertices.isEmpty()) {
			string adjVertex = vertices.getFront();

			currWeight += graph.WeightIs(currVertex, adjVertex);

			int index = findIndex(adjVertex, currVertex, table);

			if (table[index].weight > currWeight + previousMin && !table[index].mark) {
				table[index].weight = graph.WeightIs(currVertex, adjVertex) + previousMin;
				table[index].prev = currVertex;
			}
			currWeight -= graph.WeightIs(currVertex, adjVertex);
			vertices.dequeue();
		}

		//If there aren't anymore adjacent vertices, the program winds down on the remaining minWeights and prints the inaccessible vertices
		if (notOnPath) {
			finishIt(table, marked, vertexList, i);
		}
		//Print the min vertex
		else {
			for (int j = 0; j < table.size(); j++) {
				if (table[j].weight < minWeight && !table[j].mark) {
					minWeight = table[j].weight;
					previousMin = minWeight;
					minIndex = j;
				}
			}
			//Check whether the program should've entered this test case
			if (minWeight == INT_MAX || minIndex == -1) {
				notOnPath = true;
			}
			else {
				currVertex = table[minIndex].vertex;
				currWeight += graph.WeightIs(table[minIndex].prev, table[minIndex].vertex);

				for (int j = 0; j < table.size(); j++)
					if (table[j].vertex == table[minIndex].vertex)
						table[j].mark = true;


				marked[table[minIndex].vertex] = true;
				cout << setw(WIDTH) << table[minIndex].vertex << setw(WIDTH) << table[minIndex].weight << setw(WIDTH) << table[minIndex].prev << endl;
			}
		}

	}

	for (int i = 0; i < WIDTH * 4; i++)
		cout << "-";

	cout << endl;
	cout << endl;
}

void checkCycle(Graph<string> &graph, vector<DijkstraTable> &table, Queue<string> &vertices, vector<string> &vertexList) {

	vertices.makeEmpty();
	bool cycle = false;
	map<string, bool> cycleMarked;
	vector<string> cycleVs;

	//Create a new map for the cycle detection
	for (int i = 0; i < vertexList.size(); i++)
		cycleMarked[vertexList[i]] = false;

	//Look through every single vertex-to-vertex combination until a cycle is found
	for (int i = 0; i < table.size(); i++) {
		if (cycle)
			break;
		for (int j = 0; j < table.size(); j++) {
			if (j == i)
				j++;
			if (isCycle(graph, cycleVs, table[i].vertex, table[j].vertex, cycleMarked)) {
				cycle = true;
				break;
			}
			else
				cycleVs.clear();

			//Reset map after each cycle search
			for (int k = 0; k < vertexList.size(); k++)
				cycleMarked[vertexList[k]] = false;
		}
	}

	if (cycle) {
		cout << "Cycle detected." << endl;
	}

	else
		cout << "No cycle." << endl;
}

int numVertices (vector<DijkstraTable> &table) {

	map<string, bool> duplicate;

	for (int i = 0; i < table.size(); i++) {
		duplicate[table[i].vertex] = false;
		duplicate[table[i].prev] = false;
	}
	int count = 0;

	for (int i = 0; i < table.size(); i++) {
		if (!duplicate[table[i].vertex]) {
			duplicate[table[i].vertex] = true;
			count++;
		}
		if (!duplicate[table[i].prev]) {
			duplicate[table[i].prev] = true;
			count++;
		}

	}
	return count;
}

vector<string> listOfVertices (vector<DijkstraTable> &table, vector<string> &list) {

	map<string, bool> duplicate;

	for (int i = 0; i < table.size(); i++) {
		duplicate[table[i].vertex] = false;
		duplicate[table[i].prev] = false;
	}

	for (int i = 0; i < table.size(); i++) {
		if (!duplicate[table[i].vertex]) {
			duplicate[table[i].vertex] = true;
			list.push_back(table[i].vertex);
		}
		if (!duplicate[table[i].prev]) {
			duplicate[table[i].prev] = true;
			list.push_back(table[i].prev);
		}
	}
	sort (list.begin(), list.end());

	return list;
}

int findIndex (string &vertex, string &prev, vector<DijkstraTable> &table) {

	int index = -1;

	for (int i = 0; i < table.size(); i++) {
		if (table[i].vertex == vertex && table[i].prev == prev) {
			index = i;
			break;
		}
	}
	return index;
}

void finishIt (vector<DijkstraTable> &table, map<string, bool> &marked, vector<string> &vertexList, int &i) {

	bool timeToFinish = false;

	//Loops until all non INT_MAX vertices are marked
	while (!timeToFinish) {
		int minWeight = INT_MAX;
		int minIndex = -1;
		for (int j = 0; j < table.size(); j++) {
			if (table[j].weight < minWeight && !table[j].mark) {
				minWeight = table[j].weight;
				minIndex = j;
			}
		}
		if (minWeight == INT_MAX || minIndex == -1)
			timeToFinish = true;

		else {
			for (int j = 0; j < table.size(); j++)
				if (table[minIndex].vertex == table[j].vertex || table[minIndex].vertex == table[j].prev)
					table[j].mark = true;

			marked[table[minIndex].vertex] = true;

			cout << setw(WIDTH) << table[minIndex].vertex << setw(WIDTH) << table[minIndex].weight << setw(WIDTH) << table[minIndex].prev << endl;
		}
	}

	sort (vertexList.begin(), vertexList.end()); 

	for (int j = 0; j < vertexList.size(); j++) 
		if (!marked[vertexList[j]]) 
			cout << setw(WIDTH) << vertexList[j] << setw(WIDTH) << "NOT ON PATH" << setw(WIDTH) << "N/A" << endl;

	i = numVertices(table); //Sets dijkstra loop to its end condition
}

bool isCycle (Graph<string> &graph, vector<string> &cycleVs, string &start, string &end, map<string, bool> &marked) {

	Queue<string> vertices;

	if (marked[start])
		return true;

	marked[start] = true; //Mark each vertex when visiting it

	if (start == end)
		return false;

	graph.GetToVertices(start, vertices);

	if (vertices.isEmpty()) 
		return false;

	//Depth first search algorithm: don't leave the route until it's fully explored
	while (!vertices.isEmpty()) {
		string newVertex = vertices.getFront();

		if (isCycle(graph, cycleVs, newVertex, end, marked)) {
			return true;
		}
		vertices.dequeue();
	}
	return false;
}

bool containsNonNumber (string &s) {

	for (int i = 0; i < s.length(); i++)
		if (!isdigit(s[i]))
			return true;
	return false;
}
