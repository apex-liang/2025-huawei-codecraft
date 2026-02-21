#include"solutions.h"
void update_read_map(){
	int V_now =  V-max_demand_all_label*2/N ;
	for(int disk_id = 1 ;disk_id <= N ;disk_id++){
		for(int pos = 1 ; pos <= V_now ; pos++){
			read_map_all[disk_id][pos] = read_map_all[disk_id][pos-1] + read_map[disk_id][pos];
		}
	}
	
}
void read_action(int t)
{
    int n_read;
    int request_id, object_id;
    scanf("%d", &n_read);
    int n_resp=0;
    int n_busy=0;
    vector<int> busy_request;
    for (int i = 1; i <= n_read; i++) {
        scanf("%d%d", &request_id, &object_id);
        request[request_id].object_id = object_id;
        request[request_id].prev_id = object[object_id].last_request_point;
        request[request_id].t=t;
        object[object_id].last_request_point = request_id;
        int label = object[object_id].label;
        request_all++;
        if(find(label_need_read.begin(),label_need_read.end(),object[object_id].label)!=label_need_read.end()){
        	 request_not_finish_2[request_id]=true;
        	 int disk_id_tem = object[object_id].replica[1];
        	 for(int x = 1 ; x <=  object[object_id].size ;x++) read_map[disk_id_tem][object[object_id].unit[1][x]] = 1; 
		} 
        else{
            n_busy++;
            busy_request.push_back(request_id);
            request[request_id].is_done=true;
        }
    }
    update_read_map();
    memcpy(read_demand_sort,read_demand[time_stemp],sizeof read_demand[time_stemp]);
    sort(read_demand_sort+1,read_demand_sort+MAX_LABEL_NUM);
	
for(int i=1;i<=N;i++){
    // 处理第一个磁针
    int pre_token=pre_token_disk[i];
    int current_token=G;
    if((read_map_all[i][disk_point_mid[i]]-read_map_all[i][disk_point[i]]==0)||disk_point[i]>=disk_point_mid[i]){//disk_point[i]>=(disk_begin[i]-block_size*free_space_block[i].size())/2
        int x=1;
        while(read_map_all[i][x]==0) x++;
        // if(req_p1[i]-req_p2[i]>2000&&!is_right[i]&&disk_point_mid[i]<disk_begin[i]-block_size*free_space_block[i].size()-block_size*2)  {disk_point_mid[i]+=block_size;is_right[i]=1;}
        // else if(req_p2[i]-req_p1[i]>2000&&!is_left[i]&&disk_point_mid[i]>block_size*2)  {disk_point_mid[i]-=block_size;is_left[i]=1;}
        if(x>disk_point_mid[i]) x=1;//如果0个请求就从1开始
        printf("j %d\n",x);
        disk_point[i]=x;
        pre_token_disk[i]=0;
    }
    else if(  (read_map_all[i][disk_point[i]+G]-read_map_all[i][disk_point[i]]) == 0)
    {
        int x=disk_point[i]+G;
        while((read_map_all[i][x]-read_map_all[i][disk_point[i]])==0) x++;
        if(x>disk_point_mid[i])
        {
            x=1;
            while(read_map_all[i][x]==0) x++;
            if(x>disk_point_mid[i]) x=1;//如果0个请求就从1开始
        }//如果0个请求就从1开始
        printf("j %d\n",x);
        jump_all ++;
		jumpsize += x - disk_point[i];
        disk_point[i]=x;
        pre_token_disk[i]=0;
    }
    else{
        int start = disk_point[i];
        int end = start + G/16;
        int density = read_map_all[i][end] - read_map_all[i][start]; // 直接计算密度
        
        if(density > (G-G/16)/63){ // 密集模式
            while(current_token > 0){
                int need = (pre_token==0 || pre_token==1) ? 64 : max(16, (pre_token*4)/5+1);
                if(current_token >= need){
                    printf("r");
                    current_token -= need;
                    pre_token = need;
                    
                    // 处理读取请求
                    int current_id=object[disk[i][disk_point[i]]].last_request_point;
                    int cnt=0;
                    while(current_id && cnt<20){
                        int bid=block_num[i][disk_point[i]];
                        request[current_id].state[bid]=true;
                        if(!request[current_id].is_done && 
                          request[current_id].state.count()==object[request[current_id].object_id].size){
                            req_p1[i]++;
                            successful_request[++n_resp]=current_id;
                            request[current_id].is_done=true;
                        }
                        current_id=request[current_id].prev_id;
                        cnt++;
                    }
                    disk_point[i] = (disk_point[i]+1)%V;
                }
                else break;
            }
        }
        else{ // 稀疏模式
            while(current_token > 0){
                if(read_map[i][disk_point[i]] == 1){ // 需要读取的块
                    int need = (pre_token==0 || pre_token==1) ? 64 : max(16, (pre_token*4)/5+1);
                    if(current_token >= need){
                        printf("r");
                        current_token -= need;
                        pre_token = need;
                        
                        // 处理读取请求
                        int current_id=object[disk[i][disk_point[i]]].last_request_point;
                        int cnt=0;
                        while(current_id && cnt<20){
                            int bid=block_num[i][disk_point[i]];
                            request[current_id].state[bid]=true;
                            if(!request[current_id].is_done && 
                              request[current_id].state.count()==object[request[current_id].object_id].size){
                                req_p1[i]++;
                                successful_request[++n_resp]=current_id;
                                request[current_id].is_done=true;
                            }
                            current_id=request[current_id].prev_id;
                            cnt++;
                        }
                        disk_point[i] = (disk_point[i]+1)%V;
                    }
                    else break;
                }
                else if(current_token >=1){ // 不需要读取的块
                    printf("p");
                    current_token -=1;
                    pre_token = 0;
                    disk_point[i] = (disk_point[i]+1)%V;
                }
                else break;
            }
        }
        pre_token_disk[i] = pre_token;
        printf("#\n");
    }

    // 处理第二个磁针（disk_point_2）
    int pre_token2=pre_token_disk_2[i];
    int current_token2=G;
    if((read_map_all[i][disk_begin[i]-block_size*free_space_block[i].size()]-read_map_all[i][disk_point_2[i]]==0)||disk_point_2[i]<disk_point_mid[i]){//disk_point_2[i]<(disk_begin[i]-block_size*free_space_block[i].size())/2 || disk_point_2[i]>(disk_begin[i]-block_size*free_space_block[i].size())
        int x=disk_point_mid[i]+1;
        while((read_map_all[i][x]-read_map_all[i][disk_point_mid[i]])==0) x++;
        // if(req_p1[i]-req_p2[i]>2000&&!is_right[i]&&disk_point_mid[i]<disk_begin[i]-block_size*free_space_block[i].size()-block_size*2)  {disk_point_mid[i]+=block_size;is_right[i]=1;}
        // else if(req_p2[i]-req_p1[i]>2000&&!is_left[i]&&disk_point_mid[i]>block_size*2)  {disk_point_mid[i]-=block_size;is_left[i]=1;}
        if(x>(disk_begin[i]-block_size*free_space_block[i].size())) x=disk_point_mid[i]+1;//超过，说明是0个请求，所以放在盘一开始
        printf("j %d\n",x);
        disk_point_2[i]=x;
        pre_token_disk_2[i]=0;
    }
    else if(  (read_map_all[i][disk_point_2[i]+G]-read_map_all[i][disk_point_2[i]])==0)
    {
        int x=disk_point_2[i]+G;
        while((read_map_all[i][x]-read_map_all[i][disk_point[i]])==0) x++;
        if(x>(disk_begin[i]-block_size*free_space_block[i].size())) 
        {
            x=disk_point_mid[i]+1;
            while((read_map_all[i][x]-read_map_all[i][disk_point_mid[i]])==0) x++;
            if(x>(disk_begin[i]-block_size*free_space_block[i].size())) x=disk_point_mid[i]+1;//超过，说明是0个请求，所以放在盘一开始
        }
        printf("j %d\n",x);
        jump_all++;
        jumpsize += x - disk_point_2[i];
        disk_point_2[i]=x;
        pre_token_disk_2[i]=0;
    }
    else{
        int start = disk_point_2[i];
        int end = start + G/16;
        int density = read_map_all[i][end] - read_map_all[i][start];
        
        if(density > (G-G/16)/63){ // 密集模式
            while(current_token2 > 0){
                int need = (pre_token2==0 || pre_token2==1) ? 64 : max(16, (pre_token2*4)/5+1);
                if(current_token2 >= need){
                    printf("r");
                    current_token2 -= need;
                    pre_token2 = need;
                    
                    // 处理读取请求
                    int current_id=object[disk[i][disk_point_2[i]]].last_request_point;
                    int cnt=0;
                    while(current_id && cnt<20){
                        int bid=block_num[i][disk_point_2[i]];
                        request[current_id].state[bid]=true;
                        if(!request[current_id].is_done && 
                          request[current_id].state.count()==object[request[current_id].object_id].size){
                            req_p2[i]++;
                            successful_request[++n_resp]=current_id;
                            request[current_id].is_done=true;
                        }
                        current_id=request[current_id].prev_id;
                        cnt++;
                    }
                    disk_point_2[i] = (disk_point_2[i]+1)%V;
                }
                else break;
            }
        }
        else{ // 稀疏模式
            while(current_token2 > 0){
                if(read_map[i][disk_point_2[i]] == 1){
                    int need = (pre_token2==0 || pre_token2==1) ? 64 : max(16, (pre_token2*4)/5+1);
                    if(current_token2 >= need){
                        printf("r");
                        current_token2 -= need;
                        pre_token2 = need;
                        
                        // 处理读取请求
                        int current_id=object[disk[i][disk_point_2[i]]].last_request_point;
                        int cnt=0;
                        while(current_id && cnt<20){
                            int bid=block_num[i][disk_point_2[i]];
                            request[current_id].state[bid]=true;
                            if(!request[current_id].is_done && 
                              request[current_id].state.count()==object[request[current_id].object_id].size){
                                req_p2[i]++;
                                successful_request[++n_resp]=current_id;
                                request[current_id].is_done=true;
                            }
                            current_id=request[current_id].prev_id;
                            cnt++;
                        }
                        disk_point_2[i] = (disk_point_2[i]+1)%V;
                    }
                    else break;
                }
                else if(current_token2 >=1){
                    printf("p");
                    current_token2 -=1;
                    pre_token2 = 0;
                    disk_point_2[i] = (disk_point_2[i]+1)%V;
                }
                else break;
            }
        }
        pre_token_disk_2[i] = pre_token2;
        printf("#\n");
    }
}
            
            printf("%d\n",n_resp);
            
            for(int j=1;j<=n_resp;j++) 
            {
                printf("%d\n",successful_request[j]);
                int object_id=request[successful_request[j]].object_id;
                request_not_finish_2[successful_request[j]]=false;
                int disk_id_tem = object[object_id].replica[1];
        	 	for(int x = 1 ; x <=  object[object_id].size ;x++) read_map[disk_id_tem][object[object_id].unit[1][x]] = 0;
                // request_not_finish.erase(find(request_not_finish.begin(),request_not_finish.end(),successful_request[j]));
            }
           
            // deque<int>::iterator it=request_not_finish.begin();
            // while(request[*it].t<=t-105)
            // {   
            //     n_busy++;
            //     busy_request.push_back(*it);
            //     request[*it].is_done=true;
            //     it++;
            //     request_not_finish.pop_front();

            // }

            
            while((request_not_finish_2[i_tmp]==false)&&(i_tmp<request_all)) i_tmp++;
            while((request[i_tmp].t<=t-105)&&(i_tmp<=request_all))
            {
                if((request_not_finish_2[i_tmp]==true))//&&request[i_tmp].is_done==false
                {
                    n_busy++;
                    busy_request.push_back(i_tmp);
                    request[i_tmp].is_done=true;
                    request_not_finish_2[i_tmp]=false;//已经被处理
                }
                // else if(request_not_finish_2[i_tmp]==true)
                // {
                //     request_not_finish_2[i_tmp]=false;//已经被处理
                // }
                i_tmp++;
            }
            i_tmp=i_tmp-1;


            printf("%d\n",n_busy);
            for(int i=0;i<n_busy;i++)
            {
                printf("%d\n",busy_request[i]);
                int object_id=request[busy_request[i]].object_id;
                if(object[object_id].last_request_point==busy_request[i])
                {int disk_id_tem=object[object_id].replica[1];
                for(int x = 1 ; x <=  object[object_id].size ;x++) read_map[disk_id_tem][object[object_id].unit[1][x]] = 0;}
                // request[*it_tmp].is_done=true;
            }
    fflush(stdout);
}