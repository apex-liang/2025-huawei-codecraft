#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <tuple>
#include <iostream>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

#define MAX_DISK_NUM (10 + 1)
#define MAX_DISK_SIZE (16384 + 1)
#define MAX_REQUEST_NUM (30000000 + 1)
#define MAX_OBJECT_NUM (100000 + 1)
#define REP_NUM (3)
#define FRE_PER_SLICING (1800)
#define EXTRA_TIME (105)
#define MAX_TIME_SLICING (86400 + EXTRA_TIME + 1)
#define MAX_STMP (48+1)
#define disk_block_num 48//16
#define MAX_LABEL_NUM (16+1)
#define BIT_NUM 120

typedef struct Request_ {
    int object_id;
    int prev_id;
    bool is_done;
    bitset<BIT_NUM> state;
    int t;

} Request;

typedef struct Object_ {
    int replica[REP_NUM + 1];
    int* unit[REP_NUM + 1];
    int label;
    int size;
    int last_request_point;
    bool is_delete;
    int delete_time;
    int write_time;
    int is_predict;
    int write_time_2;
} Object;

typedef struct Block_{  
    int all;
    int start;
    int middle;
    int end;
}block;

inline block block_information[MAX_DISK_NUM][MAX_STMP];  
inline Request request[MAX_REQUEST_NUM];
inline Object object[MAX_OBJECT_NUM];

inline int T, M, N, V, G, K;
inline int disk[MAX_DISK_NUM][MAX_DISK_SIZE];
// int disk_point[MAX_DISK_NUM];
inline int disk_point[MAX_DISK_NUM][3];
inline int pre_token_disk[MAX_DISK_NUM][3];
inline int n_deletes[MAX_TIME_SLICING];
inline int n_writes[MAX_TIME_SLICING];
inline int n_reads[MAX_TIME_SLICING];
inline vector<int> delete_datas[MAX_TIME_SLICING];
inline vector<tuple<int, int, int>> write_datas[MAX_TIME_SLICING];
inline vector<tuple<int, int>> read_datas[MAX_TIME_SLICING];

inline int block_size;
inline int time_stemp=0;
inline set<int> free_space_block[MAX_DISK_NUM];
inline set<pair<int,int>> label_block[MAX_LABEL_NUM];
inline set<int> disk_label_able[MAX_DISK_NUM][disk_block_num+1];
inline int block_num[MAX_DISK_NUM][MAX_DISK_SIZE];
inline int disk_block[MAX_DISK_NUM][disk_block_num+1];
inline int disk_block_time[MAX_DISK_NUM][disk_block_num+1];
inline set<int> free_space[MAX_DISK_NUM];
inline int read_map[MAX_DISK_NUM][MAX_DISK_SIZE];

inline set<int> label_need_read;
inline int req_p1[MAX_DISK_NUM];
inline int req_p2[MAX_DISK_NUM];
inline int k_new=-100;
inline int k_new_2=3500;
inline int disk_point_mid[MAX_DISK_NUM];
inline int disk_begin[MAX_DISK_NUM];
inline int request_all;
inline int i_tmp=0;
inline int read_map_all[MAX_DISK_NUM][MAX_DISK_SIZE];
// int pre_token_disk[MAX_DISK_NUM];
// int pre_token_disk_2[MAX_DISK_NUM];
inline bool request_not_finish[MAX_REQUEST_NUM];
inline int successful_request[MAX_REQUEST_NUM];
// int disk_point_2[MAX_DISK_NUM];
inline int object_hot_datas[100010][49];
inline int last_object;
inline int read_demand_all[17];
inline int object_hot_label[100010][17];
inline int object_wanfenbi[100010][49];
inline int read_demand[17][50];
inline int delete_demand[17][50];
inline int write_demand[17][50];
inline long int demand[MAX_LABEL_NUM][MAX_STMP]={0};

inline int time_stamp_all;
inline int max_demand_beifen[MAX_LABEL_NUM];
inline int demand_all_label[49];
inline int preallocation_time;
inline pair<long int,long int> max_demand[MAX_LABEL_NUM];
inline int max_demand_all_label;
inline int xjb_company[MAX_LABEL_NUM][MAX_LABEL_NUM];
inline pair<int ,int> xjb_rank[MAX_LABEL_NUM][MAX_LABEL_NUM];

inline int read_sum[MAX_STMP];
inline double xjb[MAX_LABEL_NUM];
inline double xjb_avr;
inline int read_demand_all_label=0;
inline set<int> label_need_read_time[50];
inline int preallocation_label[MAX_LABEL_NUM];
inline int read_demand_round_1[MAX_LABEL_NUM];
inline int label_exist[MAX_LABEL_NUM];
inline unordered_set<int> exist;
inline int xjb_before[MAX_LABEL_NUM];
inline int read_demand_all_tem[17];

void delete_action(int round, int time);
void write_action(int round, int time);
void read_action(int round, int time);
void gc_action(int round);