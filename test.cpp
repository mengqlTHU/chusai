#include <bits/stdc++.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
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

int append_uint_to_str(char* s, unsigned int i)
{
    int size = intSize(i);
    s[size] = ',';
    int tmp = size;
    while (i > 0)
    {
        s[--size] = '0' + i % 10;
        i /= 10;
    }
    return tmp + 1;
};

typedef int ui;
struct Path{
    //ID鏈€灏忕殑绗竴涓緭鍑猴紱
    //鎬讳綋鎸夌収寰幆杞处璺緞闀垮害鍗囧簭鎺掑簭锛�
    //鍚屼竴绾у埆鐨勮矾寰勯暱搴︿笅寰幆杞处璐﹀彿ID搴忓垪锛屾寜鐓у瓧鍏稿簭锛圛D杞负鏃犵鍙锋暣鏁板悗锛夊崌搴忔帓搴�
    int length;
    vector<ui> path;

    Path(int length, const vector<ui> &path) : length(length), path(path) {}

    Path(int length_new, int* path_new) : length(length_new){
        while(--length_new>=0)
        {
            path.push_back(path_new[length_new]);
        }
        // if(length!=path.size()) printf("Error\n");
        // assert(path.size()==length);
    }

    bool operator<(const Path&rhs)const{
        if(length!=rhs.length) return length<rhs.length;
        for(int i=0;i<length;i++){
            if(path[i]!=rhs.path[i])
                return path[i]<rhs.path[i];
        }
    }
};

class Solution{
public:
    //maxN=560000
    //maxE=280000 ~avgN=26000
    //vector<int> *G;
    vector<vector<int>> G;
    vector<vector<int>> invG;
    unordered_map<ui,int> idHash; //sorted id to 0...n
    vector<ui> ids; //0...n to sorted id
    vector<ui> inputs; //u-v pairs
    vector<int> inDegrees;
    vector<bool> vis;
    vector<Path> ans;
    // int ans3[3*500000];
    // int ans4[4*500000];
    // int ans5[5*1000000];
    // int ans6[6*2000000];
    // int ans7[7*3000000];
    int nodeCnt;
    vector<int> pre_search_2;

    void parseInput(string &testFile){
        ui u,v,c;
        int cnt=0;
        // while(fscanf(file,"%u,%u,%u",&u,&v,&c)!=EOF){
        //     inputs.push_back(u);
        //     inputs.push_back(v);
        //     ++cnt;
        // }
        int fd = open(testFile.c_str(),O_RDONLY);
        int length = lseek(fd,0,SEEK_END);
        char* p = (char*)mmap(0,length,PROT_READ,MAP_SHARED,fd,0);
        char* pp = p;
        while(pp-p<length)
        {
            u=0;v=0;c=0;
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
            while (*pp++ != '\n'){}
            inputs.push_back(u);
            inputs.push_back(v);
            ++cnt;
        }
#ifdef TEST
        printf("%d Records in Total\n",cnt);
#endif
    }

    void constructGraph(){
        auto tmp=inputs;
        sort(tmp.begin(),tmp.end());
        tmp.erase(unique(tmp.begin(),tmp.end()),tmp.end());
        nodeCnt=tmp.size();
        ids=tmp;
        nodeCnt=0;
        for(ui &x:tmp){
            idHash[x]=nodeCnt++;
        }
#ifdef TEST
        printf("%d Nodes in Total\n",nodeCnt);
#endif
        int sz=inputs.size();
        //G=new vector<int>[nodeCnt];
        G=vector<vector<int>>(nodeCnt);
        invG=vector<vector<int>>(nodeCnt);
        inDegrees=vector<int>(nodeCnt,0);
        for(int i=0;i<sz;i+=2){
            int u=idHash[inputs[i]],v=idHash[inputs[i+1]];
            G[u].push_back(v);
            if(v<u) invG[v].push_back(u);
            ++inDegrees[v];
        }
    }

    void dfs(int head,int cur,int depth,vector<int> &path,int* path_new){
        vis[cur]=true;
        path.push_back(cur);
        *path_new++ = cur;

        for(int &v:G[cur]){
            if(v==head && depth>=3 && depth<=7){
                vector<ui> tmp;
                for(int &x:path)
                    tmp.push_back(ids[x]);
                ans.push_back(Path(depth,tmp));
                // ans.push_back(Path(depth,path_new));
            }
            if(depth<6 && !vis[v] && v>head){
                dfs(head,v,depth+1,path,path_new);
            }
            if(depth==6 && !vis[v] && v>head){
                if(pre_search_2[v]==1){
                    vector<ui> tmp;
                    for(int &x:path)
                        tmp.push_back(ids[x]);
                    tmp.push_back(ids[v]);
                    // path.push_back(v);
                    ans.emplace_back(Path(depth+1,tmp));
                    // path.pop_back();
                }
            }
        }
        vis[cur]=false;
        path.pop_back();
        path_new--;
    }

    //search from 0...n
    //鐢变簬瑕佹眰id鏈€灏忕殑鍦ㄥ墠锛屽洜姝ゆ悳绱㈢殑鍏ㄨ繃绋嬩腑涓嶈€冭檻姣旇捣鐐筰d鏇村皬鐨勮妭鐐�
    void solve(){
        vis=vector<bool>(nodeCnt,false);
        vector<int> path;
        int path_new[7];
        pre_search_2.resize(nodeCnt);
        for (int i = 0; i < nodeCnt; ++i)
        {
            pre_search_2[i] = 0;
        }
        for(int i=0;i<nodeCnt;i++){
            for(int &v:invG[i])
            {
                pre_search_2[v] = 1;
            }
#ifdef TEST
            // if(i%100==0)
                // cout<<i<<"/"<<nodeCnt<<endl;
#endif
            if(!G[i].empty()){
                dfs(i,i,1,path,&path_new[0]);
            }
            for(int &v:invG[i])
            {
                pre_search_2[v] = 0;
            }
        }
        sort(ans.begin(),ans.end());
    }

    void save(string &outputFile){
#ifdef TEST
        printf("Total Loops %d\n",(int)ans.size());
#endif
        // int fd = open(outputFile.c_str(),O_RDWR|O_CREAT|O_TRUNC,0666);
        ofstream out(outputFile);
        char* p = new char[ans.size() * 80];
        char* pp = (char*)p;
        pp += append_uint_to_str(pp,ans.size());
        pp[-1] = '\n';
        int tmp[7];
        for(auto &x:ans){
            int sz=x.path.size();
            for(int i=0;i<sz;i++)
            {
                pp += append_uint_to_str(pp, x.path[i]);
                // pp += itostr(pp,x.path[i]);
            }
            pp[-1] = '\n';
        }
        // truncate(outputFile.c_str(),pp-p);
        // void* dst_ptr = mmap(0,pp-p,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
        // memcpy(dst_ptr,p,pp-p);
        // munmap(dst_ptr,pp-p);
        out.write(p, pp - p);
    }
};

int main()
{
#ifdef TEST
    string testFile = "./data/38252/test_data.txt";
#else
    string testFile = "/data/test_data.txt";
#endif
    string outputFile = "/projects/student/result.txt";
#ifdef TEST
    string answerFile = "answer.txt";
#endif
#ifdef TEST
    auto t=clock();
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
        cout<<clock()-t<<endl;
#endif
//    }

    return 0;
}
