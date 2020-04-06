#ifndef CIRCUITFINDER_H
#define CIRCUITFINDER_H

#include <algorithm>
#include <iostream>
#include <list>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>

using namespace std;
using namespace chrono;

typedef std::list<int> NodeList;

class CircuitFinder
{
  std::vector<NodeList> AK;
  std::vector<int> Stack;
  std::vector<bool> Blocked;
  std::vector<NodeList> B;
  map<int, int> m;
  vector<int> nodes;
  vector<vector<vector<int>>> resVect;
  int N;
  int circuitCount;
  int S;
  time_point<system_clock, nanoseconds> start;

  void unblock(int U);
  bool circuit(int V);
  void output();
  int findMin();
  static bool compareVector(vector<int> v1, vector<int> v2);
  void outputTime(string info);
  void sortVector();
  void printVector(string filename);
  void printMap();

public:
  CircuitFinder()
      :resVect(5) //3,4,5,6,7
  {
      N=0;
      circuitCount=0;
      start = system_clock::now();
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
        if (!m.count(accountOut))
        {
            m[accountOut] = vertexIndex++;
            nodes.push_back(accountOut);
            AK.push_back(NodeList());
            Blocked.push_back(false);
            B.push_back(NodeList());
        }

        if (!m.count(accountIn))
        {
            m[accountIn] = vertexIndex++;
            nodes.push_back(accountIn);
            AK.push_back(NodeList());
            Blocked.push_back(false);
            B.push_back(NodeList());
        }

        AK[m[accountOut] - 1].push_back(m[accountIn]);
    }
    N = vertexIndex - 1;
#ifdef mydebug
    outputTime("Load Data");
    printMap();
#endif
}


bool CircuitFinder::circuit(int V)
{
  bool F = false;

  Stack.push_back(V);
  Blocked[V - 1] = true;

  int circuitLen = Stack.end() - Stack.begin();
  if (circuitLen < 8)
  {
      for (int W : AK[V - 1]) {
          if (W == S) {
              output();
              F = true;
          }
          else if (W > S && !Blocked[W - 1]) {
              if (circuit(W))
                  F=true;
          }
      }
  }
  else
      F = true;

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


bool CircuitFinder::compareVector(vector<int> v1, vector<int> v2)
{
    for (int i=0;i<v1.size();i++)
    {
        if (v1[i]!=v2[i])
            return v1[i]<v2[i];
    }
    return true;
}

void CircuitFinder::outputTime(string info)
{
    auto duration = duration_cast<microseconds>(system_clock::now() - start);
    cout <<  info
         << double(duration.count()) * microseconds::period::num / microseconds::period::den
         << "Seconds" << endl;
}

void CircuitFinder::output()
{
  int circuitLen = Stack.end() - Stack.begin();
  if (circuitLen<8 && circuitLen>2)
  {
      resVect[circuitLen-3].push_back(vector<int>());
      int idOfMin = findMin();
//      std::cout << "circuit: ";
      for (int i=idOfMin;i<circuitLen+idOfMin;i++)
      {
          auto I = Stack.begin()+(i % circuitLen);
//          std::cout << nodes[*I-1] << " -> ";
          resVect[circuitLen-3].back().push_back(nodes[*I-1]);
      }
//      std::cout << nodes[*(Stack.begin()+idOfMin)-1] << std::endl;
      circuitCount += 1;
  }
}

void CircuitFinder::sortVector()
{
    for (int i=0;i<5;i++)
    {
        if (resVect[i].size()>0)
        {
            sort(resVect[i].begin(),resVect[i].end(), compareVector);
        }
    }
}

void CircuitFinder::printVector(string filename)
{
    ofstream fout(filename);
    fout << circuitCount << endl;
    for (int i=0;i<5;i++)
    {
        for (int j=0;j<resVect[i].size();j++)
        {
            fout << resVect[i][j][0];
            for (int k=1;k<i+3;k++)
                fout << "," << resVect[i][j][k];
            fout << endl;
        }
    }
    fout.close();
}

void CircuitFinder::printMap()
{
    ofstream fout("../data/map.txt");
    for (int i = 0; i < N; i++)
        fout << nodes[i] << endl;
    fout.close();
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

#ifdef mydebug
    outputTime("A S cycle");
    cout << S << endl;
#endif
  }

#ifdef mydebug
  printVector("../data/myresult_unsorted.txt");
#endif

  sortVector();
  printVector("../data/myresult.txt");
  outputTime("Finished");
}

#endif // CIRCUITFINDER_H
