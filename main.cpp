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
#include <assert.h>

using namespace std;
using namespace chrono;

#define TEST

//#define MYTIME

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
  //vector<NodeList> subAK;
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
  void runInSubGraph(set<int>& s);
  void runInSubGraph(int* s, int len);

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

//Tarjan�㷨Ѱ������ǿ��ͨ����
//https://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm
void CircuitFinder::strongComponent()
{
//    unordered_map<int, int> preOrder;
//    unordered_map<int, int> lowLink;
//    set<int> sccFound;
//    vector<int> sccQueue;
    int preOrder_arr[N];
    int sccFound_arr[N];
    int lowLink_arr[N];
    int q_arr[N];
    int q_n = 0;
    int sccQueue_arr[N];
    int sccQueue_n = 0;
    int scc_arr[N];
    int v;
    bool done;
    int i = 0;
    for(int node=0;node<N;node++) 
    {
	preOrder_arr[node] = -1;
	sccFound_arr[node] = -1;
	scc_arr[node] = -1;
    }
    for (int node = 0; node < N; node++)
    {
//        vector<int> q;
//        if (sccFound.find(node)==sccFound.end())
        if (sccFound_arr[node]==-1)
	{
//            q.push_back(node);
	    q_arr[q_n++]=node;
	}

        while (q_n>0)
        {
//            v = q.back();
	    v = q_arr[q_n-1];
            if (preOrder_arr[v]==-1)
            {
                i++;
//                preOrder[v] = i;
		preOrder_arr[v] = i;
            } //else assert(preOrder[v]==preOrder_arr[v]);
            done = true;
            for (int w : AK[v])
            {
                if (preOrder_arr[w]==-1)
                {
//                    q.push_back(w);
		    q_arr[q_n++]=w;
                    done = false;
                    break;
                }
            }
            if (done)
            {
//                lowLink[v] = preOrder_arr[v];
		lowLink_arr[v] = preOrder_arr[v];
                for (int w : AK[v])
                {
                    if (sccFound_arr[w]==-1)
                    {
                        if (preOrder_arr[w] > preOrder_arr[v])
//                            lowLink[v] = lowLink[v] < lowLink[w] ? lowLink[v] : lowLink[w];
                            lowLink_arr[v] = min(lowLink_arr[v],lowLink_arr[w]);
                        else
//                            lowLink[v] = lowLink[v] < preOrder_arr[w] ? lowLink[v] : preOrder_arr[w];
                            lowLink_arr[v] = min(lowLink_arr[v],preOrder_arr[w]);
                    }
                }
//                q.pop_back();
		q_n--;
                if (lowLink_arr[v] == preOrder_arr[v])
                {
                    set<int> scc;
		    int scc_val[sccQueue_n+1];
		    int scc_i = 0;
		    scc_val[scc_i++] = v;
                    scc.insert(v);
		    scc_arr[v] = 1;
//                    while (!sccQueue.empty() && preOrder_arr[sccQueue.back()] > preOrder_arr[v])
                    while (sccQueue_n>0 && preOrder_arr[sccQueue_arr[sccQueue_n-1]] > preOrder_arr[v])
                    {
//                        int k = sccQueue.back();
			int k = sccQueue_arr[--sccQueue_n];
//                        sccQueue.pop_back();
                        scc.insert(k);
			scc_arr[k] = 1;
			scc_val[scc_i++]=k;
                    }

                    //vector<NodeList> subAK = getSubGraph(scc);
                    for (int ii=0;ii<scc_i;ii++)
		    {
			int tmp = scc_val[ii];
                        for (vector<int>::iterator iter = AK[tmp].begin(); iter != AK[tmp].end(); )
			{ 
                            if (scc_arr[*iter] == -1)
                                iter = AK[tmp].erase(iter); // advances iter
                            else
                                ++iter; // don't remove
                        }
			sccFound_arr[tmp] = 1;
                    }

//                    for (int W : scc)
//                    {
//                        for (vector<int>::iterator iter = AK[W].begin(); iter != AK[W].end(); )
//                        {
//                            if (scc_arr[*iter] == -1)
//                                iter = AK[W].erase(iter); // advances iter
//                            else
//                                ++iter; // don't remove
//                        }
//			sccFound_arr[W] = 1;
//                    }

//                    runInSubGraph(scc);
                    runInSubGraph(scc_val, scc_i);

//                    sccFound.insert(scc.begin(), scc.end());
                }
                else
		{
//                    sccQueue.push_back(v);
		    sccQueue_arr[sccQueue_n++] = v;
		}
            }
        }
    }

}

//unblock�Ӻ���
//��Johnson�����б������Ƶȶ�һ�£���ȥJohnson�Ǵ�listͷ�����������Ǹĳ�vectorβ������
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

//�������뺯��
//�����ļ��������ʻ����ֵ��Ⱥ�˳�򣬽��˻���ѹ��nodes������
//AK�洢�ڽӱ�����vector<vector<int>>ʵ��
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


//��������һ��������������������⣬ûʹ��
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


//�ݹ����һ�����
//��Johnson����һ�£���ȥ������������ȣ���7ֹͣ
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

//�ҵ�һ����֮�󣬶�λ�����˻�������С��λ��
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

//�Ƚ��������ĺ��������ڻ�����
bool CircuitFinder::compareVector(vector<int> v1, vector<int> v2)
{
    for (int i=0;i<v1.size();i++)
    {
        if (v1[i]!=v2[i])
            return v1[i]<v2[i];
    }
    return true;
}

//���ʱ�䣬��������
void CircuitFinder::outputTime(string info)
{
    auto duration = duration_cast<microseconds>(system_clock::now() - start);
    cout <<  info
         << double(duration.count()) * microseconds::period::num / microseconds::period::den
         << "Seconds" << endl;
}


//Circuit�����ҵ�һ�������ڴ˴�output,��ѹ��resVect
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


//��ͬһ�ֳ��ȵĻ�����������
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

//�ļ��������
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

//���nodes�������Ը�������
void CircuitFinder::printMap()
{
    ofstream fout("../data/map.txt");
    for (int i = 0; i < N; i++)
        fout << nodes[i] << endl;
    fout.close();
}

//��һ��ǿ��ͨ�����е������еĽڵ㣬����circuit�һ�����
//void CircuitFinder::runInSubGraph(set<int>& s)
//{
//    for (set<int>::iterator iter=s.begin(); iter!=s.end();iter++)
//    {
//        S = *iter;
//        Blocked = falseBlocked;
//        for (set<int>::iterator inner_iter = iter; inner_iter != s.end(); inner_iter++) {
//            B[*(inner_iter)].clear();
//        }
////        circuit(S);
//
//#ifdef mydebug
//    outputTime("A S cycle");
//    cout << S << endl;
//#endif
//
//    }
//}

void CircuitFinder::runInSubGraph(int* s, int len)
{
    for (int i=0;i<len;i++)
    {
        S = s[i];
        Blocked = falseBlocked;
	for(int j=i;j<len;j++)
	{
//        for (set<int>::iterator inner_iter = iter; inner_iter != s.end(); inner_iter++) {
            B[s[j]].clear();
        }
        circuit(S);

#ifdef mydebug
    outputTime("A S cycle");
    cout << S << endl;
#endif

    }
}


//�������
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
#elif defined TEST
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
