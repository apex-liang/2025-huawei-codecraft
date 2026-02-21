#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <queue>
#include <vector>
#include <list>
#include <deque>
#include <cstring>
#include <unordered_set>
#include <bitset>
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

Request request[MAX_REQUEST_NUM];
Object object[MAX_OBJECT_NUM];

int T, M, N, V, G;
int disk[MAX_DISK_NUM][MAX_DISK_SIZE];
int disk_point[MAX_DISK_NUM];
int token[MAX_DISK_NUM];
int block_num[MAX_DISK_NUM][MAX_DISK_SIZE];
int successful_request[MAX_REQUEST_NUM];
int q[MAX_REQUEST_NUM],tt=-1,hh=0;
bool is_drump[MAX_DISK_NUM];
pair<int,int> work_object[MAX_DISK_NUM];
//int pre_token[MAX_DISK_NUM];
#define SPACE_NONE_LABEL 4//x片空闲区域
pair<int,int> label_range[MAX_DISK_NUM][MAX_LABEL_NUM+SPACE_NONE_LABEL];
deque<int> free_blocks[MAX_DISK_NUM][MAX_LABEL_NUM+SPACE_NONE_LABEL];//多分配几个

void timestamp_action()
{
    int timestamp;
    scanf("%*s%d", &timestamp);
    printf("TIMESTAMP %d\n", timestamp);

    fflush(stdout);
}
void do_object_delete(const int* object_unit, int* disk_unit, int size,int object_id,int disk_id)
{
    int partition_size = V / (M+SPACE_NONE_LABEL); 
    for (int i = 1; i <= size; i++) {
        disk_unit[object_unit[i]] = 0;
        int label = (object_unit[i]-1)/partition_size+1;  // 获取标签
        free_blocks[disk_id][label].push_back(object_unit[i]);  // 更新空闲块记录
        block_num[disk_id][object_unit[i]]=0;
    }
}

void delete_action()
{
    int n_delete;
    int abort_num = 0;
    static int _id[MAX_OBJECT_NUM];

    scanf("%d", &n_delete);
    for (int i = 1; i <= n_delete; i++) {
        scanf("%d", &_id[i]);
    }

    for (int i = 1; i <= n_delete; i++) {
        int id = _id[i];
        int current_id = object[id].last_request_point;
        while (current_id != 0) {
            if (request[current_id].is_done == false) {
                abort_num++;
            }
            current_id = request[current_id].prev_id;
        }
    }

    printf("%d\n", abort_num);
    for (int i = 1; i <= n_delete; i++) {
        int id = _id[i];
        int current_id = object[id].last_request_point;
        while (current_id != 0) {
            if (request[current_id].is_done == false) {
                printf("%d\n", current_id);
            }
            current_id = request[current_id].prev_id;
        }
        for (int j = 1; j <= REP_NUM; j++) {
            do_object_delete(object[id].unit[j], disk[object[id].replica[j]], object[id].size,id,object[id].replica[j]);
        }
        object[id].is_delete = true;
    }

    fflush(stdout);
}
// void do_object_delete(const int* object_unit, int* disk_unit, int size)
// {
//     for (int i = 1; i <= size; i++) {
//         disk_unit[object_unit[i]] = 0;
//     }
// }

// void delete_action()
// {
//     int n_delete;
//     int abort_num = 0;
//     static int _id[MAX_OBJECT_NUM];

//     scanf("%d", &n_delete);
//     for (int i = 1; i <= n_delete; i++) {
//         scanf("%d", &_id[i]);
//     }

//     for (int i = 1; i <= n_delete; i++) {
//         int id = _id[i];
//         int current_id = object[id].last_request_point;
//         while (current_id != 0) {
//             if (request[current_id].is_done == false) {
//                 abort_num++;
//             }
//             current_id = request[current_id].prev_id;
//         }
//     }

//     printf("%d\n", abort_num);
//     for (int i = 1; i <= n_delete; i++) {
//         int id = _id[i];
//         int current_id = object[id].last_request_point;
//         while (current_id != 0) {
//             if (request[current_id].is_done == false) {
//                 printf("%d\n", current_id);
//             }
//             current_id = request[current_id].prev_id;
//         }
//         for (int j = 1; j <= REP_NUM; j++) {
//             do_object_delete(object[id].unit[j], disk[object[id].replica[j]], object[id].size);
//         }
//         object[id].is_delete = true;
//     }

//     fflush(stdout);
// }

void do_object_write(int* object_unit, int* disk_unit, int size, int object_id,int replica,int label) {
    
    int hash_seed = (label-1)*V/M;
    int start_position = hash_seed % V; 
    
    int current_write_point = 0;
    for (int i = 1; i <= V; i++) {
        int pos = (start_position + i - 1) % V + 1;
        if (disk_unit[pos] == 0) {
            disk_unit[pos] = object_id;
            object_unit[++current_write_point] = pos;
            block_num[replica][pos]=current_write_point;
            if (current_write_point == size) break;
        }
    }
    assert(current_write_point == size);
}
void do_object_write_my(int object_id, int size, int label) {
    // int replicas[REP_NUM];  // 副本磁盘
    int current_replica_index = 0;
    int current_write_point=0;
    unordered_set<int> exist;
    for (int x = label; x <= label+N-1; x++) {
        if (current_replica_index >= REP_NUM) break;
        int disk_id=x%N+1;
        // 判断当前磁盘是否有该标签的空闲空间
        if (free_blocks[disk_id][label].size()>size&&find(exist.begin(),exist.end(),disk_id)==exist.end()) {//有可能出现不够的情况，最好和size比较
            object[object_id].replica[++current_replica_index] = disk_id;  // 选中该磁盘
            exist.insert(disk_id);
            // printf("disk %d label %d is not empty!",disk_id,label);
            // 从标签分区范围内分配连续块
            int current_write_point = 0;
            while (!free_blocks[disk_id][label].empty() && current_write_point < size) {
                int block = free_blocks[disk_id][label].front();
                free_blocks[disk_id][label].pop_front();  // 分配块
                if (disk[disk_id][block] == 0){
                disk[disk_id][block] = object_id;         // 写入数据
                object[object_id].unit[current_replica_index][++current_write_point] = block;
                block_num[disk_id][block]=current_write_point;
                }
            }
            // printf("write OK",current_replica_index);
            if (current_write_point < size) {
                int partition_size = V / (M+SPACE_NONE_LABEL); 
                // 如果专属范围不足，扩展到全局空间
                for (int i = 1; i <= V; i++) {
                    if (disk[disk_id][i] == 0) {
                        disk[disk_id][i] = object_id;// 这个会影响另一个的空间
                        int tmp=(i-1)/partition_size+1;
                        deque<int>::iterator it=find(free_blocks[disk_id][tmp].begin(),free_blocks[disk_id][tmp].end(),i);
                        free_blocks[disk_id][tmp].erase(it);
                        object[object_id].unit[current_replica_index][++current_write_point] = i;
                        block_num[disk_id][i]=current_write_point;
                        if (current_write_point == size) break;
                    }
                }
            }
        }
            


    }
    // for(int i=current_replica_index+1;i<=REP_NUM;i++)//没找到规律的位置，乱写
    // {
    //     // printf("YES!");

        for(int x=(label);x<=(label+N-1);x++)//选磁盘，这里写入的是一些空余位置
        {
            if (current_replica_index >= REP_NUM) break;
            int disk_id_tmp=x%N+1;
            if(find(exist.begin(),exist.end(),disk_id_tmp)==exist.end())
            {
                
                int space_choose=M+1;
                bool is_able=false;
                for(;space_choose<=M+SPACE_NONE_LABEL;space_choose++)
                {
                    if(free_blocks[disk_id_tmp][space_choose].size()>size)
                    {
                        is_able=true;
                       break; 
                    }
                        
                }
                if(!is_able) continue;
                exist.insert(disk_id_tmp);
                object[object_id].replica[++current_replica_index]=disk_id_tmp;
                // for (int j = 1; j <= V; j++) {
                // int j_tmp=(label_range[disk_id_tmp][space_choose].second+j-1)%V+1;
                // if (disk[disk_id_tmp][j] == 0) {
                //     disk[disk_id_tmp][j] = object_id;
                //     free_blocks
                //     object[object_id].unit[current_replica_index][++current_write_point] = j;
                //     block_num[disk_id_tmp][j]=current_write_point;
                //     if (current_write_point == size) break;
                //     }
                // }  
                int current_write_point = 0;
                while (!free_blocks[disk_id_tmp][space_choose].empty() && current_write_point < size) {
                    // printf("OK");
                    int block = free_blocks[disk_id_tmp][space_choose].front();
                    free_blocks[disk_id_tmp][space_choose].pop_front();  // 分配块
                    if (disk[disk_id_tmp][block] == 0){
                    disk[disk_id_tmp][block] = object_id;         // 写入数据
                    object[object_id].unit[current_replica_index][++current_write_point] = block;
                    block_num[disk_id_tmp][block]=current_write_point;
                    }
                }
            }
             
        }




    for(int x=(label+object_id);x<=(label+object_id+N-1);x++)
        {
            if (current_replica_index >= REP_NUM) break;
            int disk_id_tmp=x%N+1;
            if(find(exist.begin(),exist.end(),disk_id_tmp)==exist.end())
            {
                int current_write_point=0;
                exist.insert(disk_id_tmp);
                object[object_id].replica[++current_replica_index]=disk_id_tmp;
                for (int j = 1; j <= V; j++) {
                if (disk[disk_id_tmp][j] == 0) {
                    disk[disk_id_tmp][j] = object_id;
                    object[object_id].unit[current_replica_index][++current_write_point] = j;
                    block_num[disk_id_tmp][j]=current_write_point;
                    if (current_write_point == size) break;
                    }
                }  
            }
             
        }
    
    // }
    exist.clear();
    // // 确保副本分布均匀
    // for (int i = current_replica_index; i <= REP_NUM; i++) {
    //     // 选择其他磁盘写入副本
    //     int remaining_size = size;
    //     for (int disk_id = 1; disk_id <= N; disk_id++) {
    //         if (!free_blocks[disk_id][label].empty() && remaining_size > 0 && disk_id!=object[object_id].replica[1] &&disk_id!=object[object_id].replica[2]) {
    //             object[object_id].replica[i] = disk_id;
    //             int current_write_point = 0;
    //             while (!free_blocks[disk_id][label].empty() && current_write_point < remaining_size) {
    //                 int block = free_blocks[disk_id][label].front();
    //                 free_blocks[disk_id][label].pop_front();
    //                 disk[disk_id][block] = object_id;
    //                 object[object_id].unit[i][++current_write_point] = block;
    //             }
    //             remaining_size -= current_write_point;
    //         }
    //     }
    // }

    // 如果上面没有分到位置,就无所谓位置随便存个地方
    // for(int i=1;i<=N;i++)
    // {

    // }
    // assert(current_write_point == size);
}


void write_action()
{
    int n_write;
    scanf("%d", &n_write);
    for (int i = 1; i <= n_write; i++) {
        int id, size, label;
        scanf("%d%d%d", &id, &size,&label);
        // printf("read OK");
        object[id].last_request_point = 0;
        object[id].size = size;
        object[id].label = label;
        object[id].is_delete = false;
        for (int j = 1; j <= REP_NUM; j++) {
            //object[id].replica[j] = (id + j) % N + 1;
            object[id].unit[j] = static_cast<int*>(malloc(sizeof(int) * (size + 1)));
            
            // do_object_write(object[id].unit[j], disk[object[id].replica[j]], size, id,object[id].replica[j],label);
            // do_object_write_my(id,size,label,N,V);
        }
        // printf("OK!");
        // object[id].size = size;
        // object[id].label = label;
        // object[id].is_delete = false;
        do_object_write_my(id,size,label);


        printf("%d\n", id);
        for (int j = 1; j <= REP_NUM; j++) {
            printf("%d", object[id].replica[j]);
            for (int k = 1; k <= size; k++) {
                printf(" %d", object[id].unit[j][k]);
            }
            printf("\n");
        }
    }

    fflush(stdout);
}

// void write_action()
// {
//     int n_write;
//     scanf("%d", &n_write);
//     for (int i = 1; i <= n_write; i++) {
//         int id, size, label;
//         scanf("%d%d%d", &id, &size,&label);
//         object[id].last_request_point = 0;
//         for (int j = 1; j <= REP_NUM; j++) {
//             object[id].replica[j] = (id + j) % N + 1;
//             object[id].unit[j] = static_cast<int*>(malloc(sizeof(int) * (size + 1)));
//             object[id].size = size;
//             object[id].label = label;
//             object[id].is_delete = false;
//             do_object_write(object[id].unit[j], disk[object[id].replica[j]], size, id,object[id].replica[j],label);
//         }

//         printf("%d\n", id);
//         for (int j = 1; j <= REP_NUM; j++) {
//             printf("%d", object[id].replica[j]);
//             for (int k = 1; k <= size; k++) {
//                 printf(" %d", object[id].unit[j][k]);
//             }
//             printf("\n");
//         }
//     }

//     fflush(stdout);
// }
// bool cmp(int request_a,int request_b){
// 	int object_id_a=request[request_a].object_id,object_id_b=request[request_b].object_id;
// 	return object[object_id_b].label<object[object_id_a].label;
// }
bool cmp(int request_a, int request_b) {
    int object_id_a = request[request_a].object_id;
    int object_id_b = request[request_b].object_id;

    int replica_a = object[object_id_a].replica[1]; // 副本 1
    int replica_b = object[object_id_b].replica[1];

    int block_a = object[object_id_a].unit[1][1];   // 默认选择第一个块
    int block_b = object[object_id_b].unit[1][1];

    // 确保磁头在块的左侧
    if (disk_point[replica_a] >= block_a) return false; // 请求 A 不满足左侧条件
    if (disk_point[replica_b] >= block_b) return true;  // 请求 B 不满足左侧条件

    // 按距离排序，保证磁头小于块位置
    int distance_a = block_a - disk_point[replica_a];
    int distance_b = block_b - disk_point[replica_b];
    return distance_a < distance_b;
}
void read_action()
{
    int n_read;
    int request_id, object_id;
    scanf("%d", &n_read);
    for (int i = 1; i <= n_read; i++) {
        scanf("%d%d", &request_id, &object_id);
        request[request_id].object_id = object_id;
        request[request_id].prev_id = object[object_id].last_request_point;
        object[object_id].last_request_point = request_id;
        int label = object[object_id].label;
        q[++tt]=request_id;
    }
    if(tt-hh>1100) sort(q+tt-800,q+tt,cmp);
    else if(tt-hh>=0) sort(q+hh,q+tt,cmp);
    int n_resp = 0;
    
if (tt>=hh) {  // Check if the queue has elements
    bool done = false;  // Flag to exit loops if the queue becomes empty
    for (int i = 1; i <= N && !done; i++) {  // Exit outer loop if done
        for (int j = 1; j <= N && !done; j++) {  // Exit inner loop if done
            int current_id = q[hh];  // Get the front of the queue
            if (request[current_id].is_done) continue;  // Skip if request is done
            
            int object_id = request[current_id].object_id;
            for (int k = 1; k <= REP_NUM; k++) {
                if (object[object_id].replica[k] == j && work_object[j].first == 0) {
                    work_object[j] = {current_id, k};  // Assign work object
                    hh++; // Remove the processed request from the queue
                    
                    if (tt<hh) {  // If the queue is empty, exit all loops
                        done = true;  // Set flag to exit all loops
                        break;  // Exit the k-loop
                    }
                    break;  // Exit the k-loop
                }
            }
        }
    }
}
	for(int i=1;i<=N;i++){
		int object_id=request[work_object[i].first].object_id,replica=work_object[i].second;
		//int label=object[object_id].label;
		// if((disk_point[i]-object[object_id].unit[replica][1])<10)
        //     is_drump[i]=true;


        // if(!is_drump[i]&&work_object[i].first!=0){
		// 	printf("j %d\n",object[object_id].unit[replica][1]);
		// 	disk_point[i]=object[object_id].unit[replica][1];
		// 	is_drump[i]=true;
		// }
		// else{
			int pre_token=0;
			int current_token=G;
			while(current_token>0){
				int current_unit=disk[i][disk_point[i]];
				if(current_unit){
					int t=max(16,pre_token*4/5+1);
					if(!pre_token||pre_token==1) current_token-=64,pre_token=64;
                    else current_token-=t,pre_token=t;
                    if(current_token>0)
					{
					printf("r");
                    int current_id=object[current_unit].last_request_point;
                    int cnt=0;
                    while(current_id != 0&&cnt<20){
                        int block_id=block_num[i][disk_point[i]];
                        request[current_id].state[block_id]=true;
                        if(request[current_id].is_done==false&&request[current_id].state.count()==object[request[current_id].object_id].size){
            
                        successful_request[++n_resp]=current_id;
                        request[current_id].is_done=true;
                        }
                        current_id=request[current_id].prev_id;
                        cnt++;
                    }
                    if(request[work_object[i].first].is_done==true){
                    	work_object[i].first=0;
                    	is_drump[i]=false;
					}
                    disk_point[i]=(disk_point[i]+1)%V;
                }
				}
				else{
					if(current_token>=1){
                    printf("p");
                    current_token-=1;
                    pre_token=1;
                    disk_point[i]=(disk_point[i]+1)%V;}
				}
			}
			printf("#\n");
        // }
    }
            printf("%d\n",n_resp);
            for(int j=1;j<=n_resp;j++) printf("%d\n",successful_request[j]);
    fflush(stdout);
}
// void read_action() {
//     int n_read; // 要读取的请求数量
//     int request_id, object_id;
//     scanf("%d", &n_read); // 读取请求数量
//     for (int i = 1; i <= n_read; i++) {
//         scanf("%d%d", &request_id, &object_id); // 读取请求ID和对象ID
//         request[request_id].object_id = object_id; // 设置请求的对象ID
//         request[request_id].prev_id = object[object_id].last_request_point; // 设置上一个请求ID
//         object[object_id].last_request_point = request_id; // 更新对象的最近请求ID
//         request[request_id].is_done = false; // 标记请求未完成
//     }

//     static int current_request = 0; // 当前处理的请求ID
//     static int current_phase = 1; // 当前请求的阶段
//     if (!current_request && n_read > 0) {
//         current_request = request_id; // 如果没有当前请求，开始处理新请求
//     }
//     if (!current_request) { // 如果当前没有请求
//         for (int i = 1; i <= N; i++) {
//             printf("#\n"); // 输出占位符
//         }
//         printf("0\n"); // 输出结束标志
//     } else { // 处理当前请求
        
//         int head_token=G;
//         object_id = request[current_request].object_id; // 获取当前请求的对象ID
//         for (int i = 1; i <= N; i++) {
//             if (i == object[object_id].replica[1]) { // 如果磁盘是对象的主副本
//                 // if (current_phase % 2 == 1) {
//                 //     printf("j %d\n", object[object_id].unit[1][current_phase / 2 + 1]); // 输出读操作
//                 // } else {
//                 //     printf("r#\n"); // 输出读取信号占位符
//                 // }
//                 int k;
//                 // if(disk_point[i]==V)
//                 //     disk_point[i]=1;
//                 int last=0;//last是上次的p r记录，p是0，r是1
//                 int diff=0;
//                 if(disk_point[i]<=object[object_id].unit[1][current_phase])
//                     diff=object[object_id].unit[1][current_phase]-disk_point[i];
//                 else
//                     diff=V-disk_point[i]+object[object_id].unit[1][current_phase];
//                 if(diff>(G-64))
//                 {
//                     printf("j %d\n", object[object_id].unit[1][current_phase]);
//                     disk_point[i]=object[object_id].unit[1][current_phase];
//                 }
//                 else{
//                     for(int k=1;k<=V;k++)//计数器
//                     {
//                         if((disk_point[i])%V == object[object_id].unit[1][current_phase]&&head_token>=64)
//                         {
//                             printf("r");
//                             current_phase++;
//                             if(last==0) head_token=head_token-64;
//                             else head_token=head_token-64;
//                             last=1;
//                             disk_point[i]=(disk_point[i]+1)%V;
//                             if(current_phase == object[object_id].size+1)
//                             {
//                                 printf("#\n");
//                                 break;
//                             }
//                         //     printf("r#\n");
//                         //     current_phase++; // 增加阶段
//                         //     disk_point[i]=(disk_point[i]+1)%V;
//                         //     break;
//                         }   
//                         else if((disk_point[i])%V == object[object_id].unit[1][current_phase]&&head_token<64)
//                         {   printf("#\n");
//                             break;}   
//                         else if(head_token>0)
//                         {
//                             printf("%c", 'p');
//                             disk_point[i]=(disk_point[i]+1)%V;
//                             head_token--;
//                         } 
//                         else
//                         {
//                             printf("#\n");
//                             break;
//                         }
//                     }
//                 }   
                
//             } else {
//                 printf("#\n"); // 其他磁盘输出占位符
//             }
//         }

//         if (current_phase == object[object_id].size+1 ) { // 如果对象读取完成* 2
//             if (object[object_id].is_delete) {
//                 printf("0\n"); // 如果对象被删除，输出0
//             } else {
//                 printf("1\n%d\n", current_request); // 输出完成信号和请求ID
//                 request[current_request].is_done = true; // 标记请求已完成
//             }
//             current_request = 0; // 重置当前请求
//             current_phase = 1; // 重置阶段
//         } else {
//             printf("0\n"); // 未完成时输出占位符
//         }
//     }

//     fflush(stdout); // 刷新输出缓冲区
// }
void clean()
{
    for (auto& obj : object) {
        for (int i = 1; i <= REP_NUM; i++) {
            if (obj.unit[i] == nullptr)
                continue;
            free(obj.unit[i]);
            obj.unit[i] = nullptr;
        }
    }
}
int write_demand[MAX_STMP][MAX_LABEL_NUM];  // 写入需求：时间段 X 内标签 M 的写入量
int read_demand[MAX_STMP][MAX_LABEL_NUM];   // 读取需求：时间段 X 内标签 M 的读取量
int delete_demand[MAX_STMP][MAX_LABEL_NUM]; // 删除需求：时间段 X 内标签 M 的删除量
int allocated_space[MAX_DISK_NUM][MAX_LABEL_NUM];
void initialize_partitions(int N, int V, int M) {
    for (int disk_id = 1; disk_id <= N; disk_id++) {
        int partition_size = V / (M+SPACE_NONE_LABEL);  // 平均分配给每个标签的空间
        for (int label = 1; label <= M+SPACE_NONE_LABEL; label++) {
            int start = (label - 1) * partition_size + 1;
            int end = label * partition_size;
            label_range[disk_id][label] = {start, end};
            for (int i = start; i <= end; i++) {
                free_blocks[disk_id][label].push_back(i);  // 初始化空闲块
            }
        }
    }
}
int main()
{
    scanf("%d%d%d%d%d", &T, &M, &N, &V, &G);
    initialize_partitions(N,V,M);
    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%*d",delete_demand[j][i]);
        }
    }

    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%*d",write_demand[j][i]);
        }
    }

    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%*d",read_demand[j][i]);
        }
    }

    printf("OK\n");
    fflush(stdout);

    for (int i = 1; i <= N; i++) {
        disk_point[i] = 1;
    }

    for (int t = 1; t <= T + EXTRA_TIME; t++) {

        timestamp_action();
        delete_action();
        write_action();
        read_action();
    }
    clean();
    
    return 0;
}