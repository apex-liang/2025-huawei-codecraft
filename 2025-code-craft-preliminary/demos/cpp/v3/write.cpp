#include"solutions.h"

void do_object_write_my(int object_id, int size, int label) {
    int current_replica_index = 0;
    int current_write_point=0;
    unordered_set<int> exist;//标记了存在的磁盘，因为有三个备份，三个备份不能存在于同一个disk中
    //找位置存备份一，按照之前分配好的label_disk存
    for(int x=0;x<label_disk[label].size();x++)
    {
        if (current_replica_index >= 1) break;
        int disk_id=label_disk[label][x];

        //vector<int>::iterator it=find(disk_label[disk_id].begin(),disk_label[disk_id].end(),label);
        int it;
        for(int i = 0 ; i < disk_label[disk_id].size() ;i++ ) 
	    {
	    	it = i;
        	if(label == disk_label[disk_id][i].first) break; 
		}
        
        int block_id=it+1;
        //如果存的下就存
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
    
    //找位置存备份二，按照之前分配好的label_disk_2存
    for(int x=0;x<label_disk_2[label].size();x++)
    {
        if (current_replica_index >= 2) break;
        int disk_id=label_disk_2[label][x];
        int block_id=3;
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
    {//备份2没存下的解决方案
            if (current_replica_index >= REP_NUM-1) break;
            int disk_id=x%N+1;
            // 判断当前哈希磁盘是否有该标签的空闲空间
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
    //备份三有效存储可以去找备份一写过了的区域，如果存储峰值已经过了，就可以存进去避免浪费空间
    for(int current_replica_3=1;current_replica_3<=cnt_max;current_replica_3++)
    {
        if (current_replica_index >= REP_NUM) break;
        int disk_id_replica_3=label_replica_disk[label][current_replica_3].first;
        int block_id_replica_3=label_replica_disk[label][current_replica_3].second;
        int label_before=disk_label[disk_id_replica_3][block_id_replica_3].first;

        int label_replica_3_before_time=label_top_time[label_before];
        int size_replica_3;
        for(int i=time_stemp;i<=48;i++)
        {
            size_replica_3=min(free_blocks[disk_id_replica_3][block_id_replica_3+1].size() , static_cast<size_t>( free_blocks[disk_id_replica_3][block_id_replica_3+1].size()+demand[label_before][i]-demand[label_before][i+1])) ;
            size_replica_3-=10;
        }
        /*ofstream outFile;
        outFile.open("./example_log.txt", std::ios::out | std::ios::app); // 以追加模式打开
        outFile << " disk_id:" << disk_id_replica_3 << " block_id:" << block_id_replica_3 << " size_replica_3:" << size_replica_3 << " time:" << time_stemp  <<  (time_stemp>label_replica_3_before_time) << (size<=size_replica_3) << (find(exist.begin(),exist.end(),disk_id_replica_3)==exist.end()) << endl;
        outFile.close();*/
        if((time_stemp>label_replica_3_before_time)&&(size<=size_replica_3)&&find(exist.begin(),exist.end(),disk_id_replica_3)==exist.end())
        {
            cnt_in++;
            ofstream outFile;
            /*outFile.open("./example_log.txt", std::ios::out | std::ios::app); // 以追加模式打开
            outFile << "cnt_in:" << cnt_in << " label:" << label <<  " size_disk:" << size_replica_3 << " disk_id:" << disk_id_replica_3 << " block_id:" << block_id_replica_3 << " time:" << time_stemp  << endl;
            outFile.close();*/
            object[object_id].replica[++current_replica_index] = disk_id_replica_3;  // 选中该磁盘
            exist.insert(disk_id_replica_3);
            int current_write_point = 0;
            while (current_write_point < size) {
            int block = *(free_blocks[disk_id_replica_3][block_id_replica_3+1].begin());
            free_blocks[disk_id_replica_3][block_id_replica_3+1].erase(free_blocks[disk_id_replica_3][block_id_replica_3+1].begin());  // 分配块
            if (disk[disk_id_replica_3][block] == 0){
            disk[disk_id_replica_3][block] = object_id;         // 写入数据
            object[object_id].unit[current_replica_index][++current_write_point] = block;
            block_num[disk_id_replica_3][block]=current_write_point;
            }
        }
        }
    }
    //没存下，全局找解
    for (int x = object_id; x <= object_id+N; x++)
    {
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

