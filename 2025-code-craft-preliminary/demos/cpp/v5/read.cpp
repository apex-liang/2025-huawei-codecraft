#include"solutions.h"
int find_next(int block_id,int disk_id)
{
    for(int i=block_id;i<=disk_block_num;i++)
    {
        if(find(label_need_read.begin(),label_need_read.end(), disk_block[disk_id][i])!=label_need_read.end())
            return i;
    }
    return disk_block_num+1;
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
        request_all++;
    }
    memcpy(read_demand_sort,read_demand[time_stemp],sizeof read_demand[time_stemp]);
    sort(read_demand_sort+1,read_demand_sort+MAX_LABEL_NUM);
	int n_resp=0;
	for(int i=1;i<=N;i++){
			int pre_token=pre_token_disk[i];
			int current_token=G;
            if((disk_point[i]>disk_begin[i])&&(find_next(disk_point[i]/block_size+1,i)==disk_block_num+1))//如果磁盘扫完了从头开始
            {
                printf("j %d\n",(find_next(1,i)-1)*block_size+1);
                disk_point[i]=(find_next(1,i)-1)*block_size+1;
                // printf("j %d\n",1);
                // disk_point[i]=1;
                pre_token_disk[i]=0;
            }
            else{
            	if(cnt_disk_block[i][disk_point[i]/block_size+1]>10){//存的很密集，优先全r过去
                while(current_token>0){
				int current_unit=disk[i][disk_point[i]];
				int label_id=disk_block[i][disk_point[i]/block_size+1];
				
				if(read_demand[time_stemp][label_id]>=read_sum[time_stemp]/28){//如果要读取，值得读取
					int t=max(16,pre_token*4/5+1);
					if(!pre_token||pre_token==1) current_token-=64,pre_token=64;
                    else current_token-=t,pre_token=t;
                    if(current_token>0)
					{
					pre_token_disk[i]=pre_token;
					printf("r");
                    int current_id=object[current_unit].last_request_point;
                    int cnt=0;
                    while(current_id != 0&&cnt<20){
                        int block_id=block_num[i][disk_point[i]];
                        request[current_id].state[block_id]=true;
                        if(request[current_id].is_done==false&&request[current_id].state.count()==object[request[current_id].object_id].size){
                        ok_all++;
                        successful_request[++n_resp]=current_id;
                        request[current_id].is_done=true;
                        }
                        current_id=request[current_id].prev_id;
                        cnt++;
                    }
                    disk_point[i]=(disk_point[i]+1)%V;
                }
			}
				else{
					if(current_token>=1){
                    printf("p");
                    current_token-=1;
                    pre_token_disk[i]=0;
                    pre_token=0;
                    disk_point[i]=(disk_point[i]+1)%V;}
				}
			}
            printf("#\n");
            }
            else{
            	while(current_token>0){//如果存的不密集，就用公式计算r与p的操作
				int current_unit=disk[i][disk_point[i]];
				int label_id=disk_block[i][disk_point[i]/block_size+1];
				
				if(read_demand[time_stemp][label_id]>=read_sum[time_stemp]/28&&label_id==object[current_unit].label){//current_unit
					int t=max(16,pre_token*4/5+1);
					if(!pre_token||pre_token==1) current_token-=64,pre_token=64;
                    else current_token-=t,pre_token=t;
                    if(current_token>0)
					{
					pre_token_disk[i]=pre_token;
					printf("r");
                    int current_id=object[current_unit].last_request_point;
                    int cnt=0;
                    while(current_id != 0&&cnt<20){
                        int block_id=block_num[i][disk_point[i]];
                        request[current_id].state[block_id]=true;
                        if(request[current_id].is_done==false&&request[current_id].state.count()==object[request[current_id].object_id].size){
                        ok_all++;
                        successful_request[++n_resp]=current_id;
                        request[current_id].is_done=true;
                        }
                        current_id=request[current_id].prev_id;
                        cnt++;
                    }
                    disk_point[i]=(disk_point[i]+1)%V;
                }
			}
			else{
				if(current_token>=1){
                    printf("p");
                    current_token-=1;
                    pre_token_disk[i]=0;
                    pre_token=0;
                    disk_point[i]=(disk_point[i]+1)%V;}
			}
        }
	      printf("#\n");
    }
  }
}
            printf("%d\n",n_resp);
            for(int j=1;j<=n_resp;j++) printf("%d\n",successful_request[j]);
    fflush(stdout);
}