#include"solutions.h"
void gc_action() {
    scanf("%*s %*s");
    printf("GARBAGE COLLECTION\n");
    for (int disk_id = 1; disk_id <= N; disk_id++) {
        vector<pair<int, int>> total_rubbish;  // 整个磁盘的交换对
        int total_cnt = 0;                    // 整个磁盘的总交换次数

        for (int block_id_rubbish = 1; block_id_rubbish <= disk_block_num; block_id_rubbish++) {
            int i = block_size * (block_id_rubbish - 1) + 1;
            int j = block_size * block_id_rubbish;
            int block_label_id = disk_block[disk_id][block_id_rubbish];
            ofstream ofs;
            ofs.open("./example_logz-6.txt",ios::out|ios::app);
			while( block_label_id != 0 && block_label_id ==  disk_block[disk_id][j/block_size + 1] ) 
			{
				ofs << "OK3" << " " << block_label_id << " " <<  disk_block[disk_id][j/block_size + 1] << " " << j/block_size + 1  << " " << disk_id << endl;
				j = j + block_size;
			}
			
            while (i < j && total_cnt < K) {
                // 移动指针寻找需要交换的位置
                while (i <= j && object[disk[disk_id][i]].label == block_label_id) i++;
                while (i <= j && object[disk[disk_id][j]].label != block_label_id) j--;
                if (i >= j) break;
				ofs << "OK4 " << i <<" " << j << endl;
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
                    ofs << "OK5 " << (i-1)/block_size+1 <<" " << (j-1)/block_size+1 << endl;
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
            ofs.close();
        }

        // 统一输出整个磁盘的结果
        printf("%d\n", total_cnt);
        for (auto& p : total_rubbish) {
            printf("%d %d\n", p.first, p.second);
        }
    }
    fflush(stdout);
}