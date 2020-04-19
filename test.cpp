#include <bits/stdc++.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
using namespace std;

#define TEST

#define MYTIME

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

int append_uint_to_str(char* s, unsigned int i)
{
    int size = intSize(i);
    s[size] = ',';
    char* c = &s[size-1];
    while(i>=100)
    {
       int pos = i % 100;
       i /= 100;
       *(short*)(c-1)=*(short*)(digit_pairs+2*pos); 
       c-=2;
    }
    while(i>0)
    {
        *c--='0' + (i % 10);
        i /= 10;
    }
    return size + 1;
};

typedef int ui;
struct Path{
    //ID鏈€灏忕殑绗竴涓緭鍑猴紱
    //鎬讳綋鎸夌収寰幆杞处璺緞闀垮害鍗囧簭鎺掑簭锛�
    //鍚屼竴绾у埆鐨勮矾寰勯暱搴︿笅寰幆杞处璐﹀彿ID搴忓垪锛屾寜鐓у瓧鍏稿簭锛圛D杞负鏃犵鍙锋暣鏁板悗锛夊崌搴忔帓搴�
    int length;
    int path[7];

    // Path(int length, const vector<ui> &path) : length(length), path(path) {}

    Path(int length_new, int* path_new) : length(length_new){
        int i=0;
        memcpy(path,path_new-length_new,length_new*sizeof(int));
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
    int G_arr_num[280000];
    int *G_arr;
    vector<vector<int>> invG;
    unordered_map<ui,int> idHash; //sorted id to 0...n
    vector<ui> ids; //0...n to sorted id
    vector<ui> inputs; //u-v pairs
    vector<int> inDegrees;
    vector<bool> vis;
    vector<vector<Path>> ans_arr;
    // int ans3[3*500000];
    // int ans4[4*500000];
    // int ans5[5*1000000];
    // int ans6[6*2000000];
    // int ans7[7*3000000];
    // int *ans_arr[5] = {ans3,ans4,ans5,ans6,ans7};
    int nodeCnt;
    int* direct_reach;
    int* onestep_reach;


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
        nodeCnt = 0;
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
            invG[v].push_back(u);
            ++inDegrees[v];
        }
        // G_arr = new int[280000*50];
        // memset(G_arr,-1,280000*50);
        // memset(G_arr_num,0,280000);
        // for(int i=0;i<sz;i+=2){
        //     int u=idHash[inputs[i]],v=idHash[inputs[i+1]];
        //     G_arr[u*50+G_arr_num[u]++] = v;
        // }
    }

    void dfs(int head,int cur,int depth,int* path_new){
        vis[cur]=true;
        *path_new++ = ids[cur];

        for(int &v:G[cur]){
            int idv = ids[v];
            if(v==head && depth>=3 && depth<=7){
                ans_arr[depth-3].emplace_back(Path(depth,path_new));
            }
            if(!vis[v] && v>head){
                if(depth==6 && direct_reach[v]==1){
                    *path_new++ = ids[v];
                    ans_arr[depth+1-3].emplace_back(Path(depth+1,path_new));
                    // sortedInsert(ans_arr_new[depth+1-3], new Path(depth+1,path_new));
                    path_new--;
                }
                if(depth<5)
                    dfs(head,v,depth+1,path_new);
                if(depth==5)
                {
                    // if(ids[head]==83 && ids[v]==115) printf("%d,%d\n", v,onestep_reach[v]);
                    if(onestep_reach[v]==1)
                        dfs(head,v,depth+1,path_new);
                }
            }
        }
        vis[cur]=false;
        path_new--;
    }

    //search from 0...n
    //鐢变簬瑕佹眰id鏈€灏忕殑鍦ㄥ墠锛屽洜姝ゆ悳绱㈢殑鍏ㄨ繃绋嬩腑涓嶈€冭檻姣旇捣鐐筰d鏇村皬鐨勮妭鐐�
    void solve(){
        vis=vector<bool>(nodeCnt,false);
        vector<int> path;
        ans_arr.resize(5);
        int path_new[7];
        direct_reach = new int[nodeCnt];
        onestep_reach = new int[nodeCnt];
        memset(direct_reach,0,nodeCnt);
        memset(onestep_reach,0,nodeCnt);
        // for (int i = 0; i < nodeCnt; ++i)
        // {
            // direct_reach[i] = 0;
            // onestep_reach[i] = 0;
        // }
        for(int i=0;i<nodeCnt;i++){
            for(int &v:invG[i])
            {
                direct_reach[v] = 1;
                onestep_reach[v] = 1;
                for (int &vv:invG[v])
                {
                    onestep_reach[vv] = 1;
                }
            }
#ifdef TEST
            // if(i%100==0)
                // cout<<i<<"/"<<nodeCnt<<endl;
#endif
            if(!G[i].empty()){
                dfs(i,i,1,&path_new[0]);
            }
            for(int &v:invG[i])
            {
                direct_reach[v] = 0;
                onestep_reach[v] = 0;
                for(int &vv:invG[v])
                {
                    onestep_reach[vv] = 0;
                }
            }
        }
        for (int i = 0; i < ans_arr.size(); ++i)
        {
            sort(ans_arr[i].begin(),ans_arr[i].end());
        }
    }

    void save(string &outputFile){
        int count = 0;
        for(auto &a:ans_arr){
            count += a.size();
        }
#ifdef TEST
        printf("Total Loops %d\n",count);
#endif
        // int fd = open(outputFile.c_str(),O_RDWR|O_CREAT|O_TRUNC,0666);
        FILE *fp = fopen(outputFile.c_str(),"w");
        // ofstream out(outputFile);
        // char* dst_ptr = (char*)mmap(0,count* 80,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
        // truncate(outputFile.c_str(),count* 80);
        char* p = new char[count* 80];
        char* pp = (char*)p;
        pp += append_uint_to_str(pp,count);
        pp[-1] = '\n';
        int tmp[7];
        for(auto &a:ans_arr){
            int sz=a[0].length;
            for(auto &x:a){
                for(int i=0;i<sz;i++)
                {
                    // char* tmp = pp;
                    // *pp++=',';
                    // printf("%s\n", tmp);
                    pp += append_uint_to_str(pp, x.path[i]);
                // pp += itostr(pp,x.path[i]);
                }
                pp[-1] = '\n';
            }
        }
        // memcpy(dst_ptr,p,pp-p);
        // truncate(outputFile.c_str(),pp-dst_ptr);
        // munmap(dst_ptr,pp-p);
        // out.write(p, pp - p);
        fwrite(p,1,pp-p,fp);
    }
};

int main()
{
#ifdef TEST
    string testFile = "./data/58284/test_data.txt";
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
