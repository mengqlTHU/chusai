#ifndef CIRCUITFINDER_H
#define CIRCUITFINDER_H

#include <algorithm>
#include <iostream>
#include <list>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

using namespace std;
typedef std::list<int> NodeList;

class CircuitFinder
{
  std::vector<NodeList> AK;
  std::vector<int> Stack;
  std::vector<bool> Blocked;
  std::vector<NodeList> B;
  map<int, int> m;
  vector<int> nodes;
  int N;
  int circuitCount;
  int S;

  void unblock(int U);
  bool circuit(int V);
  void output();
  int findMin();

public:
  CircuitFinder(){
      N=0;
      circuitCount=0;
  }

  void run();
  void loadTestData(string filename);
};

void CircuitFinder::unblock(int U)
{
  Blocked[U - 1] = false;

  while (!B[U - 1].empty()) {
    int W = B[U - 1].front();
    B[U - 1].pop_front();

    if (Blocked[W - 1]) {
      unblock(W);
    }
  }
}

void CircuitFinder::loadTestData(string filename)
{
    ifstream indata;
    indata.open(filename);
    string line;
    int accountOut, accountIn;
    int vertexIndex = 1;
    while (getline(indata, line)) {
        stringstream lineStream(line);
        string cell;
        getline(lineStream, cell, ',');
        accountOut = std::stod(cell);
        getline(lineStream, cell, ',');
        accountIn = std::stod(cell);
        if (!m.count(accountIn))
        {
           m[accountIn] = vertexIndex++;
           nodes.push_back(accountIn);
           AK.push_back(*(new NodeList()));
           Blocked.push_back(false);
           B.push_back(*(new NodeList()));
        }
        if (!m.count(accountOut))
        {
           m[accountOut] = vertexIndex++;
           nodes.push_back(accountOut);
           AK.push_back(*(new NodeList()));
           Blocked.push_back(false);
           B.push_back(*(new NodeList()));
        }

        AK[m[accountOut] - 1].push_back(m[accountIn]);
    }
    N = vertexIndex - 1;
}


bool CircuitFinder::circuit(int V)
{
  bool F = false;
  Stack.push_back(V);
  Blocked[V - 1] = true;

  for (int W : AK[V - 1]) {
    if (W == S) {
      output();
      F = true;
    } else if (W > S && !Blocked[W - 1]) {
      F = circuit(W);
    }
  }

  if (F) {
    unblock(V);
  } else {
    for (int W : AK[V - 1]) {
      auto IT = std::find(B[W - 1].begin(), B[W - 1].end(), V);
      if (IT == B[W - 1].end()) {
        B[W - 1].push_back(V);
      }
    }
  }

  Stack.pop_back();
  return F;
}

int CircuitFinder::findMin()
{
    int min = nodes[*Stack.begin()-1];
    int idOfMin = 0;
    int id = 0;
    for (auto I = Stack.begin() + 1, E = Stack.end(); I != E; ++I)
    {
        id++;
        if (nodes[*I-1]<min)
        {
            min = nodes[*I-1];
            idOfMin = id;
        }
    }
    return idOfMin;
}

void CircuitFinder::output()
{
  int circuitLen = Stack.end() - Stack.begin();
  if (circuitLen<8 && circuitLen>2)
  {
      int idOfMin = findMin();
      std::cout << "circuit: ";
      for (int i=idOfMin;i<circuitLen+idOfMin;i++)
      {
          auto I = Stack.begin()+(i % circuitLen);
          std::cout << nodes[*I-1] << " -> ";
      }
      std::cout << nodes[*(Stack.begin()+idOfMin)-1] << std::endl;
      circuitCount += 1;
  }
}

void CircuitFinder::run()
{
  Stack.clear();
  S = 1;

  while (S < N) {
    for (int I = S; I <= N; ++I) {
      Blocked[I-1] = false;
      B[I-1].clear();
    }
    circuit(S);
    ++S;
  }
  cout << circuitCount << endl;
}

#endif // CIRCUITFINDER_H
