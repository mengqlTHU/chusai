#include <iostream>
#include <sstream>
#include "CircuitFinder.h"
#include <chrono>

using namespace std;
using namespace chrono;


int main()
{
    auto start = system_clock::now();
    CircuitFinder cf;
    cf.loadTestData("../data/test_data.txt");
    cf.run();
    auto duration = duration_cast<microseconds>(system_clock::now() - start);
    cout <<  "Cost:"
         << double(duration.count()) * microseconds::period::num / microseconds::period::den
         << "Seconds" << endl;
    return 0;
}
