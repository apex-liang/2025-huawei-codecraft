#include"solutions.h"
void do_object_write_my(int object_id, int size, int label) {
    int current_replica_index = 0;
    int current_write_point=0;
    unordered_set<int> exist;
    set<pair<int,int>>::iterator it;
    for(int i=0;i<label_block[label].size();i++)
    {
        if(i==0) it=label_block[label].begin();
        if (current_replica_index >= 1) break;
        int disk_id=it->first;
        int block_id=it->second;
        if(disk_label_able[disk_id][block_id].size()>=size&&find(exist.begin(),exist.end(),disk_id)==exist.end())
        {
            object[object_id].replica[++current_replica_index] = disk_id;  // 选中该磁盘
            exist.insert(disk_id);
            int current_write_point = 0;
			int block = 0;
            while (!disk_label_able[disk_id][block_id].empty() && current_write_point < size) {
                    block = *(disk_label_able[disk_id][block_id].begin());
                    disk_label_able[disk_id][block_id].erase(disk_label_able[disk_id][block_id].begin());  // 分配块
                    if (disk[disk_id][block] == 0){
                        disk[disk_id][block] = object_id;         // 写入数据
                        object[object_id].unit[current_replica_index][++current_write_point] = block;
                        block_num[disk_id][block]=current_write_point;
                    }
                    
                }
        }
        it++;
    }

    for(int x=object_id;x<object_id+N;x++)
    {
        if (current_replica_index >= 1) break;
        int disk_id=x%N+1;
        if(find(exist.begin(),exist.end(),disk_id)==exist.end()&&free_space_block[disk_id].size()>0){
            int block_id=*(free_space_block[disk_id].begin());
            free_space_block[disk_id].erase(free_space_block[disk_id].begin());
            label_block[label].insert({disk_id,block_id});
            disk_block[disk_id][block_id]=label;
            disk_block_time[disk_id][block_id] = time_stemp;
            object[object_id].replica[++current_replica_index] = disk_id;  // 选中该磁盘
            exist.insert(disk_id);
            int current_write_point = 0;
            int block = 0;
            while (!disk_label_able[disk_id][block_id].empty() && current_write_point < size) {
                    block = *(disk_label_able[disk_id][block_id].begin());
                    disk_label_able[disk_id][block_id].erase(disk_label_able[disk_id][block_id].begin());  // 分配块
                    if (disk[disk_id][block] == 0){
                        disk[disk_id][block] = object_id;         // 写入数据
                        object[object_id].unit[current_replica_index][++current_write_point] = block;
                        block_num[disk_id][block]=current_write_point;
                    }  
            }

        }
    }
    
    for(;current_replica_index<1;){
    
        int max_size=0;
        pair<int,int> pos_tmp;
        for(int disk_id=1;disk_id<=N;disk_id++)
        {
            for(int block_id=1;block_id<=disk_block_num;block_id++)
            {
                if((disk_label_able[disk_id][block_id].size()>max_size)&&find(exist.begin(),exist.end(),disk_id)==exist.end() &&time_stemp > 5 + disk_block_time[disk_id][block_id])
                {
                    max_size=disk_label_able[disk_id][block_id].size();
                    pos_tmp={disk_id,block_id};
                }
            }
        }
        int disk_id=pos_tmp.first;
        int block_id=pos_tmp.second;
        label_block[disk_block[disk_id][block_id]].erase(find(label_block[disk_block[disk_id][block_id]].begin(),label_block[disk_block[disk_id][block_id]].end(),pos_tmp));
        label_block[label].insert(pos_tmp);
        disk_block_time[disk_id][block_id] = time_stemp;
        disk_block[disk_id][block_id]=label;
        object[object_id].replica[++current_replica_index] = disk_id;  // 选中该磁盘
        exist.insert(disk_id);
        int current_write_point = 0;
        int block = 0;
        if(max_size<5) printf("warning ,isnt enough");
        while (!disk_label_able[disk_id][block_id].empty() && current_write_point < size) {
                block = *(disk_label_able[disk_id][block_id].begin());
                disk_label_able[disk_id][block_id].erase(disk_label_able[disk_id][block_id].begin());  // 分配块
                if (disk[disk_id][block] == 0){
                    disk[disk_id][block] = object_id;         // 写入数据
                    object[object_id].unit[current_replica_index][++current_write_point] = block;
                    block_num[disk_id][block]=current_write_point;
                }
                
            }
    }


    for(int x=object_id;x<object_id+N;x++)
    {
        if (current_replica_index >= 3) break;
        int disk_id=x%N+1;
        if(free_space[disk_id].size()>size&&find(exist.begin(),exist.end(),disk_id)==exist.end())
        {
            object[object_id].replica[++current_replica_index] = disk_id;  // 选中该磁盘
            exist.insert(disk_id);
            int current_write_point = 0;
            int block = 0;
            while (!free_space[disk_id].empty() && current_write_point < size) {
                    block = *(free_space[disk_id].begin());
                    free_space[disk_id].erase(free_space[disk_id].begin());  // 分配块
                    if (disk[disk_id][block] == 0){
                        disk[disk_id][block] = object_id;         // 写入数据
                        object[object_id].unit[current_replica_index][++current_write_point] = block;
                        block_num[disk_id][block]=current_write_point;
                    }  
            }
        }
        
    }

    // 如果前述未找到空间，则扩展到全磁盘

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