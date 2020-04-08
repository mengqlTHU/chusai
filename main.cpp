//#pragma comment(linker, "/STACK:1073741824") //Require 1GB Stack RAM

#include <iostream>
#include <sstream>
#include "CircuitFinder.h"

using namespace std;


int main()
{
    CircuitFinder cf;
    cf.loadTestData("../data/test_data.txt");
    cf.run();
    return 0;
}
