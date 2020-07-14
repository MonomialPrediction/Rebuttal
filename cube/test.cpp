#include<iostream>
#include<cstdlib>
using namespace std;

int main( int argc, char * argv[] )
{
    if ( argc != 3 )
    {
        cout << "Usage: ./trivium round cube_index" << endl;
        cout << endl;
        cout << "Valid Parameter: " << endl
             << "round = 840" << endl
             << "cube_index = 1: [0,1,...,79]/{70, 72, 74, 76, 78}" << endl
             << "cube_index = 2: [0,1,...,79]/{72, 74, 76, 78}" << endl
             << "cube_index = 3: [0,1,...,79]/{70, 74, 76, 78}" << endl
             << endl
             << "round = 841" << endl
             << "Cube_index = 1: [0,1,...,79]/{70, 72, 76, 78}" << endl
             << "Cube_index = 2: [0,1,...,79]/{72, 76, 78}" << endl
             << endl
             << "round = 842" << endl
             << "Cube_index = 1: [0,1,...,79]/{72, 74, 76, 78}" << endl
             << "Cube_index = 2: [0,1,...,79]/{74, 76, 78}" << endl;
        return -1;
    }
    int round = atoi( argv[1] );
    int index = atoi( argv[2] );
}
