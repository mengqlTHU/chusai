#include <iostream>
#include <sstream>
//#include "graphmatrix/graphmatrix.h"
#include "CircuitFinder.h"

using namespace std;


int main()
{
    CircuitFinder cf;
    cf.loadTestData("../data/test_data.txt");
    cf.run();
    cout << "Hello World!" << endl;
    return 0;
}
