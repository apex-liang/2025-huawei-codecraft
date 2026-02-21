#include"solutions.h"
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
    int current_replica_index = 0;
    int current_write_point=0;
    unordered_set<int> exist;
    for(int x=0;x<label_disk[label].size();x++)//16 1 2
    {
        if (current_replica_index >= 1) break;
        int disk_id=label_disk[label][x];
        int it;
        for(int i = 0 ; i < disk_label[disk_id].size() ;i++ ) 
	    {
	    	it = i;
        	if(label == disk_label[disk_id][i].first) break; 
		}
        
        int block_id=it+1;
        if (free_blocks[disk_id][1].size()>size&&find(exist.begin(),exist.end(),disk_id)==exist.end()) {//有可能出现不够的情况，最好和size比较
            object[object_id].replica[++current_replica_index] = disk_id;  // 选中该磁盘
            exist.insert(disk_id);
            last_pos[label].first=disk_id;
            last_pos[label].second=block_id;
            int current_write_point = 0;
			int block = 0;
            while (!free_blocks[disk_id][1].empty() && current_write_point < size) {
                if(block_id==1){//Label A从前往后存
                    block = *(free_blocks[disk_id][1].begin());
                    free_blocks[disk_id][1].erase(free_blocks[disk_id][1].begin());  // 分配块
                    if (disk[disk_id][block] == 0){
                    disk[disk_id][block] = object_id;         // 写入数据
                    object[object_id].unit[current_replica_index][++current_write_point] = block;
                    block_num[disk_id][block]=current_write_point;
                    }
                    max_have_data_pos[disk_id][1] = max(max_have_data_pos[disk_id][1], block);
                }
                else if(block_id==2)
                {//Label B从后往前存
                    block = *(free_blocks[disk_id][1].rbegin());
                    free_blocks[disk_id][1].erase(find(free_blocks[disk_id][1].begin(),free_blocks[disk_id][1].end(),block));  // 分配块
                    if (disk[disk_id][block] == 0){
                    disk[disk_id][block] = object_id;         // 写入数据
                    object[object_id].unit[current_replica_index][++current_write_point] = block;
                    block_num[disk_id][block]=current_write_point;
                    // disk_label[disk_id][1].second.first=min(block,disk_label[disk_id][1].second.first);
                    }
                    min_have_data_pos[disk_id] = min(min_have_data_pos[disk_id], block);
                }
                
                
                // if (current_write_point < size) {
                //     int partition_size = V / TEST_BLOCK_DIVIDE; 
                //     for (int i = 1; i <= V; i++) {
                //         if (disk[disk_id][i] == 0) {
                //             disk[disk_id][i] = object_id;// 这个会影响另一个的空间
                //             int tmp=(i-1)/partition_size+1;
                //             set<int>::iterator it=find(free_blocks[disk_id][tmp].begin(),free_blocks[disk_id][tmp].end(),i);
                //             free_blocks[disk_id][tmp].erase(it);
                //             object[object_id].unit[current_replica_index][++current_write_point] = i;
                //             block_num[disk_id][i]=current_write_point;
                //             if (current_write_point == size)
                //             {
                //                 max_have_data_pos[disk_id][block_id-1] = i;
                //                 break;
                //             }
                //         }
                //     }
                // }
        }
        }
    }
    
    //备份二存储
    for(int x=0;x<label_disk_2[label].size();x++)
    {
        if (current_replica_index >= 2) break;
        int disk_id=label_disk_2[label][x];
        int block_id=2;
        if (free_blocks[disk_id][block_id].size()>size&&find(exist.begin(),exist.end(),disk_id)==exist.end()) {//有可能出现不够的情况，最好和size比较
            object[object_id].replica[++current_replica_index] = disk_id;  // 选中该磁盘
            exist.insert(disk_id);
            last_pos[label].first=disk_id;
            last_pos[label].second=block_id;
            int current_write_point = 0;
			int block = 0;
            while (!free_blocks[disk_id][block_id].empty() && current_write_point < size) {
                block = *(free_blocks[disk_id][block_id].begin());
                free_blocks[disk_id][block_id].erase(free_blocks[disk_id][block_id].begin());  // 分配块
                if (disk[disk_id][block] == 0){
                disk[disk_id][block] = object_id;         // 写入数据
                object[object_id].unit[current_replica_index][++current_write_point] = block;
                block_num[disk_id][block]=current_write_point;
                }
            }
            max_have_data_pos[disk_id][block_id-1] = max(max_have_data_pos[disk_id][block_id-1], block);
            if (current_write_point < size) {
                int partition_size = V / TEST_BLOCK_DIVIDE; 
                for (int i = 1; i <= V; i++) {
                    if (disk[disk_id][i] == 0) {
                        disk[disk_id][i] = object_id;// 这个会影响另一个的空间
                        int tmp=(i-1)/partition_size+1;
                        set<int>::iterator it=find(free_blocks[disk_id][tmp].begin(),free_blocks[disk_id][tmp].end(),i);
                        free_blocks[disk_id][tmp].erase(it);
                        object[object_id].unit[current_replica_index][++current_write_point] = i;
                        block_num[disk_id][i]=current_write_point;
                        if (current_write_point == size)
						{
							max_have_data_pos[disk_id][block_id-1] = i;
							break;
						}
                    }
                }
            }
        }
    }

    for (int x = object_id; x <= object_id+N; x++)
    {//哈希存储，存储到未分配区域
            if (current_replica_index >= REP_NUM) break;
            int disk_id=x%N+1;
            // 判断当前磁盘是否有该标签的空闲空间
            if (free_space[disk_id].size()>size&&find(exist.begin(),exist.end(),disk_id)==exist.end()) {//有可能出现不够的情况，最好和size比较
                object[object_id].replica[++current_replica_index] = disk_id;  // 选中该磁盘
                exist.insert(disk_id);
                int current_write_point = 0;
                while (!free_space[disk_id].empty() && current_write_point < size) {
                    int block = *(free_space[disk_id].begin());
                    free_space[disk_id].erase(free_space[disk_id].begin());  // 分配块
                    if (disk[disk_id][block] == 0){
                    disk[disk_id][block] = object_id;         // 写入数据
                    object[object_id].unit[current_replica_index][++current_write_point] = block;
                    block_num[disk_id][block]=current_write_point;
                    }
                }
                if (current_write_point < size) {
                    int partition_size = V /TEST_BLOCK_DIVIDE; 
                    // 如果专属范围不足，扩展到全局空间
                    for (int i = 1; i <= V; i++) {
                        if (disk[disk_id][i] == 0) {
                            disk[disk_id][i] = object_id;// 这个会影响另一个的空间
                            int tmp=(i-1)/partition_size+1;
                            set<int>::iterator it=find(free_blocks[disk_id][tmp].begin(),free_blocks[disk_id][tmp].end(),i);
                            free_blocks[disk_id][tmp].erase(it);
                            object[object_id].unit[current_replica_index][++current_write_point] = i;
                            block_num[disk_id][i]=current_write_point;
                            if (current_write_point == size) break;
                        }
                    }
                }
            }
    }
    for (int x = object_id; x < object_id+N; x++)
    {//实在存不下允许与备份一混存，避免出现跑不通的情况
        
        if (current_replica_index >= REP_NUM) break;
        int disk_id=x%N+1;
            for(int j=1;j<=2;j++)
            {
                if (current_replica_index >= REP_NUM) break;
                if(free_blocks[disk_id][j].size()>size&&find(exist.begin(),exist.end(),disk_id)==exist.end())
                {
                    object[object_id].replica[++current_replica_index] = disk_id;  // 选中该磁盘
                    exist.insert(disk_id);
                    int current_write_point = 0;
                    while (!free_blocks[disk_id][j].empty() && current_write_point < size) {
                        int block = *(free_blocks[disk_id][j].rbegin());
                        free_blocks[disk_id][j].erase(find(free_blocks[disk_id][j].begin(),free_blocks[disk_id][j].end(),block));  // 分配块
                        if (disk[disk_id][block] == 0){
                        disk[disk_id][block] = object_id;         // 写入数据
                        object[object_id].unit[current_replica_index][++current_write_point] = block;
                        block_num[disk_id][block]=current_write_point;
                        }
                    }
                    if (current_write_point < size) {
                    int partition_size = V /TEST_BLOCK_DIVIDE; 
                    // 如果专属范围不足，扩展到全局空间
                    for (int i = 1; i <= V; i++) {
                        if (disk[disk_id][i] == 0) {
                            disk[disk_id][i] = object_id;// 这个会影响另一个的空间
                            int tmp=(i-1)/partition_size+1;
                            set<int>::iterator it=find(free_blocks[disk_id][tmp].begin(),free_blocks[disk_id][tmp].end(),i);
                            free_blocks[disk_id][tmp].erase(it);
                            object[object_id].unit[current_replica_index][++current_write_point] = i;
                            block_num[disk_id][i]=current_write_point;
                            if (current_write_point == size) break;
                        }
                    }
                }
                }
            }
    }


    exist.clear();
}


void write_action()
{
    int n_write;
    scanf("%d", &n_write);
    object_all += n_write;
    for (int i = 1; i <= n_write; i++) {
        int id, size, label;
        scanf("%d%d%d", &id, &size,&label);
        object[id].last_request_point = 0;
        object[id].size = size;
        object[id].label = label;
        object[id].is_delete = false;
        for (int j = 1; j <= REP_NUM; j++) {
            object[id].unit[j] = static_cast<int*>(malloc(sizeof(int) * (size + 1)));
        }
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