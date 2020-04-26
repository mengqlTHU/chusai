#ifdef _WIN64
#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <time.h>
#else
#include <bits/stdc++.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#endif

using namespace std;

// #define TEST

// #define MYTIME

int sizeTable[10] = { 9, 99, 999, 9999, 99999, 999999, 9999999,
  99999999, 999999999, INT8_MAX };

inline int intSize(unsigned int x)
{
    for (int i = 0; i < 10; ++i)
    {
        if (x <= sizeTable[i]) return i + 1;
    }
};

const char digit_pairs[201] = {
  "00010203040506070809"
  "10111213141516171819"
  "20212223242526272829"
  "30313233343536373839"
  "40414243444546474849"
  "50515253545556575859"
  "60616263646566676869"
  "70717273747576777879"
  "80818283848586878889"
  "90919293949596979899"
};

inline int append_uint_to_str(char* s, unsigned int i)
{
    //    if (i == 0)
    //    {
    //        *s = '0';
    //        *(s + 1) = ',';
    //        return 2;
    //    }
    int size = intSize(i);
    s[size] = ',';
    char* c = &s[size - 1];
    while (i >= 100)
    {
        int pos = i % 100;
        i /= 100;
        *(short*)(c - 1) = *(short*)(digit_pairs + 2 * pos);
        c -= 2;
    }
    while (i > 0)
    {
        *c-- = '0' + (i % 10);
        i /= 10;
    }
    return size + 1;
};

void insertSort(int* a, int N) {
    int i, j;
    for (i = 1; i < N; i++) {
        for (j = i - 1; j >= 0 && a[i] < a[j]; j--) {

        }
        //这里跳出内层循环，a[i]应被插入到a[j]后
        int tmp = a[i];
        for (int k = i; k > j + 1; k--) {
            a[k] = a[k - 1];
        }
        a[j + 1] = tmp;
    }
}

void shellSort(int* a, int N) {
    int h = 1;
    while (h < N / 3) {
        h = 3 * h + 1; //h的取值序列为1, 4, 13, 40, ...
    }
    while (h >= 1) {
        int n, i, j, k;
        //分割后，产生n个子序列
        for (n = 0; n < h; n++) {
            //分别对每个子序列进行插入排序
            for (i = n + h; i < N; i += h) {
                for (j = i - h; j >= 0 && a[i] < a[j]; j -= h) {

                }
                int tmp = a[i];
                for (k = i; k > j + h; k -= h) {
                    a[k] = a[k - h];
                }
                a[j + h] = tmp;
            }
        }
        h = h / 3;
    }
}

typedef uint32_t ui;

class Solution {
public:
    //maxN=560000
    //maxE=280000 ~avgN=26000
    //vector<int> *G;
    vector<vector<uint32_t>> G;
    int G_arr_num[280000];
    int invG_arr_num[280000];
    int* G_arr;
    int* invG_arr;
    vector<vector<uint32_t>> invG;
    unordered_map<ui, uint32_t> idHash; //sorted id to 0...n
    vector<ui> ids; //0...n to sorted id
    char* idsStr;
    int* idsStrIndex;
    int* idsStrStep;
    vector<ui> inputs; //u-v pairs
    vector<int> input_to_node;
    vector<int> node_to_input;
    //vector<int> inDegrees;
    vector<bool> vis;
    //    vector<vector<Path>> ans_arr;
    char* ans[5];
    int n_ans[5] = { 0,0,0,0,0 };
    int ans_top[5] = { 0,0,0,0,0 };
    int nodeCnt;
    bool* direct_reach;
    bool* onestep_reach;

    void parseInput(string& testFile) {
        ui u, v, c;
        int cnt = 0;
        // while(fscanf(file,"%u,%u,%u",&u,&v,&c)!=EOF){
        //     inputs.push_back(u);
        //     inputs.push_back(v);
        //     ++cnt;
        // }
#ifdef _WIN64
        ifstream in(testFile);
        string contents((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
        int length = contents.length();
        char* p = new char[length];
        strcpy(p, contents.c_str());
        input_to_node = vector<int>(280000, 0);
#else
        int fd = open(testFile.c_str(), O_RDONLY);
        int length = lseek(fd, 0, SEEK_END);
        char* p = (char*)mmap(0, length, PROT_READ, MAP_SHARED, fd, 0);
        input_to_node = vector<int>(280000, 0);
#endif
        char* pp = p;
        nodeCnt = 0;
        while (pp - p < length)
        {
            u = 0; v = 0; c = 0;
            while (*pp != ',')
            {
                u = (*pp++ - '0') + u * 10;
            }
            pp++;
            while (*pp != ',')
            {
                v = (*pp++ - '0') + v * 10;
            }
            pp++;
            while (*pp++ != '\n') {}
            inputs.push_back(u);
            inputs.push_back(v);
            input_to_node[u]++;
            input_to_node[v]++;
            // if (idHash.find(u) == idHash.end())
            // {
            //     idHash[u] = nodeCnt++;
            // }
            // if (idHash.find(v) == idHash.end())
            // {
            //     idHash[v] = nodeCnt++;
            // }
            ++cnt;
        }
#ifdef TEST
        printf("%d Records in Total\n", cnt);
#endif
    }

    void constructGraph() {
        // auto tmp = inputs;
        // sort(tmp.begin(), tmp.end());
        // tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());
        // //ids = tmp;
        // nodeCnt = 0;
            // idHash[1] = nodeCnt++;
        // for (ui& x : tmp) {

            // idHash[x] = nodeCnt++;
        // }
        nodeCnt = 0;
        node_to_input = vector<int>(280000, 0);
        for (int i = 0; i < 280000; ++i)
        {
            if (input_to_node[i] > 0)
            {
                node_to_input[nodeCnt] = i;
                input_to_node[i] = nodeCnt++;
            }
        }
        // assert(newNodeCnt==nodeCnt);
        idsStr = new char[nodeCnt * 10];
        idsStrIndex = new int[nodeCnt + 1];
        idsStrStep = new int[nodeCnt];
        char* p = &idsStr[0];
        int index = 0; int step = 0;
        idsStrIndex[0] = 0;
        for (int i = 0; i < nodeCnt; i++)
        {
            p += append_uint_to_str(p, node_to_input[i]);
            //            p += step;
            idsStrIndex[i + 1] = p - idsStr;
            //            idsStrStep[i] = step;
            //            index += step;
        }
#ifdef TEST
        printf("%d Nodes in Total\n", nodeCnt);
#endif
        int sz = inputs.size();
        //G=new vector<int>[nodeCnt];
//        G = vector<vector<uint32_t>>(nodeCnt);
//        invG = vector<vector<uint32_t>>(nodeCnt);
        //for (int i = 0; i < nodeCnt; i ++) {
        //    G[i].reserve(10);
        //    invG[i].reserve(10);
        //}
        //inDegrees = vector<int>(nodeCnt, 0);
        int u, v;
        //        for (int i = 0; i < sz; i += 2) {
        //            u = idHash[inputs[i]]; v = idHash[inputs[i + 1]];
        //            G[u].push_back(v);
        //            invG[v].push_back(u);
        //            //++inDegrees[v];
        //        }
        //        for (int i = 0; i < nodeCnt; i++)
        //        {
        //            sort(G[i].begin(), G[i].end());
        ////	    insertSort((int*)&G[i][0],G[i].size());
        ////	    shellSort((int*)&G[i][0],G[i].size());
        //        }
        G_arr = new int[280000 * 50];
        invG_arr = new int[280000 * 50];
        //        memset(G_arr,-1,280000*50*sizeof(int));
        //        memset(G_arr_num,0,280000*sizeof(int));
        for (int i = 0; i < sz; i += 2) {
            int u = input_to_node[inputs[i]], v = input_to_node[inputs[i + 1]];
            G_arr[u * 50 + G_arr_num[u]++] = v;
            invG_arr[v * 50 + invG_arr_num[v]++] = u;
        }
        for (int i = 0; i < nodeCnt; i++)
        {
            shellSort((int*)&G_arr[i * 50], G_arr_num[i]);
        }

    }

    void dfs(int head, int cur, int depth, char* path_new, char* path_head) {
        vis[cur] = true;
        //        int len = idsStrStep[cur];
        int len = idsStrIndex[cur + 1] - idsStrIndex[cur];
        memcpy(path_new, &idsStr[idsStrIndex[cur]], len * sizeof(char));
        path_new += len;

        //        for (uint32_t& v : G[cur]) {
        for (int i = 0; i < G_arr_num[cur]; i++) {
            ui v = G_arr[cur * 50 + i];
            // int idv = ids[v];
            if (v == head && depth >= 3) {
                //memcpy(&ans[depth - 3][n_ans[depth - 3]++ * depth], path_new - depth, depth * sizeof(int));
                memcpy(&ans[depth - 3][ans_top[depth - 3]], path_head, path_new - path_head);
                ans_top[depth - 3] += path_new - path_head;
                n_ans[depth - 3]++;
                ans[depth - 3][ans_top[depth - 3] - 1] = '\n';
            }
            if (!vis[v] && v > head) {
                if (depth == 6 && direct_reach[v]) {
                    //*path_new++ = ids[v];
                    //memcpy(&ans[4][n_ans[4]++ * 7], path_new - 7, 7 * sizeof(int));
                    //path_new--;
                    int len7 = idsStrIndex[v + 1] - idsStrIndex[v];
                    memcpy(path_new, &idsStr[idsStrIndex[v]], len7 * sizeof(char));
                    path_new += len7;
                    memcpy(&ans[4][ans_top[4]], path_head, path_new - path_head);
                    ans_top[4] += path_new - path_head;
                    n_ans[4]++;
                    ans[4][ans_top[4] - 1] = '\n';
                    path_new -= len7;
                }
                if (depth < 4)
                    dfs(head, v, depth + 1, path_new, path_head);
                if (depth == 4 || depth == 5)
                {
                    if (onestep_reach[v])
                        dfs(head, v, depth + 1, path_new, path_head);
                }
            }
        }
        vis[cur] = false;
        path_new -= len;
    }

    //search from 0...n
    //鐢变簬瑕佹眰id鏈€灏忕殑鍦ㄥ墠锛屽洜姝ゆ悳绱㈢殑鍏ㄨ繃绋嬩腑涓嶈€冭檻姣旇捣鐐筰d鏇村皬鐨勮妭鐐�
    void solve() {
        ans[0] = new char[3 * 500000 * 40];
        ans[1] = new char[4 * 500000 * 50];
        ans[2] = new char[5 * 1000000 * 60];
        ans[3] = new char[6 * 2000000 * 70];
        ans[4] = new char[7 * 3000000 * 80];

        vis = vector<bool>(nodeCnt, false);
        vector<int> path;
        //        ans_arr.resize(5);
        uint32_t path_new[7];
        char* path_new_char = new char[200];
        char* path_head = path_new_char;
        direct_reach = new bool[nodeCnt];
        onestep_reach = new bool[nodeCnt];
        memset(direct_reach, false, nodeCnt);
        memset(onestep_reach, false, nodeCnt);

        for (int i = 0; i < nodeCnt; i++) {
            int* p = &invG_arr[i * 50];
            for (int j = 0; j < invG_arr_num[i]; j++)
                //for (uint32_t& v : invG[i])
            {
                int v = p[j];
                //if(v<i) continue;
                direct_reach[v] = true;
                onestep_reach[v] = true;
                // invvis[v] = true;
                int* pp = &invG_arr[v * 50];
                for (int k = 0; k < invG_arr_num[v]; k++)
                    //for (uint32_t& vv : invG[v])
                {
                    int vv = pp[k];
                    //if(vv<i) continue;
                    onestep_reach[vv] = true;
                    int* ppp = &invG_arr[vv * 50];
                    for (int l = 0; l < invG_arr_num[vv]; l++)
                        //for (uint32_t& vvv : invG[vv])
                    {
                        int vvv = ppp[l];
                        //if(vvv<i) continue;
                        onestep_reach[vvv] = true;
                    }
                }
            }
            //if (!G[i].empty()) {
            if (G_arr_num[i] > 0) {
                dfs(i, i, 1, path_new_char, path_head);
            }
            for (int j = 0; j < invG_arr_num[i]; j++)
                //for (uint32_t& v : invG[i])
            {
                int v = p[j];
                //if(v<i) continue;
                direct_reach[v] = false;
                onestep_reach[v] = false;
                // invvis[v] = true;
                int* pp = &invG_arr[v * 50];
                for (int k = 0; k < invG_arr_num[v]; k++)
                    //for (uint32_t& vv : invG[v])
                {
                    int vv = pp[k];
                    //if(vv<i) continue;
                    onestep_reach[vv] = false;
                    int* ppp = &invG_arr[vv * 50];
                    for (int l = 0; l < invG_arr_num[vv]; l++)
                        //for (uint32_t& vvv : invG[vv])
                    {
                        int vvv = ppp[l];
                        //if(vvv<i) continue;
                        onestep_reach[vvv] = false;
                    }
                }
            }
        }
    }

    void save(string& outputFile) {
        int count = 0;
        //        for(auto &a:ans_arr){
        //            count += a.size();
        //        }
        for (int i = 0; i < 5; i++) {
            count += n_ans[i];
        }
#ifdef TEST
        printf("Total Loops %d\n", count);
#endif
        // int fd = open(outputFile.c_str(),O_RDWR|O_CREAT|O_TRUNC,0666);
        FILE* fp = fopen(outputFile.c_str(), "w");
        // ofstream out(outputFile);
        // char* dst_ptr = (char*)mmap(0,count* 80,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
        // truncate(outputFile.c_str(),count* 80);
        char* p = new char[80];
        char* pp = (char*)p;
        pp += append_uint_to_str(pp, count);
        pp[-1] = '\n';
        fwrite(p, 1, pp - p, fp);
        //int tmp[7];
        //        for(auto &a:ans_arr){
        //            int sz=a[0].length;
        //            for(auto &x:a){
        //                for(int i=0;i<sz;i++)
        //                {
        //                    pp += append_uint_to_str(pp, x.path[i]);
        //                }
        //                pp[-1] = '\n';
        //            }
        //        }
        //for (int i = 0; i < 5; i++) {
        //    for (int j = 0; j < n_ans[i]; j++) {
        //        for (int k = 0; k < i + 3; k++)
        //        {
        //            pp += append_uint_to_str(pp, ans[i][j * (i + 3) + k]);
        //        }
        //        pp[-1] = '\n';
        //    }
        //}

        for (int i = 0; i < 5; i++)
        {
            //            memcpy(pp, ans[i], ans_top[i]);
            fwrite(ans[i], 1, ans_top[i], fp);
            //            pp += ans_top[i];
        }

        // memcpy(dst_ptr,p,pp-p);
        // truncate(outputFile.c_str(),pp-dst_ptr);
        // munmap(dst_ptr,pp-p);
        // out.write(p, pp - p);
//        fwrite(p, 1, pp - p, fp);
    }
};

int main()
{
#ifdef _WIN64
    string testFile = "../data/1004812/test_data.txt";
    //string testFile = "../test_data.txt";
    clock_t start, finish;
    double totaltime;
    start = clock();
#elif defined TEST
    string testFile = "/root/lhb/chusai/data/77409/test_data.txt";
#else
    string testFile = "/data/test_data.txt";
#endif

#ifdef _WIN64
    string outputFile = "../../data/myresults.txt";
#else
    string outputFile = "/projects/student/result.txt";
#endif 

#ifdef TEST
    string answerFile = "answer.txt";
#endif
#ifdef TEST
    auto t = clock();
#endif
    //    for(int i=0;i<100;i++){
    Solution solution;
#ifdef MYTIME
    struct timeval ov_start, ov_end;
    gettimeofday(&ov_start, NULL);
#endif
    solution.parseInput(testFile);
#ifdef MYTIME
    gettimeofday(&ov_end, NULL);
    double timeuse
        = 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
        + ov_end.tv_usec - ov_start.tv_usec;
    cout << "parseInput: " << timeuse << " us" << endl;
#endif
#ifdef MYTIME
    gettimeofday(&ov_start, NULL);
#endif
    solution.constructGraph();
#ifdef MYTIME
    gettimeofday(&ov_end, NULL);
    timeuse
        = 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
        + ov_end.tv_usec - ov_start.tv_usec;
    cout << "constructGraph: " << timeuse << " us" << endl;
#endif
    //solution.topoSort();
#ifdef MYTIME
    gettimeofday(&ov_start, NULL);
#endif
    solution.solve();
#ifdef MYTIME
    gettimeofday(&ov_end, NULL);
    timeuse
        = 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
        + ov_end.tv_usec - ov_start.tv_usec;
    cout << "solve: " << timeuse << " us" << endl;
#endif
#ifdef MYTIME
    gettimeofday(&ov_start, NULL);
#endif
    solution.save(outputFile);
#ifdef MYTIME
    gettimeofday(&ov_end, NULL);
    timeuse
        = 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
        + ov_end.tv_usec - ov_start.tv_usec;
    cout << "save: " << timeuse << " us" << endl;
#endif
#ifdef TEST
    cout << clock() - t << endl;
#endif

#ifdef _WIN64
    finish = clock();
    totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
    cout << "Run:  " << totaltime << "  Seconds" << endl;
#endif
    return 0;
}

