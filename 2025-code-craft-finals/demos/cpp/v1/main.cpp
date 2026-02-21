#include"solutions.h"
void timestamp_action()
{
    int timestamp;
    scanf("%*s%d", &timestamp);
    printf("TIMESTAMP %d\n", timestamp);

    fflush(stdout);
}
void update_mid(int round)
{
    if(round==2){
    for(int i=1;i<=N;i++)
    {
        if(req_p1[i]*10>12*req_p2[i]&&disk_point_mid[i]>block_size*(req_p1[i]-req_p2[i])/(req_p2[i]+req_p1[i]))  
        {
            if(req_p2[i]==0){
            int disk_point_pos=((disk_point_mid[i]-1)/block_size+1)*block_size;
            bool flag=0;
            while(find(label_need_read.begin(),label_need_read.end(),disk_block[i][disk_point_pos/block_size])==label_need_read.end()) 
            {
                disk_point_pos=disk_point_pos-block_size;
                flag=1;
            }
            if(flag&&disk_point_pos>block_size)
                disk_point_mid[i]=disk_point_pos-block_size*(req_p1[i]-req_p2[i])/(req_p2[i]+req_p1[i]);
            else
                disk_point_mid[i]-=block_size*(req_p1[i]-req_p2[i])/(req_p2[i]+req_p1[i]);
            }
            else
                disk_point_mid[i]-=block_size*(req_p1[i]-req_p2[i])/(req_p2[i]+req_p1[i]);
        }
        else if(req_p2[i]*10>req_p1[i]*12&&disk_point_mid[i]<disk_begin[i]-block_size*free_space_block[i].size()-block_size*(req_p2[i]-req_p1[i])/(req_p2[i]+req_p1[i]))  
        {
            if(req_p1[i]==0){
            int disk_point_pos=((disk_point_mid[i]-1)/block_size)*block_size+1;
            bool flag=0;
            while(find(label_need_read.begin(),label_need_read.end(),disk_block[i][disk_point_pos/block_size+1])==label_need_read.end()) 
            {
                disk_point_pos=disk_point_pos+block_size;
                flag=1;
            }
            if(flag&&disk_point_pos<disk_begin[i]-block_size*free_space_block[i].size()-block_size)
                disk_point_mid[i]=disk_point_pos+block_size*(req_p2[i]-req_p1[i])/(req_p2[i]+req_p1[i]);
            else
                disk_point_mid[i]+=block_size*(req_p2[i]-req_p1[i])/(req_p2[i]+req_p1[i]);
            }
        
           disk_point_mid[i]+=block_size*(req_p2[i]-req_p1[i])/(req_p2[i]+req_p1[i]);
        }
        req_p1[i]=0;
        req_p2[i]=0;
    }
    }  
    else{
        for(int i=1;i<=N;i++)
        {
            if(req_p1[i]*10>12*req_p2[i]&&disk_point_mid[i]>block_size*(req_p1[i]-req_p2[i])/(req_p2[i]+req_p1[i]))  
            {
                disk_point_mid[i]-=block_size*(req_p1[i]-req_p2[i])/(req_p2[i]+req_p1[i]);
            }
            else if(req_p2[i]*10>req_p1[i]*10&&disk_point_mid[i]<disk_begin[i]-block_size*free_space_block[i].size()-block_size*(req_p2[i]-req_p1[i])/(req_p2[i]+req_p1[i]))  
            {
            disk_point_mid[i]+=block_size*(req_p2[i]-req_p1[i])/(req_p2[i]+req_p1[i]);
            }
            req_p1[i]=0;
            req_p2[i]=0;
        }
    }
}
void clean()
{
    for (auto& obj : object) {
        for (int i = 1; i <= REP_NUM; i++) {
            if (obj.unit[i] == nullptr)
                continue;
            free(obj.unit[i]);
            obj.unit[i] = nullptr;
        }
        // obj.last_request_point=0;
    }
    for(int disk_id=1;disk_id<=N;disk_id++)
    {
        for(int block_id=1;block_id<=disk_block_num;block_id++)
        {
            disk_label_able[disk_id][block_id].clear();
            free_space_block[disk_id].clear();
            disk_block[disk_id][block_id]=M+1;
            disk_block_time[disk_id][block_id]=0;
        }
        disk_begin[disk_id]=0;
        free_space[disk_id].clear();
    }
    label_need_read.clear();
    for(int i=0;i<=M;i++)
    {
        label_block[i].clear();
    }
    memset(block_information,0,sizeof(block_information));
    i_tmp=0;
    request_all=0;
    
}
void incre_action()
{
    int n_incre;
    scanf("%d", &n_incre);
    for (int i = 1; i <= n_incre; ++i) {
    	int object_id;
    	int object_tag;
        scanf("%d%d",&object_id,&object_tag);
        object[object_id].label = object_tag;
        for(int i = 1 ; i <= 48 ; i++){
        	//read_demand[object_tag][i] += object_hot_datas[object_id][i];
		}
    }
}
void update(int round)
{
    if(round==1)
    {
        
        if(time_stemp<=0){
            for(int disk_id = 1 ; disk_id <= 10 ; disk_id++){
                for(int block_id = 1 ; block_id <= disk_block_num ; block_id++){
                    if( find(label_need_read.begin(),label_need_read.end(),disk_block[disk_id][block_id])!=label_need_read.end()){
                        block_information[disk_id][time_stemp].all ++ ;
                    }
                }
            }
            int free_to_divide=V-V*0.9*2/3+k_new;
            block_size=free_to_divide/disk_block_num;
            for(int disk_id = 1 ; disk_id <= 10 ; disk_id++){
                int cnt = 0;
                for(int block_id = 1 ; block_id <= disk_block_num ; block_id++){
                    if( find(label_need_read.begin(),label_need_read.end(),disk_block[disk_id][block_id])!=label_need_read.end()){
                        cnt++;
                        if(cnt == 1) 	block_information[disk_id][time_stemp].start = block_id;
                        if(cnt == (block_information[disk_id][time_stemp].all  / 2 + 1)) block_information[disk_id][time_stemp].middle = block_id;
                        if(cnt == block_information[disk_id][time_stemp].all ) block_information[disk_id][time_stemp].end = block_id;
                    }
                }
                if(block_information[disk_id][time_stemp].all == 0 || block_information[disk_id][time_stemp].all == 1)disk_point_mid[disk_id] = max( (block_information[disk_id][time_stemp].middle ) * block_size - block_size/2 , 1);
                else {
                if(time_stemp <= 24 )disk_point_mid[disk_id] = (block_information[disk_id][time_stemp].middle ) * block_size - block_size/2;
                else disk_point_mid[disk_id] = (block_information[disk_id][time_stemp].middle ) * block_size;
                }
            }
        }
        else
        {
            for(int disk_id=1;disk_id<=N;disk_id++)
            {
                // if(time_stemp<=20)
                //     disk_point_mid[disk_id]=4300;
                // else
                //     disk_point_mid[disk_id]=4270;
                if(req_p1[disk_id]+req_p1[disk_id]>1500)
                    disk_point_mid[disk_id]=4280;
                else
                    disk_point_mid[disk_id]=4300;
            }
        }
        for(int i=1;i<=N;i++)
        {
            req_p1[i]=0;
            req_p2[i]=0;
        }
    }
    else;  
        
}
void update_label_need_read_round_1()
{
    if(time_stemp>20&&time_stemp<=48)//时间超过15才去更新Label_need_read
    {
        int read_demand_all_label_tmp=0;
        int demand_all_label_tmp=0;
        for(int i=1;i<=M;i++)
            xjb_before[i]=xjb[i];
        for(int i=1;i<=M;i++)
        {
            xjb[i]=100*read_demand_round_1[i]/label_exist[i];
            read_demand_all_label_tmp+=read_demand_round_1[i];
            demand_all_label_tmp+=label_exist[i];
        }
        xjb_avr=100*read_demand_all_label_tmp/demand_all_label_tmp;
        
        label_need_read.clear();
        label_need_read.insert(0);
        for(int i=1;i<=M;i++)
        {
            if(xjb[i]>=xjb_avr)
                label_need_read.insert(i);
        }
    }
    for(int i=1;i<=M;i++)
    {
        read_demand_round_1[i]=0;
    }
}
void match_action(){
    // printf("matchin!");
	ofstream ofs;
	ofs.open("log_2.txt",ios::out|ios::app);

	for(int object_id = 1 ; object_id <= last_object ; object_id++){
		int object_remand_all = 0;
        if(object[object_id].label) continue;
		for(int time = 1 ; time <= 48 ; time++ ) {
			object_remand_all += object_hot_datas[object_id][time];
		}
        if( object_remand_all <= 90 ) {
		   object[object_id].label = 0;
		   continue;
		}
		for(int time = object[object_id].write_time ; time <= object[object_id].delete_time ; time++){
		  if(object_remand_all)	object_wanfenbi[object_id][time] = object_hot_datas[object_id][time] * 10000 / object_remand_all;
		}
        for(int label = 1 ; label <= 16 ;label++){
			read_demand_all_tem[label] = 0;
			for(int time = object[object_id].write_time ; time <= object[object_id].delete_time ; time++){
				read_demand_all_tem[label] += read_demand[label][time];
				}
		}
		
		int predict_tag = 0;
		int min_hot = 99999999;
		for(int label_id = 1 ; label_id <= 16 ;label_id++){
			int hot = 0;
			for(int time = object[object_id].write_time ; time <= object[object_id].delete_time ;time++){
				hot +=  abs( read_demand[label_id][time] * 10000 / read_demand_all_tem[label_id] -  object_wanfenbi[object_id][time] ) ;
			}
			object_hot_label[object_id][label_id] = hot;
			if(hot < min_hot){
				min_hot = hot;
				predict_tag = label_id;
			}
		}
		object[object_id].label = predict_tag;
		
	
		ofs << object_id << endl;
		for(int i=1;i<=M;i++)
			ofs << object_hot_label[object_id][i] << " ";
		ofs << endl;
	}
	ofs.close();
	
}
void calculate_demand() // 每个label实时存在的块数
{
	int flag = 0;
	    
        for(int i=0;i<=M;i++)
        {
        	int maxnum = 0;
            delete_demand[i][0]=0;
            demand[i][0]=0;
            for(int j=1;j<=time_stamp_all;j++){
			demand[i][j]=(long int)(write_demand[i][j] + demand[i][j-1]) - delete_demand[i][j-1];
			if(demand[i][j] > maxnum ) {
			    maxnum = demand[i][j];
			  }
			}
			max_demand[i] =  {maxnum , i};
			max_demand_beifen[i]=max_demand[i].first;   
        }
        
    for(int time_stamp=1;time_stamp <= time_stamp_all ;time_stamp++)
    {
        for(int i=1;i<=M;i++)
            demand_all_label[time_stamp]+=demand[i][time_stamp];
        if(demand_all_label[time_stamp] * 3 > V * 8   && !flag) {
        	preallocation_time = time_stamp;
            preallocation_time += 4;
        	flag = 1;
		}
    }
    max_demand_all_label=*max_element(demand_all_label+1,demand_all_label+48);
}

void initialize()
{
	// hot_allocate();
	// xjb_allocate();
    int free_to_divide=V-V*0.9*2/3+k_new;
    block_size=free_to_divide/disk_block_num;
    
    for(int disk_id=1;disk_id<=N;disk_id++)
    {
        for(int block_id=1;block_id<=disk_block_num;block_id++)
        {
            for(int i=(block_id-1)*block_size+1;i<=block_size*block_id;i++)
                disk_label_able[disk_id][block_id].insert(i);
            free_space_block[disk_id].insert(block_id);
            disk_block[disk_id][block_id]=M+1;
        }
        disk_begin[disk_id]=block_size*disk_block_num+1;
    }
    for(int disk_id=1;disk_id<=N;disk_id++)
    {
        for(int x=disk_begin[disk_id];x<=V;x++)
            free_space[disk_id].insert(x);
    }
    for(int i=0;i<=M;i++)
    {
        label_need_read.insert(i);
    }
    for(int i=1;i<=N;i++)
    {
        for(int j=1;j<=M;j++)
        {
            int label_tmp=(i+j-2)%M+1;
            label_block[label_tmp].insert({j,i});
            disk_block[i][j]=label_tmp;
            free_space_block[i].erase(find(free_space_block[i].begin(),free_space_block[i].end(),j));
        }
    }
	// preallocation();

}
void xjb_allocate(){
	int xjb[49][MAX_LABEL_NUM];
    for(int time = 1 ; time <= 48 ;time++){
	    for(int i=1;i<=M;i++)
	    {
	        xjb[time][i]=read_demand[i][time]* 100/demand[i][time] ;
	    }
	}
    
	for(int i = 1 ;i <= M ;i++){
		for(int j = 1 ; j <= M ;j++){
			for(int k = 1; k <= (T - 1) / FRE_PER_SLICING + 1; k++ ){
				xjb_company[i][j] += (xjb[k][i] / 100)* (xjb[k][j] / 100);
			}
			xjb_rank[i][j] = {xjb_company[i][j] , j };
		}
		sort(xjb_rank[i]+1,xjb_rank[i]+M+1);
	}
	
}
void update()
{
    
    read_demand_all_label=0;
    for(int i=1;i<=M;i++)
    {
        xjb[i]=100*read_demand[i][time_stemp]/demand[i][time_stemp];
        read_demand_all_label+=read_demand[i][time_stemp];
    }
    xjb_avr=78*read_demand_all_label/demand_all_label[time_stemp];
    
    label_need_read.clear();
    for(int i=1;i<=M;i++)
    {
        if(xjb[i]>=xjb_avr)
            label_need_read.insert(i);
    }
    // for(int i=1;i<=N;i++)
    // {
    //     req_p1[i]=0;
    //     req_p2[i]=0;
    // }
	// if(time_stemp<=100){
	// for(int disk_id = 1 ; disk_id <= 10 ; disk_id++){
    // 	for(int block_id = 1 ; block_id <= disk_block_num ; block_id++){
    // 		if( find(label_need_read.begin(),label_need_read.end(),disk_block[disk_id][block_id])!=label_need_read.end()){
    // 			block_information[disk_id][time_stemp].all ++ ;
	// 		}
	// 	}
	// }
	// int free_to_divide=V-10500;
    // block_size=free_to_divide/disk_block_num;
	// for(int disk_id = 1 ; disk_id <= 10 ; disk_id++){
	// 	int cnt = 0;
    // 	for(int block_id = 1 ; block_id <= disk_block_num ; block_id++){
    // 		if( find(label_need_read.begin(),label_need_read.end(),disk_block[disk_id][block_id])!=label_need_read.end()){
    // 			cnt++;
    // 			if(cnt == 1) 	block_information[disk_id][time_stemp].start = block_id;
    // 			if(cnt == (block_information[disk_id][time_stemp].all  / 2 + 1)) block_information[disk_id][time_stemp].middle = block_id;
    // 			if(cnt == block_information[disk_id][time_stemp].all ) block_information[disk_id][time_stemp].end = block_id;
	// 		}
	// 	}
	// 	if(block_information[disk_id][time_stemp].all == 0 || block_information[disk_id][time_stemp].all == 1)disk_point_mid[disk_id] = max( (block_information[disk_id][time_stemp].middle ) * block_size - block_size/2 , 1);
	// 	else {
	// 	   if(time_stemp <= 24 )disk_point_mid[disk_id] = (block_information[disk_id][time_stemp].middle ) * block_size - block_size/2;
	// 	   else disk_point_mid[disk_id] = (block_information[disk_id][time_stemp].middle ) * block_size;
	// 	}
	// }
    // }
    // else
    // {
    //     for(int disk_id = 1 ; disk_id <= 10 ; disk_id++){
    // 	for(int block_id = 1 ; block_id <= disk_block_num ; block_id++){
    // 		if( find(label_need_read.begin(),label_need_read.end(),disk_block[disk_id][block_id])!=label_need_read.end()){
    // 			block_information[disk_id][time_stemp].all += read_demand[disk_block[disk_id][block_id]][time_stemp]/((block_size-disk_label_able[disk_id][block_id].size()/demand[disk_block[disk_id][block_id]][time_stemp])) ;
	// 		}
	// 	}
	// }
    // int free_to_divide=V-10500;
    // block_size=free_to_divide/disk_block_num;
	// for(int disk_id = 1 ; disk_id <= 10 ; disk_id++){
	// 	int cnt = 0;
    //     bool flag=false;
    // 	for(int block_id = 1 ; block_id <= disk_block_num ; block_id++){
    // 		if( find(label_need_read.begin(),label_need_read.end(),disk_block[disk_id][block_id])!=label_need_read.end()){
    // 			cnt+=read_demand[disk_block[disk_id][block_id]][time_stemp]/((block_size-disk_label_able[disk_id][block_id].size()/demand[disk_block[disk_id][block_id]][time_stemp])) ;
    // 			if(cnt > (block_information[disk_id][time_stemp].all  / 2 + 1)&&!flag) {block_information[disk_id][time_stemp].middle = block_id; flag=true;}
    // 			if(cnt == block_information[disk_id][time_stemp].all ) block_information[disk_id][time_stemp].end = block_id;
	// 		}
	// 	}
	// 	if(block_information[disk_id][time_stemp].all == 0 || block_information[disk_id][time_stemp].middle == block_information[disk_id][time_stemp].end)disk_point_mid[disk_id] = max( (block_information[disk_id][time_stemp].middle ) * block_size - block_size/2 , 1);
	// 	else {
	// 	   if(time_stemp <= 24 )disk_point_mid[disk_id] = (block_information[disk_id][time_stemp].middle ) * block_size;
	// 	   else disk_point_mid[disk_id] = (block_information[disk_id][time_stemp].middle ) * block_size;
	// 	}
	// }
    // }
        
        
}

void preallocation(){  // 预分配每个label的片片
	int st_preallocation[17];
    for(int i = 0 ; i <= M ;i++) st_preallocation[i] = 0;
    st_preallocation[1] = 1;
    int last_label = 1;
    preallocation_label[1] = 1;
    for(int i = 2 ; i <= M ;i++){
    	for(int j = M ; j >= 1 ; j--){
    		int want_allocation_label = xjb_rank[last_label][j].second;
    		if( !st_preallocation[want_allocation_label]){
    			st_preallocation[want_allocation_label] = 1;
    			last_label = want_allocation_label;
    			preallocation_label[i] = want_allocation_label;
    			break;
			}
		}
	}
	
    int now_block = 1;
    int now_disk_id = 1;
    for(int i = M ;i >= 1 ; i--){
    	int want_allocation_label = preallocation_label[i];
    	int demand_preallocation = demand[want_allocation_label][preallocation_time];
    	int cnt = demand_preallocation / block_size ; 
    	
    	while(cnt){
            free_space_block[now_disk_id].erase(find(free_space_block[now_disk_id].begin(),free_space_block[now_disk_id].end(),now_block));
    		label_block[want_allocation_label].insert({now_disk_id , now_block}) ;
            disk_block[now_disk_id][now_block]=want_allocation_label;
    		cnt--;
    		now_disk_id++;
    		if(now_disk_id > 10) {
    			now_block++;
    			now_disk_id = 1;
			}
		}
		
	}
}
void initialize_2()
{
	xjb_allocate();
    ofstream ofs;
    ofs.open("LOG_1.txt",ios::out|ios::app);
    // int free_to_divide=V-V*0.6-k_new_2;
    int free_to_divide=V-10500;
    block_size=free_to_divide/disk_block_num;
    ofs << free_to_divide << " " << block_size << endl;
    
    for(int disk_id=1;disk_id<=N;disk_id++)
    {
        for(int block_id=1;block_id<=disk_block_num;block_id++)
        {
            for(int i=(block_id-1)*block_size+1;i<=block_size*block_id;i++)
                disk_label_able[disk_id][block_id].insert(i);
            free_space_block[disk_id].insert(block_id);
        }
        disk_begin[disk_id]=block_size*disk_block_num+1;
    }
    for(int disk_id=1;disk_id<=N;disk_id++)
    {
        for(int x=disk_begin[disk_id];x<=V;x++)
            free_space[disk_id].insert(x);
    }
    for(int i=1;i<=MAX_STMP;i++){
    	for(int label_id=1;label_id<MAX_LABEL_NUM;label_id++){
    		read_sum[i]+=read_demand[label_id][i];
		}
	}
    ofs << "OK2" << endl;
    for(int time=1;time<=48;time++)
    {
        read_demand_all_label=0;
        for(int i=1;i<=M;i++)
        {
            xjb[i]=100*read_demand[i][time]/demand[i][time];
            read_demand_all_label+=read_demand[i][time];
        }
        xjb_avr=75*read_demand_all_label/demand_all_label[time];
        
        label_need_read_time[time].clear();
        for(int i=1;i<=M;i++)
        {
            if(xjb[i]>=xjb_avr)
                label_need_read_time[time].insert(i);
        }
    }
    ofs << "OK3" << endl;
	preallocation();
    ofs << "OK4" << endl;
    ofs.close();
}
void test_label_update(int round)
{
    ofstream ofs;
    ofs.open("./label.txt",ios::out|ios::app);
    ofs << "time: " << time_stemp << " round:" << round << endl;
    for(int i=1;i<=41000;i++){
        if(object[i].write_time!=0)
            ofs << i << " " << object[i].label << endl;
    }
    ofs.close();
}
void test_demand()
{
    ofstream ofs;
    ofs.open("./demand.txt",ios::out|ios::app);
    ofs << "read_demand:" << endl;
    for(int i=0;i<=M;i++)
    {
        for(int time=1;time<=48;time++)
        {
            ofs << read_demand[i][time] <<" " ;
        }
        ofs << endl;
    }
    ofs << "delete_demand:" << endl;
    for(int i=0;i<=M;i++)
    {
        for(int time=1;time<=48;time++)
        {
             ofs << delete_demand[i][time] <<" " ;
        }
        ofs << endl;
    }
    ofs << "write_demand:" << endl;
    for(int i=0;i<=M;i++)
    {
        for(int time=1;time<=48;time++)
        {
             ofs << write_demand[i][time] <<" " ;
        }
        ofs << endl;
    }
    ofs << "demand:" << endl;
    for(int i=0;i<=M;i++)
    {
        for(int time=1;time<=48;time++)
        {
             ofs << demand[i][time] <<" " ;
        }
        ofs << endl;
    }
    ofs.close();

}
void test_label_need_read()
{
    ofstream ofs;
    ofs.open("./test_label_need_read.txt",ios::out|ios::app);
    ofs << "time:" << time_stemp << endl;
    for(set<int>::iterator it=label_need_read.begin();it!=label_need_read.end();it++)
    {
        ofs << *it << " ";
    }
    ofs << endl;
    ofs.close();
}
void pre_init()
{
    int cnt_del=0;
    int cnt_write=0;
    for(int time=1;time<=MAX_TIME_SLICING;time++)
    {
        for(int i=0;i<n_deletes[time];i++)
        {
            int object_id=delete_datas[time][i];
            delete_demand[object[object_id].label][(time-1)/1800+1]+=object[object_id].size;
            cnt_del++;
        }
        for(int i=1;i<=n_reads[time];i++)
        {
            const auto& read_tuple = read_datas[time].at(i - 1);
            int object_id = get<1>(read_tuple);
            read_demand[object[object_id].label][(time-1)/1800+1]+=object[object_id].size;
        }
        for(int i=1;i<=n_writes[time];i++)
        {
            const auto& write_tuple = write_datas[time].at(i - 1);
            int object_id = get<0>(write_tuple);
            write_demand[object[object_id].label][(time-1)/1800+1]+=object[object_id].size;
            cnt_write++;
        }
    }
    ofstream ofs;
    ofs.open("./log_6.txt",ios::out|ios::app);
    ofs << cnt_del << " " << cnt_write << cnt_write-cnt_del <<  endl;
    ofs.close();
}


void do_predict_my(int object_id , int t){
	int object_remand_all = 0;
	int object_wanfenbi_now[49];
	int read_demand_all_now[17];
	int object_hot_label_now[17];
	ofstream ofs;
    ofs.open("log_0416.txt",ios::out|ios::app);
		for(int time = object[object_id].write_time ; time <= min(object[object_id].delete_time , (t - 1) / 1800 + 1 ) ; time++){
			object_remand_all += object_hot_datas[object_id][time];
		}
		
		if(object_remand_all <= 340) {
		   //object[object_id].label = 0;
		   return;
		}
		
		for(int time = object[object_id].write_time ; time <= min(object[object_id].delete_time , (t - 1) / 1800 + 1 ) ; time++){
		    if(object_remand_all)	object_wanfenbi_now[time] = object_hot_datas[object_id][time] * 10000 / object_remand_all;
        }
		
		for(int label = 1 ; label <= 16 ;label++){
			read_demand_all_now[label] = 0;
			for(int time = object[object_id].write_time ; time <= min(object[object_id].delete_time , (t - 1) / 1800 + 1 ) ; time++){
				read_demand_all_now[label] += read_demand[label][time];
				}
		}
		
		int predict_tag = 0;
		int min_hot = 99999999;
		for(int label_id = 1 ; label_id <= 16 ;label_id++){
			int hot = 0;
			for(int time = object[object_id].write_time ; time <= min(object[object_id].delete_time , (t - 1) / 1800 + 1 ) ; time++){
				hot +=  abs( read_demand[label_id][time] * 10000 / read_demand_all_now[label_id] -  object_wanfenbi_now[time] ) ;
			}
			object_hot_label_now[label_id] = hot;
			if(hot < min_hot){
				min_hot = hot;
				predict_tag = label_id;
			}
		}
		if(min_hot <= 8000)  {
		  if(object[object_id].label == 0&&object_id!=16262&&object_id!=16259){
		    object[object_id].label = predict_tag;
		    object[object_id].is_predict = 1;
		    for(int time = object[object_id].write_time ; time <= min(object[object_id].delete_time , (t - 1) / 1800 + 1 ) ; time++){
        	   read_demand[predict_tag][time] += object_hot_datas[object_id][time];
			}
		  }
		  else if(object[object_id].label != predict_tag){
		  	int old_tag = object[object_id].label;
		  	for(int time = object[object_id].write_time ; time <= min(object[object_id].delete_time , (t - 1) / 1800 + 1 ) ; time++){
        	   read_demand[old_tag][time] -= object_hot_datas[object_id][time];
			}
			object[object_id].label = predict_tag;
		    for(int time = object[object_id].write_time ; time <= min(object[object_id].delete_time , (t - 1) / 1800 + 1 ) ; time++){
        	   read_demand[predict_tag][time] += object_hot_datas[object_id][time];
		  }
		  
		}
		
	}
    ofs.close();
}

void round_1_predict(int t){
	int k = 15;
	if(t <= k * 1800) return ;
    ofstream ofs;
    ofs.open("log_0416.txt",ios::out|ios::app);
    
	for(int time = 1 ; time <= t - k*1800 ;time++){
		int n_write = n_writes[time];
		for(int i = 1 ; i <= n_write ;i++){
			int id, size, tag;
			const auto& write_tuple = write_datas[time].at(i - 1);
            
            id = get<0>(write_tuple);
            size = get<1>(write_tuple);
            tag = get<2>(write_tuple);
            if(tag && object[id].is_predict == 0) continue;
            do_predict_my(id , t); 
		}	
	}
    ofs.close();
}

void clean_predict(){
	for(int object_id = 1 ; object_id <= last_object ; object_id++){
		if(object[object_id].is_predict  && object[object_id].label){
			int old_tag = object[object_id].label;
			for(int time = object[object_id].write_time ; time <= object[object_id].delete_time ; time++){
        	   read_demand[old_tag][time] -= object_hot_datas[object_id][time];
			}
			object[object_id].label = 0;
		}
	}
}
void gc_before_after_predict()//主要用于block易主
{
    if(time_stemp>=25&&time_stemp<=30){
    for(int disk_id=1;disk_id<=N;disk_id++)
    {
        for(int block_id=1;block_id<=disk_block_num;block_id++)
        {
            if(disk_block[disk_id][block_id]!=0||disk_block[disk_id][block_id]==17) continue;
            // 计数，如果某label数量炒作总大小的80%就易主
            int cnt[MAX_LABEL_NUM];
            for(int i=(block_id-1)*block_size+1;i<=block_id*block_size;i++)
            {
                if(object[disk[disk_id][i]].label!=17) cnt[object[disk[disk_id][i]].label]++;
            }
            for(int i=1;i<=M;i++)
            {
                if(cnt[i]>=block_size*0.8)
                    disk_block[disk_id][block_id]=i;
            }
        }
    }
    }
}
void test_req()
{
    std::ofstream outFile;
    outFile.open("./test_req.txt", std::ios::out|std::ios::app); // 以追加模式打开
    for(int i=1;i<=N;i++)
        outFile << req_p1[i] << " "<< req_p2[i] << endl;
    outFile.close();
}
int request_map[MAX_DISK_NUM][16400];
int request_map_all[MAX_DISK_NUM][16400];
void update_mid_new(int t)//根据已知信息分配磁头
{
    memset(request_map,0,sizeof(request_map));
    for(int time=t;time<t+600;time++)
    {
        for(int i=1;i<=n_reads[time];i++)
        {
            const auto& read_tuple = read_datas[time].at(i - 1);
            int object_id = get<1>(read_tuple);
            if(object[object_id].write_time_2 >= t) continue;
            for(int j=1;j<=object[object_id].size;j++){
                if(disk[object[object_id].replica[1]][object[object_id].unit[1][j]]!=object_id) break;
                request_map[object[object_id].replica[1]][object[object_id].unit[1][j]]++;
            }
        }
    }
    //计算前缀和
    for(int disk_id = 1 ; disk_id <= 10 ; disk_id++ ){
        for(int i = 1 ; i <= disk_begin[disk_id] ; i++){
            request_map_all[disk_id][i] = request_map_all[disk_id][i-1] + request_map[disk_id][i];
        }
    }
   //计算Mid
//    int mid;
   for(int disk_id = 1 ; disk_id <= 10 ; disk_id++ ){
        for(int i = 1 ; i <= disk_begin[disk_id] ; i++){
            if( request_map_all[disk_id][i] > request_map_all[disk_id][disk_begin[disk_id]]/2 ){
                disk_point_mid[disk_id] = i;
                break;
            }
        }
        if(disk_point_mid[disk_id]==0)
        {
            disk_point_mid[disk_id]=disk_begin[disk_id]/2;
        }
    }
    for(int disk_id = 1 ; disk_id <= 10 ; disk_id++ )
    {
        req_p1[disk_id]=0;
        req_p2[disk_id]=0;
    }

}

int main()
{
    scanf("%d%d%d%d%d%d", &T, &M, &N, &V, &G, &K);
    time_stamp_all= T/FRE_PER_SLICING;
    initialize();
    printf("OK\n");
    fflush(stdout);
    object[0].label=17;

    for (int i = 1; i <= N; i++) {
        disk_point[i][1] = 1;
        disk_point[i][2] = 1;
        pre_token_disk[i][1] = 0;
        pre_token_disk[i][2] = 0;
    }

    for (int t = 1; t <= T + EXTRA_TIME; t++) {
        timestamp_action();
        time_stemp=t/FRE_PER_SLICING+1;
        if(t%FRE_PER_SLICING == 1&&t!=86401) {test_req(); update(1);}
        if(t%100==99) { test_req(); update_mid(1);}
        if(t%600 == 1){update_label_need_read_round_1(); test_label_need_read();}
        delete_action(1, t);
        write_action(1, t);
        read_action(1, t);
        if (t % FRE_PER_SLICING == 0) {
        	if(t >= 15 * 1800) round_1_predict(t);
            // test_label_update(1);
            // gc_before_after_predict();
            gc_action(1);
        }
    }
    ofstream ofs;
    ofs.open("LOG_1.txt",ios::out|ios::app);
    
    
    clean_predict();
    incre_action();
    clean();
    memset(disk, 0, sizeof(disk));
    memset(read_map, 0, sizeof(read_map));
    memset(read_map_all,0,sizeof(read_map_all));
    memset(block_num, 0, sizeof(block_num));
    memset(request_not_finish,false,sizeof(request_not_finish));
    memset(disk_point_mid,0,sizeof(disk_point_mid));
    memset(disk_begin,0,sizeof(disk_begin));
    
    for (int i = 1; i <= N; i++) {
        disk_point[i][1] = 1;
        disk_point[i][2]=1;
        pre_token_disk[i][1]=0;
        pre_token_disk[i][2]=0;
        req_p1[i]=0;
        req_p2[i]=0;
    }
    
    match_action();
    
	ofs.close();
    // test_label_update(2);
    memset(read_demand, 0, sizeof(read_demand));
    memset(write_demand, 0, sizeof(write_demand));
    memset(delete_demand, 0, sizeof(delete_demand));
    memset(demand, 0, sizeof(demand));
    memset(xjb,0,sizeof(xjb));
    
    xjb_avr=0;
    pre_init();
    calculate_demand();
    initialize_2();
    for (int t = 1; t <= T + EXTRA_TIME; t++) {
        time_stemp=t/FRE_PER_SLICING+1;
        if(t%FRE_PER_SLICING == 1&&t!=86401) {update();test_label_need_read();}
        
        if(t%600==1&&time_stemp>=6) { update_mid_new(t);}
        if(t%100==99&&time_stemp<=5) { update_mid(2);}
        timestamp_action();
        delete_action(2,t);//没改
        write_action(2,t);
        read_action(2,t);
        if (t % FRE_PER_SLICING == 0) {
            gc_action(2);
        }
    }
    clean();
    ofs.close();
    return 0;
}