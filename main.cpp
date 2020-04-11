#ifndef CIRCUITFINDER_H
#define CIRCUITFINDER_H

#pragma comment(linker, "/STACK:1073741824") //Require 1GB Stack RAM

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS //hash_map will be removed on windows in the future

#include <algorithm>
#include <iostream>
#include <list>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <hash_map>
#include <set>
#include <chrono>

using namespace std;
using namespace chrono;

#define TEST

// #define TIMERS

#ifdef MYTIME
#include <sys/time.h>
#endif

#ifdef TIMERS
#include "Timers.hpp"
#endif

class CircuitFinder
{
  vector<vector<int>> AK;
  vector<vector<int>> subAK;
  vector<int> Stack;
  std::vector<bool> Blocked;
  std::vector<bool> falseBlocked;
  std::vector<vector<int>> B;
//   map<int, int> m;
  vector<int> nodes;
  vector<vector<vector<int>>> resVect;
  int N;
  int circuitCount;
  int S;
  time_point<system_clock, nanoseconds> start;

  void unblock(int U);
  bool circuit(int V);
  //bool circuitSubGraph(int V);
  //vector<NodeList> getSubGraph(set<int> s);

  //void circuitIterate(int V);
  void output();
  int findMin();
  static bool compareVector(vector<int> v1, vector<int> v2);
  void outputTime(string info);
  void sortVector();
  void printVector(string filename);
  void printMap();
  void strongComponent();
  void runInSubGraph(set<int> s);

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


//Tarjan Algorithm to find strong component
void CircuitFinder::strongComponent()
{
    map<int, int> preOrder;
    map<int, int> lowLink;
    set<int> sccFound;
    vector<set<int>> sccVec;
    vector<int> sccMin;
    vector<int> sccQueue;
    int v;
    bool done;
    int i = 0;
    for (int node = S; node < N; node++)
    {
        vector<int> q;
        if (sccFound.find(node)==sccFound.end())
            q.push_back(node);

        while (!q.empty())
        {
            v = q.back();
            if (preOrder.find(v)==preOrder.end())
            {
                i++;
                preOrder[v] = i;
            }
            done = true;
            for (int w : AK[v])
            {
                if (w >= S)
                {
                    if (preOrder.find(w) == preOrder.end())
                    {
                        q.push_back(w);
                        done = false;
                        break;
                    }
                }
            }
            if (done)
            {
                lowLink[v] = preOrder[v];
                for (int w : AK[v])
                {
                    if (w >= S)
                    {
                        if (sccFound.find(w) == sccFound.end())
                        {
                            if (preOrder[w] > preOrder[v])
                                lowLink[v] = lowLink[v] < lowLink[w] ? lowLink[v] : lowLink[w];
                            else
                                lowLink[v] = lowLink[v] < preOrder[w] ? lowLink[v] : preOrder[w];
                        }
                    }
                }
                q.pop_back();
                if (lowLink[v] == preOrder[v])
                {
                    set<int> scc;
                    scc.insert(v);
                    while (!sccQueue.empty() && preOrder[sccQueue.back()] > preOrder[v])
                    {
                        int k = sccQueue.back();
                        sccQueue.pop_back();
                        scc.insert(k);
                    }

                    //vector<NodeList> subAK = getSubGraph(scc);
                   

                    sccVec.push_back(scc);
                    sccFound.insert(scc.begin(), scc.end());
                }
                else
                    sccQueue.push_back(v);
            }
        }
    }

    set<int> resScc = *min_element(sccVec.begin(), sccVec.end(), [](set<int> a, set<int> b) {return *a.begin() < *b.begin(); });

    if (resScc.size())
    {
        subAK = AK;

        for (int W : resScc)
        {
            for (vector<int>::iterator iter = subAK[W].begin(); iter != subAK[W].end(); )
            {
                if (resScc.find(*iter) == resScc.end())
                    iter = subAK[W].erase(iter); // advances iter
                else
                    ++iter; // don't remove
            }
        }
        for (int W : resScc)
        {
            Blocked[W] = false;
            B[W].clear();
        }
        //runInSubGraph(*resScc.begin());
        circuit(*resScc.begin());
    }

}


void CircuitFinder::unblock(int U)
{
  Blocked[U] = false;

  while (!B[U].empty()) {
    int W = B[U].back();
    B[U].pop_back();

    if (Blocked[W]) {
      unblock(W);
    }
  }
}

void CircuitFinder::loadTestData(string filename)
{
#ifdef _WIN64
    map<int, int> intHash;
#else
    __gnu_cxx::hash_map<int,int> intHash(20000);
#endif
    ifstream indata;
    indata.open(filename);
    string line;
    int vertexIndex = 0;
    int AK_ptr[280000];
    while (getline(indata, line)) {
        char* s = &line[0];
        int tmp=0;
        int accountOut=0, accountIn=0;
        while(*s!=',')
        {
            accountOut = (*s++-'0')+accountOut*10;
        }
        s++;
        while(*s!=',')
        {
            accountIn = (*s++-'0')+accountIn*10;
        }
        if(intHash.find(accountOut)==intHash.end())
        // if (!m.count(accountOut))
        {
            intHash[accountOut] = vertexIndex++;
            // m[accountOut] = vertexIndex++;
            nodes.push_back(accountOut);
            AK.push_back(vector<int>());
            Blocked.push_back(false);
            B.push_back(vector<int>());
        }

        if(intHash.find(accountIn)==intHash.end())
        // if (!m.count(accountIn)) // 1700us
        {
            // m[accountIn] = vertexIndex++;
            intHash[accountIn] = vertexIndex++;
            nodes.push_back(accountIn);
            AK.push_back(vector<int>());
            Blocked.push_back(false);
            B.push_back(vector<int>());
        }

        AK[intHash[accountOut]].push_back(intHash[accountIn]); // 400us
    }
    N = vertexIndex;
    falseBlocked = Blocked;
#ifdef mydebug
    outputTime("Load Data");
    printMap();
#endif
}

// void CircuitFinder::circuitIterate(int V)
// {
//     Stack.push_back(V);
//     Blocked[V - 1] = true;
//     vector<int> s;
//     s.push_back(V);
//     vector<NodeList> sNode;
//     sNode.push_back(AK[V - 1]);
//     set<int> closed;
//     int thisNode, nextNode;
//     NodeList nbrs;
//     while (!s.empty())
//     {
//         thisNode = s.back();
//         nbrs = sNode.back();
//         if (!nbrs.empty() && Stack.size()<8)
//         {
//             nextNode = nbrs.back();
//             sNode.back().pop_back();
//             if (nextNode < V)
//                 continue;
//             if (nextNode == V)
//             {
//                 output();
//                 closed.insert(Stack.begin(), Stack.end());
//             }
//             else if (!Blocked[nextNode - 1])
//             {
//                 Stack.push_back(nextNode);
//                 s.push_back(nextNode);
//                 sNode.push_back(AK[nextNode - 1]);
//                 closed.erase(nextNode);
//                 Blocked[nextNode - 1] = true;
//                 continue;
//             }
//         }
//         if (nbrs.empty() || Stack.size()>7)
//         {
//             if (closed.count(thisNode))
//             {
//                 unblock(thisNode);
//             }
//             else
//             {
//                 for (int W : AK[thisNode - 1])
//                 {
//                     auto IT = std::find(B[W - 1].begin(), B[W - 1].end(), thisNode);
//                     if (IT == B[W - 1].end())
//                         B[W - 1].push_back(thisNode);
//                 }
//             }
//             s.pop_back();
//             sNode.pop_back();
//             Stack.pop_back();
//         }
//     }
// }

bool CircuitFinder::circuit(int V)
{
    bool F = false;

    Stack.push_back(V);
    Blocked[V] = true;

    auto circuitLen = Stack.size();
    if (circuitLen < 7)
    {
        for (int W : subAK[V]) {
            if (W > S && !Blocked[W])
                F = circuit(W) || F;
            else if (W == S) {
                output();
                F = true;
            }
        }
    }
    else if (circuitLen == 7)
    {
        for (int W : subAK[V]) {
            if (W == S) {
                output();
                break;
            }
        }
        F = true;
    }
    else
        F = true;

    if (F) {
        unblock(V);
    }
    else {
        for (int W : subAK[V]) {
            auto IT = std::find(B[W].begin(), B[W].end(), V);
            if (IT == B[W].end()) {
                B[W].push_back(V);
            }
        }
    }

    Stack.pop_back();
    return F;
}

int CircuitFinder::findMin()
{
    int min = nodes[*Stack.begin()];
    int idOfMin = 0;
    int id = 0;
    for (auto I = Stack.begin() + 1, E = Stack.end(); I != E; ++I)
    {
        id++;
        if (nodes[*I]<min)
        {
            min = nodes[*I];
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
  auto circuitLen = Stack.size();
  if (circuitLen>2)
  {
      resVect[circuitLen-3].push_back(vector<int>());
      int idOfMin = findMin();
      for (int i=idOfMin;i<circuitLen+idOfMin;i++)
      {
          auto I = Stack.begin()+(i % circuitLen);
          resVect[circuitLen-3].back().push_back(nodes[*I]);
      }
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

void CircuitFinder::runInSubGraph(set<int> s)
{
    for (set<int>::iterator iter=s.begin(); iter!=s.end();iter++)
    {
        S = *iter;
        Blocked = falseBlocked;
        for (set<int>::iterator inner_iter = iter; inner_iter != s.end(); inner_iter++) {
            B[*(inner_iter)].clear();
        }
        circuit(S);

#ifdef mydebug
    outputTime("A S cycle");
    cout << S << endl;
#endif

    }
}

void CircuitFinder::run()
{
  Stack.clear();
  S = 0;
#ifdef MYTIME
    struct timeval ov_start, ov_end;
    gettimeofday(&ov_start,NULL);
#endif
while (S < N) {
      //Blocked = falseBlocked;
     //circuitIterate(S);
    strongComponent();
     ++S;
 #ifdef mydebug
     outputTime("A S cycle");
     cout << S << endl;
 #endif
 }

//strongComponent();
#ifdef MYTIME
    gettimeofday(&ov_end,NULL);
    double timeuse
        = 1000000*(ov_end.tv_sec-ov_start.tv_sec)
        + ov_end.tv_usec-ov_start.tv_usec;
    cout<<"run_circuit: "<<timeuse<<" us"<<endl;
#endif

#ifdef mydebug
  printVector("../data/myresult_unsorted.txt");
#endif

#ifdef MYTIME
    gettimeofday(&ov_start,NULL);
#endif
  sortVector();
#ifdef MYTIME
    gettimeofday(&ov_end,NULL);
    timeuse
        = 1000000*(ov_end.tv_sec-ov_start.tv_sec)
        + ov_end.tv_usec-ov_start.tv_usec;
    cout<<"sortVector: "<<timeuse<<" us"<<endl;
#endif
#ifdef MYTIME
    gettimeofday(&ov_start,NULL);
#endif

#ifdef _WIN64
    printVector("../data/myresult.txt");
#else
    printVector("/projects/student/result.txt");
#endif

#ifdef MYTIME
    gettimeofday(&ov_end,NULL);
    timeuse
        = 1000000*(ov_end.tv_sec-ov_start.tv_sec)
        + ov_end.tv_usec-ov_start.tv_usec;
    cout<<"printVector: "<<timeuse<<" us"<<endl;
#endif
  outputTime("Finished");
}

#endif // CIRCUITFINDER_H


int main()
{
#ifdef TIMERS
    MPI_Init(NULL,NULL);
    Timer:startTimer("overall");
#endif
    CircuitFinder cf;
#ifdef MYTIME
    struct timeval ov_start, ov_end;
    gettimeofday(&ov_start,NULL);
#endif

#ifdef _WIN64
    cf.loadTestData("../data/test_data_small.txt");
#elif def TEST
    cf.loadTestData("/root/data/test_data_small.txt");
#else
    cf.loadTestData("/data/test_data.txt");
#endif

#ifdef MYTIME
    gettimeofday(&ov_end,NULL);
    double timeuse
        = 1000000*(ov_end.tv_sec-ov_start.tv_sec)
        + ov_end.tv_usec-ov_start.tv_usec;
    cout<<"loadTestData: "<<timeuse<<" us"<<endl;
#endif
#ifdef MYTIME
    gettimeofday(&ov_start,NULL);
#endif
    cf.run();
#ifdef MYTIME
    gettimeofday(&ov_end,NULL);
    timeuse
        = 1000000*(ov_end.tv_sec-ov_start.tv_sec)
        + ov_end.tv_usec-ov_start.tv_usec;
    cout<<"run: "<<timeuse<<" us"<<endl;
#endif
    return 0;
}
