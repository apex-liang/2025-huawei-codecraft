#include <cstdio>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <queue>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <cstring>
#include <unordered_set>
#include <unordered_map>
#include <bitset>
#include <fstream>
#include <algorithm>


// 目前311匹配验证非常完美版本
// 磁盘切的太碎，读取不利，切的太大，竞争太大，所以需要调参
#define MAX_DISK_NUM (10 + 1)
#define MAX_DISK_SIZE (16384 + 1)
#define MAX_REQUEST_NUM (30000000 + 1)
#define MAX_OBJECT_NUM (100000 + 1)
#define MAX_TIMESTAMP (86400)
#define REP_NUM (3)
#define MAX_LABEL_NUM (16+1)
#define FRE_PER_SLICING (1800)
#define EXTRA_TIME (105)
#define BIT_NUM 120
#define MAX_STMP (48+1)
using namespace std;

typedef struct Request_ {
    int object_id;
    int prev_id;
    bitset<BIT_NUM> state;
    bool is_done;
    
} Request;

typedef struct Object_ {
    int replica[REP_NUM + 1];
    int* unit[REP_NUM + 1];
    int label;
    int size;
    int last_request_point;
    bool is_delete;
} Object;

inline Request request[MAX_REQUEST_NUM];
inline Object object[MAX_OBJECT_NUM];

inline int T, M, N, V, G;
#define disk_block_num 14//16
inline int block_size;
inline int read_sum[MAX_STMP];//时间段i内所有标签的读取请求总和，显示读压力
inline int disk_begin[MAX_DISK_NUM];
inline int disk_block[MAX_DISK_NUM][disk_block_num+1];//disk i block j内存储的label 是什么
inline int free_block[MAX_DISK_NUM][disk_block_num+1];
inline int cnt_disk_block[MAX_DISK_NUM][disk_block_num+1];//disk block j中需要读取的所有block数量
inline set<int> free_space_block[MAX_DISK_NUM];//disk i中还没被分配的块（没有被标记的块）
inline set<pair<int,int>> label_block[MAX_LABEL_NUM];
inline set<int> disk_label_able[MAX_DISK_NUM][disk_block_num+1];//disk i block j中可以用的块
inline int top_time[MAX_LABEL_NUM];

inline int disk[MAX_DISK_NUM][MAX_DISK_SIZE];
inline int disk_point[MAX_DISK_NUM];
inline int token[MAX_DISK_NUM];
inline int block_num[MAX_DISK_NUM][MAX_DISK_SIZE];
inline int successful_request[MAX_REQUEST_NUM];
inline int q[MAX_REQUEST_NUM],tt=-1,hh=0;
inline bool is_drump[MAX_DISK_NUM];
inline pair<int,int> work_object[MAX_DISK_NUM];
//int pre_token[MAX_DISK_NUM];
#define SPACE_NONE_LABEL 0//三片空闲区域
#define TEST_BLOCK_DIVIDE 32
inline pair<int,int> label_range[MAX_DISK_NUM][MAX_LABEL_NUM+SPACE_NONE_LABEL];
inline set<int> free_space[MAX_DISK_NUM];
inline pair<int,int> last_pos[MAX_LABEL_NUM];
inline int write_demand[MAX_STMP][MAX_LABEL_NUM];  // 写入需求：时间段 X 内标签 M 的写入量
inline int read_demand[MAX_STMP][MAX_LABEL_NUM];   // 读取需求：时间段 X 内标签 M 的读取量
inline int delete_demand[MAX_STMP][MAX_LABEL_NUM]; // 删除需求：时间段 X 内标签 M 的删除量
inline long int demand[MAX_LABEL_NUM][MAX_STMP]={0};
inline pair<long int,long int> max_demand[MAX_LABEL_NUM];
inline int pre_token_disk[MAX_DISK_NUM];
inline bool st[MAX_REQUEST_NUM];
inline int max_demand_all=0;
inline int max_have_data_pos[MAX_DISK_NUM][3];
inline int max_have_data_pos_1[MAX_DISK_NUM];
inline int min_have_data_pos[MAX_DISK_NUM];
inline int ok_all;
inline int request_all;
inline int delect_all;
inline int object_all;
inline pair<pair<int,int>,int> disk_point_range[50][MAX_DISK_NUM];
inline int read_demand_sort[MAX_LABEL_NUM];
inline set<int> label_need_read;//在当前时间段内需要读取的所有label
inline int time_stemp=0;
inline int time_stamp_all;
inline int max_demand_beifen[MAX_LABEL_NUM];
inline int demand_all_label[49];
inline int max_demand_all_label;
inline int demand_two[MAX_DISK_NUM][MAX_TIMESTAMP];
inline int max_demand_two[MAX_DISK_NUM];
inline bool cmp(int request_a,int request_b){
	int object_id_a=request[request_a].object_id,object_id_b=request[request_b].object_id;
	return object[object_id_b].size<object[object_id_a].size;
}
void read_action();
void delete_action();
void write_action();