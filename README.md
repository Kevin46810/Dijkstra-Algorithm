Problem:
Sort a graph in ascending order by each vertex’s minimum weight from a given starting point.  This will give a summary table and allow the user to analyze the graph more clearly.

Specification Functions:
 Graph();          
 Graph(int maxV);  
~Graph();             
void AddVertex(VertexType); //Adds vertices to the array
void AddEdge(VertexType, VertexType, int); //Creates edges by adding weights between vertices on an adjacency matrix
int WeightIs(VertexType, VertexType) const; //Tells the user the weight between two vertices
void GetToVertices(VertexType, Queue<VertexType>&) const; //Stores the adjacent vertices/vertex from a current vertex
    
explicit Queue( int capacity = 10 );
bool isEmpty( ) const;
const Object & getFront( ) const;
void makeEmpty( );
Object dequeue( );
void enqueue( const Object & x );


Client Algorithm:
The client will build the graph by processing each input file and using the given implementation.  Then, the client will traverse the graph from a given starting point increment each adjacent vertex’s weight by its edge’s weight and the last marked vertex’s weight.  A currWeight variable will be used to increment the weight as we keep traversing more vertices.  Finally, after the summary table has printed, we check to see if there are any cycles by using a Depth-First search algorithm.
