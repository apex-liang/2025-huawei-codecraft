#include"solutions.h"
void do_object_delete(const int* object_unit, int* disk_unit, int size)
{
    for (int i = 1; i <= size; i++) {
        disk_unit[object_unit[i]] = 0;
    }
}
void do_object_delete_my(int round,const int* object_unit, int* disk_unit, int size,int object_id,int disk_id,int replica_id)
{
    if(round==1)
    {
        for (int i = 1; i <= size; i++) {
        disk_unit[object_unit[i]] = 0;
        if(replica_id!=1)
            free_space[disk_id].insert(object_unit[i]);
        else{
                read_map[disk_id][object_unit[i]]=0;
                for(int j=1;j<=disk_block_num;j++)
                {
                    if((object_unit[i]>=(j-1)*block_size+1)&&(object_unit[i]<=j*block_size))
                        disk_label_able[disk_id][j].insert(object_unit[i]);
                }
            }
            block_num[disk_id][object_unit[i]]=0;
        }
    }
    else{
        for (int i = 1; i <= size; i++) {
        disk_unit[object_unit[i]] = 0;
        if((replica_id!=1)||(object[object_id].label==0))
            free_space[disk_id].insert(object_unit[i]);
        else{
                read_map[disk_id][object_unit[i]]=0;
                for(int j=1;j<=disk_block_num;j++)
                {
                    if((object_unit[i]>=(j-1)*block_size+1)&&(object_unit[i]<=j*block_size))
                        disk_label_able[disk_id][j].insert(object_unit[i]);
                }
            }
            
            block_num[disk_id][object_unit[i]]=0;
        }
    }
    
}

void delete_action(int round, int time)
{
    int n_delete;
    int abort_num = 0;
    static int _id[MAX_OBJECT_NUM];

    if (round == 1) {
        scanf("%d", &n_delete);
        n_deletes[time] = n_delete;
        for (int i = 1; i <= n_delete; i++) {
            scanf("%d", &_id[i]);
            delete_datas[time].push_back(_id[i]);
        }
    } else {
        n_delete = n_deletes[time];
        for (int i = 1; i <= n_delete; ++i) {
            _id[i] = delete_datas[time].at(i - 1);
        }
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
                request_not_finish[current_id]=false;
            }
            current_id = request[current_id].prev_id;
        }
        for (int j = 1; j <= REP_NUM; j++) {
            do_object_delete_my(round,object[id].unit[j], disk[object[id].replica[j]], object[id].size,id,object[id].replica[j],j);
        }
        object[id].is_delete = true;
        object[id].delete_time = (time - 1) / 1800 + 1;
        if(object[id].is_predict==0)
        {
            label_exist[object[id].label]-=object[id].size;
        }
    }

    fflush(stdout);
}
