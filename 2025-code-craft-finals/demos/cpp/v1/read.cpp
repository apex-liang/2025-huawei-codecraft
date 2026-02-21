#include"solutions.h"
void update_read_map(){
	int V_now = V-V*0.9*2/3+k_new;
	for(int disk_id = 1 ;disk_id <= N ;disk_id++){
		for(int pos = 1 ; pos <= V_now ; pos++){
			read_map_all[disk_id][pos] = read_map_all[disk_id][pos-1] + read_map[disk_id][pos];
		}
	}
	
}
void do_read_action_point_my(int point_id,int left_pos,int right_pos,int i,int &n_resp)
{
    int pre_token=pre_token_disk[i][point_id];
    int current_token=G;
    if((read_map_all[i][right_pos]-read_map_all[i][disk_point[i][point_id]]==0)||disk_point[i][point_id]>=right_pos){//disk_point[i]>=(disk_begin[i]-block_size*free_space_block[i].size())/2
        int x=left_pos;
        while(x<16000&&read_map_all[i][x]==0) x++;
        if(x>right_pos) x=left_pos;//如果0个请求就从1开始
        printf("j %d\n",x);
        disk_point[i][point_id]=x;
        pre_token_disk[i][point_id]=0;
    }
    else if((read_map_all[i][disk_point[i][point_id]+G]-read_map_all[i][disk_point[i][point_id]]) == 0)
    {
        int x=disk_point[i][point_id]+G;
        while(x<16000&&(read_map_all[i][x]-read_map_all[i][disk_point[i][point_id]])==0) x++;
        if(x>right_pos)
        {
            x=left_pos;
            while(read_map_all[i][x]==0) x++;
            if(x>right_pos) x=left_pos;//如果0个请求就从1开始
        }//如果0个请求就从1开始
        printf("j %d\n",x);
        disk_point[i][point_id]=x;
        pre_token_disk[i][point_id]=0;
    }
    else{
        int start = disk_point[i][point_id];
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
                    int current_id=object[disk[i][disk_point[i][point_id]]].last_request_point;
                    int cnt=0;
                    while(current_id && cnt<20){
                        int bid=block_num[i][disk_point[i][point_id]];
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
                    disk_point[i][point_id] = (disk_point[i][point_id]+1)%V;
                }
                else break;
            }
        }
        else{ // 稀疏模式
            while(current_token > 0){
                if(read_map[i][disk_point[i][point_id]] == 1){ // 需要读取的块
                    int need = (pre_token==0 || pre_token==1) ? 64 : max(16, (pre_token*4)/5+1);
                    if(current_token >= need){
                        printf("r");
                        current_token -= need;
                        pre_token = need;
                        
                        // 处理读取请求
                        int current_id=object[disk[i][disk_point[i][point_id]]].last_request_point;
                        int cnt=0;
                        while(current_id && cnt<20){
                            int bid=block_num[i][disk_point[i][point_id]];
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
                        disk_point[i][point_id] = (disk_point[i][point_id]+1)%V;
                    }
                    else break;
                }
                else if(current_token >=1){ // 不需要读取的块
                    printf("p");
                    current_token -=1;
                    pre_token = 0;
                    disk_point[i][point_id] = (disk_point[i][point_id]+1)%V;
                }
                else break;
            }
        }
        pre_token_disk[i][point_id] = pre_token;
        printf("#\n");
    }
}
void do_read_action_point_my_2(int point_id,int left_pos,int right_pos,int i,int &n_resp)
{
    int pre_token=pre_token_disk[i][point_id];
    int current_token=G;
    if((read_map_all[i][right_pos]-read_map_all[i][disk_point[i][point_id]]==0)||disk_point[i][point_id]<left_pos){//disk_point[i]>=(disk_begin[i]-block_size*free_space_block[i].size())/2
        int x=left_pos;
        while(x<16000&&(read_map_all[i][x]-read_map_all[i][left_pos-1])==0) x++;
        if(x>right_pos) x=left_pos;//超过，说明是0个请求，所以放在盘一开始
        printf("j %d\n",x);
        disk_point[i][point_id]=x;
        pre_token_disk[i][point_id]=0;
    }
    else if((read_map_all[i][disk_point[i][point_id]+G]-read_map_all[i][disk_point[i][point_id]]) == 0)
    {
        int x=disk_point[i][point_id]+G;        
        while(x<16000&&(read_map_all[i][x]-read_map_all[i][disk_point[i][point_id]])==0) x++;
        if(x>right_pos)
        {
            x=left_pos;
            while((read_map_all[i][x]-read_map_all[i][left_pos-1])==0) x++;
            if(x>right_pos) x=left_pos;//如果0个请求就从1开始
        }//如果0个请求就从1开始
        printf("j %d\n",x);
        disk_point[i][point_id]=x;
        pre_token_disk[i][point_id]=0;
    }
    else{
        int start = disk_point[i][point_id];
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
                    int current_id=object[disk[i][disk_point[i][point_id]]].last_request_point;
                    int cnt=0;
                    while(current_id && cnt<20){
                        int bid=block_num[i][disk_point[i][point_id]];
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
                    disk_point[i][point_id] = (disk_point[i][point_id]+1)%V;
                }
                else break;
            }
        }
        else{ // 稀疏模式
            while(current_token > 0){
                if(read_map[i][disk_point[i][point_id]] == 1){ // 需要读取的块
                    int need = (pre_token==0 || pre_token==1) ? 64 : max(16, (pre_token*4)/5+1);
                    if(current_token >= need){
                        printf("r");
                        current_token -= need;
                        pre_token = need;
                        
                        // 处理读取请求
                        int current_id=object[disk[i][disk_point[i][point_id]]].last_request_point;
                        int cnt=0;
                        while(current_id && cnt<20){
                            int bid=block_num[i][disk_point[i][point_id]];
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
                        disk_point[i][point_id] = (disk_point[i][point_id]+1)%V;
                    }
                    else break;
                }
                else if(current_token >=1){ // 不需要读取的块
                    printf("p");
                    current_token -=1;
                    pre_token = 0;
                    disk_point[i][point_id] = (disk_point[i][point_id]+1)%V;
                }
                else break;
            }
        }
        pre_token_disk[i][point_id] = pre_token;
        printf("#\n");
    }
}
void read_action(int round, int time)
{
    int n_read;
    int request_id, object_id;
    // ofstream ofs;
    // ofs.open("mid.txt",ios::out|ios::app);
    if (round == 1) {
        scanf("%d", &n_read);
        n_reads[time] = n_read;
    } else {
        n_read = n_reads[time];
    }

    // int n_busy = n_read;
    // int reqs[n_read + 1];
    if(round==1)
    {
        int n_resp=0;
        int n_busy=0;
        vector<int> busy_request;
        for (int i = 1; i <= n_read; i++) {
            scanf("%d%d", &request_id, &object_id);
            read_datas[time].emplace_back(request_id, object_id);
            object_hot_datas[object_id][(time - 1 ) / 1800 + 1] += object[object_id].size;
            int tag = object[object_id].label;
            if (tag != 0) {
            int time_slice = (time - 1) / 1800 + 1;
            read_demand[tag][time_slice] += object[object_id].size;
            read_demand_round_1[tag]++;
            } 
            request[request_id].object_id = object_id;
            request[request_id].is_done=false;
            request[request_id].prev_id = object[object_id].last_request_point;
            request[request_id].t=time;
            request[request_id].state=0;
            object[object_id].last_request_point = request_id;
            int label = object[object_id].label;
            request_all++;
            if(find(label_need_read.begin(),label_need_read.end(),object[object_id].label)!=label_need_read.end()||object[object_id].unit[1][1]>(disk_block_num - free_space_block[object[object_id].replica[1]].size())*block_size){
                request_not_finish[request_id]=true;
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
        for(int i=1;i<=N;i++){
            do_read_action_point_my(1,1,disk_point_mid[i],i,n_resp);
            do_read_action_point_my_2(2,disk_point_mid[i]+1,disk_begin[i],i,n_resp);
        }
                
        printf("%d\n",n_resp);
                
        for(int j=1;j<=n_resp;j++) 
        {
            printf("%d\n",successful_request[j]);
            int object_id=request[successful_request[j]].object_id;
            request_not_finish[successful_request[j]]=false;
            int disk_id_tem = object[object_id].replica[1];
            for(int x = 1 ; x <=  object[object_id].size ;x++) read_map[disk_id_tem][object[object_id].unit[1][x]] = 0;
            // request_not_finish.erase(find(request_not_finish.begin(),request_not_finish.end(),successful_request[j]));
        }    
        while((request_not_finish[i_tmp]==false)&&(i_tmp<request_all)) i_tmp++;
        while((request[i_tmp].t<=time-105)&&(i_tmp<=request_all))
        {
            if((request_not_finish[i_tmp]==true))//&&request[i_tmp].is_done==false
            {
                n_busy++;
                busy_request.push_back(i_tmp);
                request[i_tmp].is_done=true;
                request_not_finish[i_tmp]=false;//已经被处理
            }
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
        }
        fflush(stdout);
    }
    else
    {
        int n_resp=0;
        int n_busy=0;
        vector<int> busy_request;
        for (int i = 1; i <= n_read; i++) {
            if (round == 1) {
                scanf("%d%d", &request_id, &object_id);
                read_datas[time].emplace_back(request_id, object_id);
                object_hot_datas[object_id][(time - 1 ) / 1800 + 1] += object[object_id].size;
                int tag = object[object_id].label;
                if (tag != 0) {
                int time_slice = (time - 1) / 1800 + 1;
                read_demand[tag][time_slice] += object[object_id].size;
                read_demand_round_1[tag]++;
                } 
            }
            else {
                const auto& read_tuple = read_datas[time].at(i - 1);
                request_id = get<0>(read_tuple);
                object_id = get<1>(read_tuple);
            }
            
            request[request_id].object_id = object_id;
            request[request_id].is_done=false;
            request[request_id].prev_id = object[object_id].last_request_point;
            request[request_id].t=time;
            request[request_id].state=0;
            object[object_id].last_request_point = request_id;
            int label = object[object_id].label;
            
            // if(label==1)
            // {
            //     ofstream ofs;
            //     ofs.open("./label_read.txt",ios::out|ios::app);
            //     ofs << object_id << " ";
            //     ofs.close();
            // }
            
            request_all++;
            if(find(label_need_read.begin(),label_need_read.end(),object[object_id].label)!=label_need_read.end()){
                request_not_finish[request_id]=true;
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
        for(int i=1;i<=N;i++){
            do_read_action_point_my(1,1,disk_point_mid[i],i,n_resp);
            do_read_action_point_my_2(2,disk_point_mid[i]+1,disk_begin[i],i,n_resp);
        }
                
        printf("%d\n",n_resp);
                
        for(int j=1;j<=n_resp;j++) 
        {
            printf("%d\n",successful_request[j]);
            int object_id=request[successful_request[j]].object_id;
            request_not_finish[successful_request[j]]=false;
            int disk_id_tem = object[object_id].replica[1];
            for(int x = 1 ; x <=  object[object_id].size ;x++) read_map[disk_id_tem][object[object_id].unit[1][x]] = 0;
            // request_not_finish.erase(find(request_not_finish.begin(),request_not_finish.end(),successful_request[j]));
        }    
        while((request_not_finish[i_tmp]==false)&&(i_tmp<request_all)) i_tmp++;
        while((request[i_tmp].t<=time-105)&&(i_tmp<=request_all))
        {
            if((request_not_finish[i_tmp]==true))//&&request[i_tmp].is_done==false
            {
                n_busy++;
                busy_request.push_back(i_tmp);
                request[i_tmp].is_done=true;
                request_not_finish[i_tmp]=false;//已经被处理
            }
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
        }
        ofstream ofs;
        // if(time%1800==1799){
        // ofs.open("./label_read.txt",ios::out|ios::app);
        // ofs << endl;
        // ofs.close();
        // }
        fflush(stdout);
        
    }
    // ofs.close();
    fflush(stdout);
}