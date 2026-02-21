#include"solutions.h"
void read_action(int t)
{
    int n_read;
    int request_id, object_id;
    scanf("%d", &n_read);
    int n_resp=0;
    vector<int> busy_request;
    for (int i = 1; i <= n_read; i++) {
        scanf("%d%d", &request_id, &object_id);
        request[request_id].object_id = object_id;
        request[request_id].prev_id = object[object_id].last_request_point;
        request[request_id].t=t;
        object[object_id].last_request_point = request_id;
        int label = object[object_id].label;
        request_all++;
        // if(find(label_need_read.begin(),label_need_read.end(),object[object_id].label)!=label_need_read.end())
        //     request_not_finish_2[request_id]=true;
        // else{
        //     n_busy++;
        //     busy_request.push_back(request_id);
        //     request[request_id].is_done=true;
        // }
    }
    memcpy(read_demand_sort,read_demand[time_stemp],sizeof read_demand[time_stemp]);
    sort(read_demand_sort+1,read_demand_sort+MAX_LABEL_NUM);
	
	for(int i=1;i<=N;i++){
			int pre_token=pre_token_disk[i];
			int current_token=G;
            if(disk_point[i]>disk_begin[i])
            {
                printf("j %d\n",1);
                disk_point[i]=1;
                pre_token_disk[i]=0;
            }
            else{
                while(current_token>0){
				int current_unit=disk[i][disk_point[i]];
				int label_id=disk_block[i][disk_point[i]/block_size+1];
				
				if(read_demand[time_stemp][label_id]>=read_sum[time_stemp]/30){//30
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
            
            printf("%d\n",n_resp);
            
            for(int j=1;j<=n_resp;j++) 
            {
                printf("%d\n",successful_request[j]);
                request_not_finish_2[successful_request[j]]=false;
                // request_not_finish.erase(find(request_not_finish.begin(),request_not_finish.end(),successful_request[j]));
            }

    fflush(stdout);
}