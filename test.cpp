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
#include <thread>
#else
#include <bits/stdc++.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <thread>
#endif

using namespace std;

//#define TEST

//#define MYTIME

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

typedef struct __THREAD_DATA
{
    int head; int cur; int depth; int* path_new; int thread_num;
}THREAD_DATA;

int append_uint_to_str(char* s, unsigned int i)
{
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

typedef uint32_t ui;
struct Path {
    //ID鏈€灏忕殑绗竴涓緭鍑猴紱
    //鎬讳綋鎸夌収寰幆杞处璺緞闀垮害鍗囧簭鎺掑簭锛�
    //鍚屼竴绾у埆鐨勮矾寰勯暱搴︿笅寰幆杞处璐﹀彿ID搴忓垪锛屾寜鐓у瓧鍏稿簭锛圛D杞负鏃犵鍙锋暣鏁板悗锛夊崌搴忔帓搴�
    int length;
    int path[7];

    // Path(int length, const vector<ui> &path) : length(length), path(path) {}

    Path(int length_new, int* path_new) : length(length_new) {
        int i = 0;
        memcpy(path, path_new - length_new, length_new * sizeof(int));
    }

    bool operator<(const Path& rhs)const {
        if (length != rhs.length) return length < rhs.length;
        for (int i = 0; i < length; i++) {
            if (path[i] != rhs.path[i])
                return path[i] < rhs.path[i];
        }
    }
};

bool cmp(Path* a, Path* b)
{
    return *a < *b;
}

class Solution {
public:
    //maxN=560000
    //maxE=280000 ~avgN=26000
    //vector<int> *G;
    vector<vector<int>> G;
    int G_arr_num[280000];
    int* G_arr;
    vector<vector<int>> invG;
    unordered_map<ui, int> idHash; //sorted id to 0...n
    vector<ui> ids; //0...n to sorted id
    vector<ui> inputs; //u-v pairs
    vector<int> inDegrees;
    vector<vector<bool>> vis;
    //    vector<vector<Path>> ans_arr;
    int* ans3;
    int* ans4;
    int* ans5;
    int* ans6;
    int* ans7;
    int* ans[5];
    int n_ans[5] = { 0,0,0,0,0 };
    int nodeCnt;
    int* direct_reach;
    int* onestep_reach;

    void parseInput(string& testFile) {
        ui u, v, c;
        int cnt = 0;
        // while(fscanf(file,"%u,%u,%u",&u,&v,&c)!=EOF){
        //     inputs.push_back(u);
        //     inputs.push_back(v);
        //     ++cnt;
        // }
#ifdef _WIN64
        //HANDLE f = CreateFileA(testFile.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        //HANDLE fd = CreateFileMappingA(f, NULL, PAGE_READONLY, 0, 0, NULL);
        ifstream in(testFile);
        string contents((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
        int length = contents.length();
        char* p = new char[length];
        strcpy(p, contents.c_str());
#else
        int fd = open(testFile.c_str(), O_RDONLY);
        int length = lseek(fd, 0, SEEK_END);
        char* p = (char*)mmap(0, length, PROT_READ, MAP_SHARED, fd, 0);
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
        auto tmp = inputs;
        sort(tmp.begin(), tmp.end());
        tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());
        nodeCnt = tmp.size();
        ids = tmp;
        nodeCnt = 0;
        for (ui& x : tmp) {
            idHash[x] = nodeCnt++;
        }
#ifdef TEST
        printf("%d Nodes in Total\n", nodeCnt);
#endif
        int sz = inputs.size();
        //G=new vector<int>[nodeCnt];
        G = vector<vector<int>>(nodeCnt);
        invG = vector<vector<int>>(nodeCnt);
        inDegrees = vector<int>(nodeCnt, 0);
        for (int i = 0; i < sz; i += 2) {
            int u = idHash[inputs[i]], v = idHash[inputs[i + 1]];
            G[u].push_back(v);
            invG[v].push_back(u);
            ++inDegrees[v];
        }
        for (int i = 0; i < nodeCnt; i++)
        {
            sort(G[i].begin(), G[i].end());
        }
        // G_arr = new int[280000*50];
        // memset(G_arr,-1,280000*50);
        // memset(G_arr_num,0,280000);
        // for(int i=0;i<sz;i+=2){
        //     int u=idHash[inputs[i]],v=idHash[inputs[i+1]];
        //     G_arr[u*50+G_arr_num[u]++] = v;
        // }
    }

    void dfs(int head, int cur, int depth, int* path_new, int thread_num) {
        vis[thread_num][cur] = true;
        *path_new++ = ids[cur];

        for (int& v : G[cur]) {
            // int idv = ids[v];
            if (v == head && depth >= 3 && depth <= 7) {
                memcpy(&ans[depth - 3][n_ans[depth - 3]++ * depth], path_new - depth, depth * sizeof(int));
            }
            if (!vis[thread_num][v] && v > head) {
                if (depth == 6 && direct_reach[thread_num * nodeCnt + v] == 1) {
                    *path_new++ = ids[v];
                    memcpy(&ans[4][n_ans[4]++ * 7], path_new - 7, 7 * sizeof(int));
                    path_new--;
                }
                if (depth < 4)
                    dfs(head, v, depth + 1, path_new, thread_num);
                if (depth == 4 || depth == 5)
                {
                    if (onestep_reach[thread_num * nodeCnt + v] == 1)
                        dfs(head, v, depth + 1, path_new, thread_num);
                }
            }
        }
        vis[thread_num][cur] = false;
        path_new--;
    }

    //search from 0...n
    //鐢变簬瑕佹眰id鏈€灏忕殑鍦ㄥ墠锛屽洜姝ゆ悳绱㈢殑鍏ㄨ繃绋嬩腑涓嶈€冭檻姣旇捣鐐筰d鏇村皬鐨勮妭鐐�
    void solve() {
        ans[0] = new int[3 * 500000];
        ans[1] = new int[4 * 500000];
        ans[2] = new int[5 * 1000000];
        ans[3] = new int[6 * 2000000];
        ans[4] = new int[7 * 3000000];

        const int thread_cnt = 1;
        int thread_num = 0;

        vis = vector<vector<bool>>(thread_cnt);
        for (int i = 0; i < thread_cnt; i++)
        {
            vis[i] = vector<bool>(nodeCnt, false);
        }

        thread th[thread_cnt];
        vector<int> path;
        //        ans_arr.resize(5);
        int path_new[7*thread_cnt];
        direct_reach = new int[nodeCnt*thread_cnt];
        onestep_reach = new int[nodeCnt*thread_cnt];
        memset(direct_reach, 0, nodeCnt*thread_cnt);
        memset(onestep_reach, 0, nodeCnt*thread_cnt);
        // for (int i = 0; i < nodeCnt; ++i)
        // {
            // direct_reach[i] = 0;
            // onestep_reach[i] = 0;
        // }
        // vector<bool> invvis(nodeCnt,false);
        for (int j = 0; j < nodeCnt; j+=thread_cnt) {
            for (int i = j; i < min(j + thread_cnt, nodeCnt); i++)
            {
                thread_num = i % thread_cnt;

                for (int& v : invG[i])
                {
                    //		if(v<i) continue;
                    direct_reach[v + thread_num * nodeCnt] = 1;
                    onestep_reach[v + thread_num * nodeCnt] = 1;
                    // invvis[v] = true;
                    for (int& vv : invG[v])
                    {
                        //		    if(vv<i) continue;
                        onestep_reach[vv + thread_num * nodeCnt] = 1;
                        for (int& vvv : invG[vv])
                        {
                            //			if(vvv<i) continue;
                            onestep_reach[vvv + thread_num * nodeCnt] = 1;
                        }
                    }
                }
            }
            for (int i = j; i < min(j + thread_cnt, nodeCnt); i++)
            {
                thread_num = i % thread_cnt;
                if (!G[i].empty()) {
                    th[thread_num] = thread(&Solution::dfs, this, i, i, 1, &path_new[thread_num*7], thread_num);
                    //dfs(i, i, 1, &path_new[0], thread_num);
                }
            }

            for (int i = j; i < min(j + thread_cnt, nodeCnt); i++)
            {
                thread_num = i % thread_cnt;
                if (!G[i].empty()) {
                    th[thread_num].join();
                }
            }

            for (int i = j; i < min(j + thread_cnt, nodeCnt); i++)
            {
                //cout << "processed: " << i << endl;
                thread_num = i % thread_cnt;
                for (int& v : invG[i])
                {
                    //		if(v<i) continue;
                    direct_reach[v + thread_num * nodeCnt] = 0;
                    onestep_reach[v + thread_num * nodeCnt] = 0;
                    for (int& vv : invG[v])
                    {
                        //		    if(vv<i) continue;
                        onestep_reach[vv + thread_num * nodeCnt] = 0;
                        for (int& vvv : invG[vv])
                        {
                            //			if(vvv<i) continue;
                            onestep_reach[vvv + thread_num * nodeCnt] = 0;
                        }
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
        char* p = new char[count * 80];
        char* pp = (char*)p;
        pp += append_uint_to_str(pp, count);
        pp[-1] = '\n';
        int tmp[7];
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
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < n_ans[i]; j++) {
                for (int k = 0; k < i + 3; k++)
                {
                    pp += append_uint_to_str(pp, ans[i][j * (i + 3) + k]);
                }
                pp[-1] = '\n';
            }
        }

        // memcpy(dst_ptr,p,pp-p);
        // truncate(outputFile.c_str(),pp-dst_ptr);
        // munmap(dst_ptr,pp-p);
        // out.write(p, pp - p);
        fwrite(p, 1, pp - p, fp);
    }
};

int main()
{
#ifdef _WIN64
    string testFile = "../data/3738/test_data.txt";
    //string testFile = "../../data/test_data_small.txt";
    clock_t start, finish;
    double totaltime;
    start = clock();
#elif defined TEST
    string testFile = "./data/58284/test_data.txt";
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
