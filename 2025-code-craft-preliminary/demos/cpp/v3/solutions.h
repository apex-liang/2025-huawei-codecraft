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
inline int disk[MAX_DISK_NUM][MAX_DISK_SIZE];
inline int disk_point[MAX_DISK_NUM];
inline int token[MAX_DISK_NUM];
inline int block_num[MAX_DISK_NUM][MAX_DISK_SIZE];
inline int successful_request[MAX_REQUEST_NUM];
inline int q[MAX_REQUEST_NUM], tt = -1, hh = 0;
inline bool is_drump[MAX_DISK_NUM];
inline std::pair<int, int> work_object[MAX_DISK_NUM];
// inline int pre_token[MAX_DISK_NUM];

#define SPACE_NONE_LABEL 0 // 三片空闲区域
#define TEST_BLOCK_DIVIDE 32
inline std::pair<int, int> label_range[MAX_DISK_NUM][MAX_LABEL_NUM + SPACE_NONE_LABEL];
inline std::set<int> free_blocks[MAX_DISK_NUM][MAX_LABEL_NUM + 2]; // 多分配几个，在磁盘i中扇区j还没有被用的小块块号，记录还没用的，使用set是因为从前到后插空存储。磁盘是被分成了已分配的和未分配的
inline std::set<int> free_space[MAX_DISK_NUM];
inline std::pair<int, int> last_pos[MAX_LABEL_NUM];

// 需求相关
inline int write_demand[MAX_STMP][MAX_LABEL_NUM];  // 写入需求：时间段 X 内标签 M 的写入量
inline int read_demand[MAX_STMP][MAX_LABEL_NUM];   // 读取需求：时间段 X 内标签 M 的读取量
inline int delete_demand[MAX_STMP][MAX_LABEL_NUM]; // 删除需求：时间段 X 内标签 M 的删除量
inline long int demand[MAX_LABEL_NUM][MAX_STMP] = {0};//记录了每个label每个时间段的存储需求
inline std::pair<long int, long int> max_demand[MAX_LABEL_NUM];//按照最大存储需求进行排名，每个元素为 最大存储需求、对应标签

// 分配逻辑相关
// inline int allocated_space[MAX_DISK_NUM][MAX_LABEL_NUM];
inline std::vector<std::pair<int, std::pair<int, int>>> disk_label[MAX_DISK_NUM]; // disk存储的LABEL，每个vector的含义为每个磁盘依次存储的Label,{begin end} 
inline std::unordered_map<int, int> range_begin[MAX_LABEL_NUM];                  // 存储标签M的disk_id和block_id
inline std::vector<int> label_disk[MAX_LABEL_NUM];                               // LABEL存储的disk_id,优先值
inline std::vector<int> label_disk_2[MAX_LABEL_NUM];                             // 备份2对应
inline bool st[MAX_REQUEST_NUM];
inline int max_demand_all = 0;
inline int max_have_data_pos[MAX_DISK_NUM][3];

// 写入相关
inline int pre_token_disk[MAX_DISK_NUM];


// 统计与进度
inline int ok_all;
inline int request_all;
inline int delect_all;
inline int object_all;
inline std::pair<std::pair<int, int>, int> disk_point_range[50][MAX_DISK_NUM];//磁针优化，时间段i内磁盘j的磁针动态区间

// 热度与排名
inline long long hot_company[MAX_LABEL_NUM][MAX_LABEL_NUM];
inline std::unordered_map<int, int> replica_3_label;
inline std::pair<long long, int> hot_rank[MAX_LABEL_NUM][MAX_LABEL_NUM];//读取最大近似
inline bool allocation_st[MAX_LABEL_NUM];
inline int disk_hot[11][50];
inline std::pair<long long, int> hot_disk_rank[MAX_DISK_NUM][MAX_DISK_NUM];
inline long long hot_disk_company[MAX_DISK_NUM][MAX_DISK_NUM];
inline bool allocation_disk_st[MAX_DISK_NUM];
inline std::pair<int, int> label_replica_disk[MAX_LABEL_NUM][12];
inline int label_top_time[MAX_LABEL_NUM];
inline int disk_begin[MAX_DISK_NUM];                                        //记录当前disk的起始值，用于标记当前disk的块已经分配/读写到哪个地方，后面的都是没被分配的

// 计数与策略参数
inline int cnt_in = 0;
inline int cnt_max = 5;                                                     //
inline int hot_disk_label_company[MAX_DISK_NUM][MAX_LABEL_NUM];
inline std::pair<long long, int> hot_disk_label_rank[MAX_DISK_NUM][MAX_LABEL_NUM];
inline int allocation_disk_label_st[MAX_LABEL_NUM];
inline int time_stemp = 0;


inline bool cmp(int request_a,int request_b){
	int object_id_a=request[request_a].object_id,object_id_b=request[request_b].object_id;
	return object[object_id_b].size<object[object_id_a].size;
}
