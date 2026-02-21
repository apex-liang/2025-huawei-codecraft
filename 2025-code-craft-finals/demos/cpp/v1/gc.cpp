#include"solutions.h"
void gc_action(int round)
{
    if (round == 1) {
        scanf("%*s %*s");
    }
    printf("GARBAGE COLLECTION\n");
    if(round==1){
        
        for(int disk_id=1;disk_id<=N;disk_id++)
        {
            vector<pair<int, int>> total_rubbish;  // 整个磁盘的交换对
            int total_cnt = 0;

            if(time_stemp>0){
                for(int block_id=1;block_id<=disk_block_num;block_id++)
                {
                    if(disk_block[disk_id][block_id]>M||disk_block[disk_id][block_id]==0) continue;
                    if(find(label_need_read.begin(),label_need_read.end(),disk_block[disk_id][block_id])==label_need_read.end()) continue;//没找到也continue
                    int i=(block_id-1)*block_size+1;
                    int j=block_size*disk_block_num-1;
                    
                    while (i < j && total_cnt < K) {
                    // 移动指针寻找需要交换的位置
                    while(i<=block_id*block_size&&object[disk[disk_id][i]].label==disk_block[disk_id][block_id]) i++;//有可能是空，和非正常Label
                    while(j>i&&(object[disk[disk_id][j]].label!=disk_block[disk_id][block_id]||disk[disk_id][j]==0)) j--;//一定有数
                    if(i>=j) break;
                    // 获取交换对象信息
                    int object1 = disk[disk_id][i];
                    int object2 = disk[disk_id][j];
                    int block1=block_num[disk_id][i];
                    int block2=block_num[disk_id][j];
                    int read1=read_map[disk_id][i];
                    int read2=read_map[disk_id][j];
                    
                    // 处理 object1 为空的情况
                    if (object1 == 0) {
                        // 更新 object2 的单元位置
                        object[object2].unit[1][block2] = i;
                        block_num[disk_id][i]=block2;
                        block_num[disk_id][j]=0;
                        // 更新磁盘数据
                        disk[disk_id][i] = object2;
                        disk[disk_id][j] = 0;
                        read_map[disk_id][j]=read1;
                        read_map[disk_id][i]=read2;
                        // 更新标签可用位置
                        auto& label_able_1 = disk_label_able[disk_id][(i-1)/block_size+1];
                        auto& label_able_2 = disk_label_able[disk_id][(j-1)/block_size+1];
                        
                        label_able_1.erase(find(label_able_1.begin(), label_able_1.end(), i));
                        label_able_2.insert(j);
                    } 
                    // 处理 object1 非空的情况
                    else {
                        // 查找两个对象的单元位置
                        // 更新对象单元位置
                        object[object1].unit[1][block1] = j;
                        object[object2].unit[1][block2] = i;
                        block_num[disk_id][i]=block2;
                        block_num[disk_id][j]=block1;
                        // 更新磁盘数据
                        disk[disk_id][i] = object2;
                        disk[disk_id][j] = object1;
                        read_map[disk_id][j]=read1;
                        read_map[disk_id][i]=read2;
                    }

                    // 记录交换对并更新计数器
                    total_rubbish.push_back({i, j});
                    total_cnt++;
                    
                    // 强制移动指针防止死循环
                    i++;
                    j--;
                }
                }
            }
            if(time_stemp<=28)
                {
                    for(int block_id=1;block_id<=disk_block_num;block_id++)
                    {
                        if(disk_block[disk_id][block_id]>M||disk_block[disk_id][block_id]==0) continue;
                        int i=(block_id-1)*block_size+1;
                        int j=block_size*disk_block_num-1;
                        
                        while (i < j && total_cnt < K) {
                        // 移动指针寻找需要交换的位置
                        while(i<=block_id*block_size&&object[disk[disk_id][i]].label==disk_block[disk_id][block_id]) i++;//有可能是空，和非正常Label
                        while(j>i&&(object[disk[disk_id][j]].label!=disk_block[disk_id][block_id]||disk[disk_id][j]==0)) j--;//一定有数
                        if(i>=j) break;
                        // 获取交换对象信息
                        int object1 = disk[disk_id][i];
                        int object2 = disk[disk_id][j];
                        int block1=block_num[disk_id][i];
                        int block2=block_num[disk_id][j];
                        int read1=read_map[disk_id][i];
                        int read2=read_map[disk_id][j];
                        
                        // 处理 object1 为空的情况
                        if (object1 == 0) {
                            // 更新 object2 的单元位置
                            object[object2].unit[1][block2] = i;
                            block_num[disk_id][i]=block2;
                            block_num[disk_id][j]=0;
                            // 更新磁盘数据
                            disk[disk_id][i] = object2;
                            disk[disk_id][j] = 0;
                            read_map[disk_id][j]=read1;
                            read_map[disk_id][i]=read2;
                            // 更新标签可用位置
                            auto& label_able_1 = disk_label_able[disk_id][(i-1)/block_size+1];
                            auto& label_able_2 = disk_label_able[disk_id][(j-1)/block_size+1];
                            
                            label_able_1.erase(find(label_able_1.begin(), label_able_1.end(), i));
                            label_able_2.insert(j);
                        } 
                        // 处理 object1 非空的情况
                        else {
                            // 查找两个对象的单元位置
                            // 更新对象单元位置
                            object[object1].unit[1][block1] = j;
                            object[object2].unit[1][block2] = i;
                            block_num[disk_id][i]=block2;
                            block_num[disk_id][j]=block1;
                            // 更新磁盘数据
                            disk[disk_id][i] = object2;
                            disk[disk_id][j] = object1;
                            read_map[disk_id][j]=read1;
                            read_map[disk_id][i]=read2;
                        }

                        // 记录交换对并更新计数器
                        total_rubbish.push_back({i, j});
                        total_cnt++;
                        
                        // 强制移动指针防止死循环
                        i++;
                        j--;
                    }
                    }
            
                }
            else{
                for(int block_id = ((time_stemp-28)*2)%disk_block_num+1; block_id <= disk_block_num; block_id++)
                    {
                        if(disk_block[disk_id][block_id]>M||disk_block[disk_id][block_id]==0) continue;
                        int i=(block_id-1)*block_size+1;
                        int j=block_size*disk_block_num-1;
                        
                        while (i < j && total_cnt < K) {
                        // 移动指针寻找需要交换的位置
                        while(i<=block_id*block_size&&object[disk[disk_id][i]].label==disk_block[disk_id][block_id]) i++;//有可能是空，和非正常Label
                        while(j>i&&(object[disk[disk_id][j]].label!=disk_block[disk_id][block_id]||disk[disk_id][j]==0)) j--;//一定有数
                        if(i>=j) break;
                        // 获取交换对象信息
                        int object1 = disk[disk_id][i];
                        int object2 = disk[disk_id][j];
                        int block1=block_num[disk_id][i];
                        int block2=block_num[disk_id][j];
                        int read1=read_map[disk_id][i];
                        int read2=read_map[disk_id][j];
                        
                        // 处理 object1 为空的情况
                        if (object1 == 0) {
                            // 更新 object2 的单元位置
                            object[object2].unit[1][block2] = i;
                            block_num[disk_id][i]=block2;
                            block_num[disk_id][j]=0;
                            // 更新磁盘数据
                            disk[disk_id][i] = object2;
                            disk[disk_id][j] = 0;
                            read_map[disk_id][j]=read1;
                            read_map[disk_id][i]=read2;
                            // 更新标签可用位置
                            auto& label_able_1 = disk_label_able[disk_id][(i-1)/block_size+1];
                            auto& label_able_2 = disk_label_able[disk_id][(j-1)/block_size+1];
                            
                            label_able_1.erase(find(label_able_1.begin(), label_able_1.end(), i));
                            label_able_2.insert(j);
                        } 
                        // 处理 object1 非空的情况
                        else {
                            // 查找两个对象的单元位置
                            // 更新对象单元位置
                            object[object1].unit[1][block1] = j;
                            object[object2].unit[1][block2] = i;
                            block_num[disk_id][i]=block2;
                            block_num[disk_id][j]=block1;
                            // 更新磁盘数据
                            disk[disk_id][i] = object2;
                            disk[disk_id][j] = object1;
                            read_map[disk_id][j]=read1;
                            read_map[disk_id][i]=read2;
                        }

                        // 记录交换对并更新计数器
                        total_rubbish.push_back({i, j});
                        total_cnt++;
                        
                        // 强制移动指针防止死循环
                        i++;
                        j--;
                    }
                    }
            // 统一输出整个磁盘的结果
            
                }
            if(time_stemp>0){
                printf("%d\n", total_cnt);
                for (auto& p : total_rubbish) {
                    printf("%d %d\n", p.first, p.second);
                }
            }
            // else
            //     printf("0\n");

        }
    }
    else
    {
        for (int disk_id = 1; disk_id <= N; disk_id++) {
        vector<pair<int, int>> total_rubbish;  // 整个磁盘的交换对
        int total_cnt = 0;      
        if(time_stemp<=46)
        {
            for (int block_id_rubbish = 1; block_id_rubbish <= disk_block_num; block_id_rubbish++) {
            if(find(label_need_read_time[time_stemp+2].begin(),label_need_read_time[time_stemp+2].end(),disk_block[disk_id][block_id_rubbish])==label_need_read_time[time_stemp+2].end()) continue;
            int i = block_size * (block_id_rubbish - 1) + 1;
            int j = block_size * block_id_rubbish;
            int block_label_id = disk_block[disk_id][block_id_rubbish];
			while( block_label_id != 0 && block_label_id ==  disk_block[disk_id][j/block_size + 1] ) 
			{
				j = j + block_size;
			}
            
			
            while (i < j && total_cnt < K) {
                // 移动指针寻找需要交换的位置
                while (i <= j && object[disk[disk_id][i]].label == block_label_id) i++;
                while (i <= j && object[disk[disk_id][j]].label != block_label_id) j--;
                if (i >= j) break;
                // 获取交换对象信息
                int object1 = disk[disk_id][i];
                int object2 = disk[disk_id][j];
                int block1=block_num[disk_id][i];
                int block2=block_num[disk_id][j];
                int read1=read_map[disk_id][i];
                int read2=read_map[disk_id][j];
                
                // 处理 object1 为空的情况
                if (object1 == 0) {
                    // 更新 object2 的单元位置
                    object[object2].unit[1][block2] = i;
                    block_num[disk_id][i]=block2;
                    block_num[disk_id][j]=0;
                    // 更新磁盘数据
                    disk[disk_id][i] = object2;
                    disk[disk_id][j] = 0;
                    read_map[disk_id][j]=read1;
                    read_map[disk_id][i]=read2;
                    // 更新标签可用位置
                    auto& label_able_1 = disk_label_able[disk_id][(i-1)/block_size+1];
                    auto& label_able_2 = disk_label_able[disk_id][(j-1)/block_size+1];
                    
                    label_able_1.erase(find(label_able_1.begin(), label_able_1.end(), i));
                    label_able_2.insert(j);
                } 
                // 处理 object1 非空的情况
                else {
                    // 查找两个对象的单元位置
                    // 更新对象单元位置
                    object[object1].unit[1][block1] = j;
                    object[object2].unit[1][block2] = i;
                    block_num[disk_id][i]=block2;
                    block_num[disk_id][j]=block1;
                    // 更新磁盘数据
                    disk[disk_id][i] = object2;
                    disk[disk_id][j] = object1;
                    read_map[disk_id][j]=read1;
                    read_map[disk_id][i]=read2;
                }

                // 记录交换对并更新计数器
                total_rubbish.push_back({i, j});
                total_cnt++;
                
                // 强制移动指针防止死循环
                i++;
                j--;
            }
        }

        // 统一输出整个磁盘的结果
        }
        if(time_stemp<=28)
        {
            for (int block_id_rubbish = 1; block_id_rubbish <= disk_block_num; block_id_rubbish++) {
            // if(find(redisk_block[disk_id][block_id_rubbish])
            int i = block_size * (block_id_rubbish - 1) + 1;
            int j = block_size * block_id_rubbish;
            int block_label_id = disk_block[disk_id][block_id_rubbish];
			while( block_label_id != 0 && block_label_id ==  disk_block[disk_id][j/block_size + 1] ) 
			{
				j = j + block_size;
			}
            
			
            while (i < j && total_cnt < K) {
                // 移动指针寻找需要交换的位置
                while (i <= j && object[disk[disk_id][i]].label == block_label_id) i++;
                while (i <= j && object[disk[disk_id][j]].label != block_label_id) j--;
                if (i >= j) break;
                // 获取交换对象信息
                int object1 = disk[disk_id][i];
                int object2 = disk[disk_id][j];
                int block1=block_num[disk_id][i];
                int block2=block_num[disk_id][j];
                int read1=read_map[disk_id][i];
                int read2=read_map[disk_id][j];
                
                // 处理 object1 为空的情况
                if (object1 == 0) {
                    // 更新 object2 的单元位置
                    if(object2!=0)
                    object[object2].unit[1][block2] = i;
                    block_num[disk_id][i]=block2;
                    block_num[disk_id][j]=0;
                    // 更新磁盘数据
                    disk[disk_id][i] = object2;
                    disk[disk_id][j] = 0;
                    read_map[disk_id][j]=read1;
                    read_map[disk_id][i]=read2;
                    // 更新标签可用位置
                    auto& label_able_1 = disk_label_able[disk_id][(i-1)/block_size+1];
                    auto& label_able_2 = disk_label_able[disk_id][(j-1)/block_size+1];
                    
                    label_able_1.erase(find(label_able_1.begin(), label_able_1.end(), i));
                    label_able_2.insert(j);
                } 
                // 处理 object1 非空的情况
                else {
                    // 查找两个对象的单元位置
                    // 更新对象单元位置
                    object[object1].unit[1][block1] = j;
                    object[object2].unit[1][block2] = i;
                    block_num[disk_id][i]=block2;
                    block_num[disk_id][j]=block1;
                    // 更新磁盘数据
                    disk[disk_id][i] = object2;
                    disk[disk_id][j] = object1;
                    read_map[disk_id][j]=read1;
                    read_map[disk_id][i]=read2;
                }

                // 记录交换对并更新计数器
                total_rubbish.push_back({i, j});
                total_cnt++;
                
                // 强制移动指针防止死循环
                i++;
                j--;
            }
        }

        // 统一输出整个磁盘的结果
        }
        else{
            for (int block_id_rubbish = ((time_stemp-28)*2)%disk_block_num+1; block_id_rubbish <= disk_block_num; block_id_rubbish++) {
                int i = block_size * (block_id_rubbish - 1) + 1;
                int j = block_size * block_id_rubbish;
                int block_label_id = disk_block[disk_id][block_id_rubbish];
                while( block_label_id != 0 && block_label_id ==  disk_block[disk_id][j/block_size + 1] ) 
                {
                    j = j + block_size;
                }
                
                
                while (i < j && total_cnt < K) {
                    // 移动指针寻找需要交换的位置
                    while (i <= j && object[disk[disk_id][i]].label == block_label_id) i++;
                    while (i <= j && object[disk[disk_id][j]].label != block_label_id) j--;
                    if (i >= j) break;
                    // 获取交换对象信息
                    int object1 = disk[disk_id][i];
                    int object2 = disk[disk_id][j];
                    int block1=block_num[disk_id][i];
                    int block2=block_num[disk_id][j];
                    int read1=read_map[disk_id][i];
                    int read2=read_map[disk_id][j];
                    
                    // 处理 object1 为空的情况
                    if (object1 == 0) {
                        // 更新 object2 的单元位置
                        object[object2].unit[1][block2] = i;
                        block_num[disk_id][i]=block2;
                        block_num[disk_id][j]=0;
                        // 更新磁盘数据
                        disk[disk_id][i] = object2;
                        disk[disk_id][j] = 0;
                        read_map[disk_id][j]=read1;
                        read_map[disk_id][i]=read2;
                        // 更新标签可用位置
                        auto& label_able_1 = disk_label_able[disk_id][(i-1)/block_size+1];
                        auto& label_able_2 = disk_label_able[disk_id][(j-1)/block_size+1];
                        
                        label_able_1.erase(find(label_able_1.begin(), label_able_1.end(), i));
                        label_able_2.insert(j);
                    } 
                    // 处理 object1 非空的情况
                    else {
                        // 查找两个对象的单元位置
                        // 更新对象单元位置
                        object[object1].unit[1][block1] = j;
                        object[object2].unit[1][block2] = i;
                        block_num[disk_id][i]=block2;
                        block_num[disk_id][j]=block1;
                        // 更新磁盘数据
                        disk[disk_id][i] = object2;
                        disk[disk_id][j] = object1;
                        read_map[disk_id][j]=read1;
                        read_map[disk_id][i]=read2;
                    }

                    // 记录交换对并更新计数器
                    total_rubbish.push_back({i, j});
                    total_cnt++;
                    
                    // 强制移动指针防止死循环
                    i++;
                    j--;
                }
            }

            // 统一输出整个磁盘的结果
            
            }
            for (int block_id_rubbish = 1; block_id_rubbish <= disk_block_num; block_id_rubbish++) {
                // if(find(redisk_block[disk_id][block_id_rubbish])
                int i = block_size * (block_id_rubbish - 1) + 1;
                int j = block_size * block_id_rubbish;
                int block_label_id = disk_block[disk_id][block_id_rubbish];
                while( block_label_id != 0 && block_label_id ==  disk_block[disk_id][j/block_size + 1] ) 
                {
                    j = j + block_size;
                }
                
                
                while (i < j && total_cnt < K) {
                    // 移动指针寻找需要交换的位置
                    while (i <= j && object[disk[disk_id][i]].label == block_label_id) i++;
                    while (i <= j && object[disk[disk_id][j]].label != block_label_id) j--;
                    if (i >= j) break;
                    // 获取交换对象信息
                    int object1 = disk[disk_id][i];
                    int object2 = disk[disk_id][j];
                    int block1=block_num[disk_id][i];
                    int block2=block_num[disk_id][j];
                    int read1=read_map[disk_id][i];
                    int read2=read_map[disk_id][j];
                    
                    // 处理 object1 为空的情况
                    if (object1 == 0) {
                        // 更新 object2 的单元位置
                        object[object2].unit[1][block2] = i;
                        block_num[disk_id][i]=block2;
                        block_num[disk_id][j]=0;
                        // 更新磁盘数据
                        disk[disk_id][i] = object2;
                        disk[disk_id][j] = 0;
                        read_map[disk_id][j]=read1;
                        read_map[disk_id][i]=read2;
                        // 更新标签可用位置
                        auto& label_able_1 = disk_label_able[disk_id][(i-1)/block_size+1];
                        auto& label_able_2 = disk_label_able[disk_id][(j-1)/block_size+1];
                        
                        label_able_1.erase(find(label_able_1.begin(), label_able_1.end(), i));
                        label_able_2.insert(j);
                    } 
                    // 处理 object1 非空的情况
                    else {
                        // 查找两个对象的单元位置
                        // 更新对象单元位置
                        object[object1].unit[1][block1] = j;
                        object[object2].unit[1][block2] = i;
                        block_num[disk_id][i]=block2;
                        block_num[disk_id][j]=block1;
                        // 更新磁盘数据
                        disk[disk_id][i] = object2;
                        disk[disk_id][j] = object1;
                        read_map[disk_id][j]=read1;
                        read_map[disk_id][i]=read2;
                    }

                    // 记录交换对并更新计数器
                    total_rubbish.push_back({i, j});
                    total_cnt++;
                    
                    // 强制移动指针防止死循环
                    i++;
                    j--;
                }
            }
            printf("%d\n", total_cnt);
            for (auto& p : total_rubbish) {
                printf("%d %d\n", p.first, p.second);
            }
        }
    }
    fflush(stdout);
}
