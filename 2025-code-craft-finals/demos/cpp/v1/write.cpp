#include"solutions.h"
void do_object_write(int* object_unit, int* disk_unit, int size, int object_id)
{
    int current_write_point = 0;
    for (int i = 1; i <= V; i++) {
        if (disk_unit[i] == 0) {
            disk_unit[i] = object_id;
            object_unit[++current_write_point] = i;
            if (current_write_point == size) {
                break;
            }
        }
    }

    assert(current_write_point == size);
}
void do_object_write_my_mode(int mode,int object_id, int size, int label,int &current_replica_index)
{
    set<pair<int,int>>::iterator it;
    if(mode==1)//备份一寻找已占领位置
    {
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
        return;
    }
    else if(mode==2)//备份一抢占空模式
    {
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
        return;
    }
    else if(mode==3)
    {
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
        return;
    }
    else if(mode==4)
    {
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
        return;
    }
    else if(mode==5)//label_0备份1
    {
        for(int x=object_id;x<object_id+N;x++)
        {
            if (current_replica_index >= 1) break;
            int disk_id=x%N+1;
            int block_id=disk_block_num;
            while(disk_label_able[disk_id][block_id].size()<size) block_id--;
            
            if(disk_label_able[disk_id][block_id].size()>=size&&find(exist.begin(),exist.end(),disk_id)==exist.end())
            {
                object[object_id].replica[++current_replica_index] = disk_id;  // 选中该磁盘
                exist.insert(disk_id);
                int current_write_point = 0;
                int block=0;
                while (!disk_label_able[disk_id][block_id].empty() && current_write_point < size) {
                        block = *(disk_label_able[disk_id][block_id].rbegin());
                        disk_label_able[disk_id][block_id].erase(find(disk_label_able[disk_id][block_id].begin(),disk_label_able[disk_id][block_id].end(),block));  // 分配块
                        if (disk[disk_id][block] == 0){
                            disk[disk_id][block] = object_id;         // 写入数据
                            object[object_id].unit[current_replica_index][++current_write_point] = block;
                            block_num[disk_id][block]=current_write_point;
                        }
                        
                    }
            }
        }
    }
    else if(mode==6)//备份一寻找已占领位置
    {
        
        for(int i=0;i<label_block[label].size();i++)
        {
            if(i==0) it=label_block[label].begin();
            if (current_replica_index >= 1) break;
            int block_id=it->first;
            int disk_id=it->second;
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
        return;
    }
    else if(mode==7)//备份一抢占空模式
    {
        
        for(int x=object_id;x<object_id+N;x++)
        {
            if (current_replica_index >= 1) break;
            
            int disk_id=0;
            int min=disk_block_num;
            for(int m=1;m<=N;m++)
            {
                if(*free_space_block[m].begin()<min) {disk_id=m;min=*free_space_block[disk_id].begin();}

            }
            if(find(exist.begin(),exist.end(),disk_id)==exist.end()&&free_space_block[disk_id].size()>0){
                int block_id=*(free_space_block[disk_id].begin());
                free_space_block[disk_id].erase(free_space_block[disk_id].begin());
                label_block[label].insert({block_id,disk_id});
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
        return;
    }
    else if(mode==8)
    {
       
        for(;current_replica_index<1;){
            //  ofstream ofs;
            // ofs.open("./write_debug.txt",ios::out|ios::app);
            // ofs << "OK3" << endl;
            // ofs.close();
        
            int max_size=0;
            pair<int,int> pos_tmp;
            for(int disk_id=1;disk_id<=N;disk_id++)
            {
                for(int block_id=1;block_id<=disk_block_num;block_id++)
                {
                    if((disk_label_able[disk_id][block_id].size()>max_size)&&find(exist.begin(),exist.end(),disk_id)==exist.end() &&time_stemp > 5 + disk_block_time[disk_id][block_id])
                    {
                        max_size=disk_label_able[disk_id][block_id].size();
                        pos_tmp={block_id,disk_id};
                    }
                }
            }
            int block_id=pos_tmp.first;
            int disk_id=pos_tmp.second;
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
        return;
    }
}

void do_object_write_my(int round,int object_id, int size, int label) {
    int current_replica_index = 0;
    int current_write_point=0;
    exist.clear();
    if(round==1)
    {
        if(label==0)
        {
            do_object_write_my_mode(5,object_id,size,label,current_replica_index);
            do_object_write_my_mode(4,object_id,size,label,current_replica_index);
        }
        else{
            do_object_write_my_mode(6,object_id,size,label,current_replica_index);
            do_object_write_my_mode(7,object_id,size,label,current_replica_index);
            do_object_write_my_mode(8,object_id,size,label,current_replica_index);
            do_object_write_my_mode(4,object_id,size,label,current_replica_index);
        }
    }
    else
    {
        if(object[object_id].label!=0)
        {
            do_object_write_my_mode(1,object_id,size,label,current_replica_index);
            do_object_write_my_mode(2,object_id,size,label,current_replica_index);
            do_object_write_my_mode(3,object_id,size,label,current_replica_index);
            do_object_write_my_mode(4,object_id,size,label,current_replica_index);
        }
        else
        {
            do_object_write_my_mode(4,object_id,size,label,current_replica_index);
        }
    }    
    exist.clear();
}
void write_action(int round, int time)
{
    int n_write;
    if (round == 1) {
        scanf("%d", &n_write);
        n_writes[time] = n_write;
    } else {
        n_write = n_writes[time];
    }

    for (int i = 1; i <= n_write; i++) {
        int id, size, tag;
        if (round == 1) {
            scanf("%d%d%d", &id, &size, &tag);
            last_object = id;
            write_datas[time].emplace_back(id, size, tag);
        } else {
            const auto& write_tuple = write_datas[time].at(i - 1);
            id = get<0>(write_tuple);
            size = get<1>(write_tuple);
            tag = get<2>(write_tuple);
        }
        object[id].last_request_point = 0;
        object[id].size = size;
        if(round==1)
        {
            object[id].label = tag;
            object[id].write_time = (time - 1) / 1800 + 1 ;
            object[id].write_time_2=time;
            object[id].delete_time = 48;
        }
        object[id].is_delete = false;
        label_exist[tag]+=size;
        for (int j = 1; j <= REP_NUM; j++) {   
            object[id].unit[j] = static_cast<int*>(malloc(sizeof(int) * (size + 1)));
        }
        do_object_write_my(round,id,size,object[id].label);

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
