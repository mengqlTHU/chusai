#ifndef CIRCUITFINDER_H
#define CIRCUITFINDER_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
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
  vector<vector<int>> AK;//出邻接表
  vector<int> inDeg;//入度
  //vector<NodeList> subAK;
  vector<set<int>> removedEdge;
  vector<int> Stack;
  vector<int> szwarcStack;
  std::vector<bool> Blocked;
  std::vector<bool> falseBlocked;
  vector<bool> mark;
  vector<bool> reach;
  vector<int> position;
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
  bool cycle(int v, int q);
  void noCycle(int x, int y);
  void unMark(int x);
  //bool circuitSubGraph(int V);
  //vector<NodeList> getSubGraph(set<int> s);

  //void circuitIterate(int V);
  void output();
  void szwarcOutput(int v, int w);
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
      :resVect(5),AK(80000),nodes(80000) //3,4,5,6,7
  {
      N=0;
      circuitCount=0;
      start = system_clock::now();
  }

  void run();
  void loadTestData(string filename);
};

//Tarjan算法寻找所有强连通分量
//https://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm
void CircuitFinder::strongComponent()
{
    unordered_map<int, int> preOrder;
    unordered_map<int, int> lowLink;
    set<int> sccFound;
    vector<int> sccQueue;
    int v;
    bool done;
    int i = 0;
    for (int node = 0; node < N; node++)
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
                if (preOrder.find(w)==preOrder.end())
                {
                    q.push_back(w);
                    done = false;
                    break;
                }
            }
            if (done)
            {
                lowLink[v] = preOrder[v];
                for (int w : AK[v])
                {
                    if (sccFound.find(w)==sccFound.end())
                    {
                        if (preOrder[w] > preOrder[v])
                            lowLink[v] = lowLink[v] < lowLink[w] ? lowLink[v] : lowLink[w];
                        else
                            lowLink[v] = lowLink[v] < preOrder[w] ? lowLink[v] : preOrder[w];
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

                    if (scc.size() > 1)
                    {
                        //for (int W : scc)
                        //{
                        //    for (vector<int>::iterator iter = AK[W].begin(); iter != AK[W].end(); )
                        //    {
                        //        if (scc.find(*iter) == scc.end())
                        //            iter = AK[W].erase(iter); // advances iter
                        //        else
                        //            ++iter; // don't remove
                        //    }
                        //}

                        int maxDegS = *max_element(scc.begin(), scc.end(), [this](int a, int b) {return inDeg[a] > inDeg[b];});
                        cycle(maxDegS,0);
                        //for (int W : scc)
                        //{
                        //    reach[W] = false;
                        //    mark[W] = false;
                        //}
                    }
                    //runInSubGraph(scc);

                    sccFound.insert(scc.begin(), scc.end());
                }
                else
                    sccQueue.push_back(v);
            }
        }
    }

}

inline void CircuitFinder::noCycle(int x, int y)
{
    B[y].push_back(x);
    removedEdge[x].insert(y);
    //AK[x].erase(find(AK[x].begin(), AK[x].end(), y));
}

inline void CircuitFinder::unMark(int x)
{
    mark[x] = false;
    for (int y : B[x])
    {
        //AK[y].push_back(x);
        removedEdge[y].erase(x);
        if (mark[y])
            unMark(y);
    }
    B[x].clear();
}

//unblock子函数
//与Johnson论文中变量名称等都一致，除去Johnson是从list头部弹出，我们改成vector尾部弹出
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

//数据载入函数
//按照文件中所有帐户出现的先后顺序，将账户名压入nodes向量中
//AK存储邻接表，用vector<vector<int>>实现
void CircuitFinder::loadTestData(string filename)
{
#ifdef _WIN64
    unordered_map<int, int> intHash;
#else
    unordered_map<int,int> intHash(20000);
#endif
    ifstream indata;
    indata.open(filename);
    string line;
    int vertexIndex = 0;
    inDeg.resize(140000, 0);
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
            intHash[accountOut] = vertexIndex;
            nodes[vertexIndex++] = accountOut;
        }

        if(intHash.find(accountIn)==intHash.end())
        // if (!m.count(accountIn)) // 1700us
        {
            intHash[accountIn] = vertexIndex;
            nodes[vertexIndex++] = accountIn;
        }

        AK[intHash[accountOut]].push_back(intHash[accountIn]); // 400us
        inDeg[intHash[accountIn]] += 1;
    }
    N = vertexIndex;
    B.resize(N);Blocked.resize(N,false);
    mark.resize(N, false); reach.resize(N, false); position.resize(N);
    nodes.resize(N); AK.resize(N); inDeg.resize(N); removedEdge.resize(N);

    falseBlocked = Blocked;
#ifdef mydebug
    outputTime("Load Data");
    printMap();
#endif
}


//迭代版的找环函数，发现有性能问题，没使用
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


//szwarc的找环函数
//与Szwarc论文一致，除去限制了搜索深度，到7停止
bool CircuitFinder::cycle(int v, int q)
{
    bool f;
    int t;
    mark[v] = true;
    f = false;
    szwarcStack.push_back(v);
    t = Stack.size();
    position[v] = t;
    if (!reach[v])
        q = t;
    for (int w : AK[v])
    {
        if (removedEdge[v].find(w) != removedEdge[v].end())
            continue;
        if (!mark[w])
        {
            if (cycle(w,q))
                f = true;
            else
                noCycle(v, w);
        }
        else if (position[w] <= q)
        {
            szwarcOutput(v, w);
            f = true;
        }
        else
            noCycle(v, w);
    }
    szwarcStack.pop_back();
    if (f)
        unMark(v);
    reach[v] = true;
    position[v] = N;
    return f;
}

//递归版的找环函数
//与Johnson论文一致，除去限制了搜索深度，到7停止
bool CircuitFinder::circuit(int V)
{
    bool F = false;

    Stack.push_back(V);
    Blocked[V] = true;

    auto circuitLen = Stack.size();
    if (circuitLen < 7)
    {
        for (int W : AK[V]) {
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
        for (int W : AK[V]) {
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
        for (int W : AK[V]) {
            auto IT = std::find(B[W].begin(), B[W].end(), V);
            if (IT == B[W].end()) {
                B[W].push_back(V);
            }
        }
    }

    Stack.pop_back();
    return F;
}

//找到一条环之后，定位环中账户名称最小的位置
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

//比较两条环的函数，用于环排序
bool CircuitFinder::compareVector(vector<int> v1, vector<int> v2)
{
    for (int i=0;i<v1.size();i++)
    {
        if (v1[i]!=v2[i])
            return v1[i]<v2[i];
    }
    return true;
}

//输出时间，辅助函数
void CircuitFinder::outputTime(string info)
{
    auto duration = duration_cast<microseconds>(system_clock::now() - start);
    cout <<  info
         << double(duration.count()) * microseconds::period::num / microseconds::period::den
         << "Seconds" << endl;
}


//Circuit函数找到一条环后在此处output,即压入resVect
void CircuitFinder::output()
{
  auto circuitLen = Stack.size();
  if (circuitLen>2 && circuitLen<8)
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

void CircuitFinder::szwarcOutput(int v, int w)
{
    Stack.clear();
    auto posW = find(szwarcStack.begin(), szwarcStack.end(), w);
    auto posV = find(posW, szwarcStack.end(), v);

    for (vector<int>::iterator iter = posW; iter != posV; iter++)
    {
        Stack.push_back(*iter);
    }
    Stack.push_back(*posV);
    output();
}


//对同一种长度的环排序，最后调用
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

//文件输出函数
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

//输出nodes表，调试辅助函数
void CircuitFinder::printMap()
{
    ofstream fout("../data/map.txt");
    for (int i = 0; i < N; i++)
        fout << nodes[i] << endl;
    fout.close();
}

//在一个强连通分量中迭代所有的节点，调用circuit找环函数
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


//运行入口
void CircuitFinder::run()
{
  Stack.clear();
  S = 0;
#ifdef MYTIME
    struct timeval ov_start, ov_end;
    gettimeofday(&ov_start,NULL);
#endif
//   while (S < N) {
//     for (int I = S; I <= N; ++I) {
//       Blocked[I-1] = false;
//       B[I-1].clear();
//     }
//     circuit(S);
//     //circuitIterate(S);

//     removeNode(S);

//     ++S;

// #ifdef mydebug
//     outputTime("A S cycle");
//     cout << S << endl;
// #endif
//   }

strongComponent();
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
    cf.loadTestData("../data/test_data.txt");
#elif TEST
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
