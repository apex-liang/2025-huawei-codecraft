#include"solutions.h"
void timestamp_action()
{
    int timestamp;
    scanf("%*s%d", &timestamp);
    printf("TIMESTAMP %d\n", timestamp);

    fflush(stdout);
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
    }
}
void calculate_demand() // 每个label实时存在的块数
{
	int flag = 0;
	    
        for(int i=1;i<=M;i++)
        {
        	int maxnum = 0;
            delete_demand[0][i]=0;
            demand[0][i]=0;
            for(int j=1;j<time_stamp_all;j++){
			demand[i][j]=(long int)(write_demand[j][i] + demand[i][j-1]) - delete_demand[j-1][i];
			if(demand[i][j] > maxnum ) {
			    maxnum = demand[i][j];
			  }
			}
			max_demand[i] =  {maxnum , i};
			max_demand_beifen[i]=max_demand[i].first;   
        }
        
    for(int time_stamp=1;time_stamp < time_stamp_all ;time_stamp++)
    {
        for(int i=1;i<=M;i++)
            demand_all_label[time_stamp]+=demand[i][time_stamp];
        if(demand_all_label[time_stamp] * 3 > V * 8 && !flag) {
        	preallocation_time = time_stamp;
        	flag = 1;
		}
    }
    max_demand_all_label=*max_element(demand_all_label+1,demand_all_label+48);
}



bool cmp_demand(pair<long int,long int> label1,pair<long int,long int> label2)
{
    return label1.first>label2.first;
}

void preallocation(){  // 预分配每个label的片片
    int now_block = 1;
    int now_disk_id = 1;
    for(int i = 1 ;i <= M ; i++){
    	int demand_preallocation = demand[i][preallocation_time];
    	int cnt = demand_preallocation / block_size ; 
    	
    	while(cnt){
            free_space_block[now_disk_id].erase(find(free_space_block[now_disk_id].begin(),free_space_block[now_disk_id].end(),now_block));
    		label_block[i].insert({now_disk_id , now_block}) ;
            disk_block[now_disk_id][now_block]=i;
    		cnt--;
    		now_disk_id++;
    		if(now_disk_id > 10) {
    			now_block++;
    			now_disk_id = 1;
			}
		}
		
	}
}
void update()
{
    label_need_read.clear();
    for(int i=1;i<=M;i++)
    {
        if(read_demand[time_stemp][i]>=read_sum[time_stemp]/30)
            label_need_read.insert(i);
    }
}
void initialize()
{
    int k=20;//预留空间
    int free_to_divide=V-max_demand_all_label*2/N-k;
    block_size=free_to_divide/disk_block_num;
    
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
    		read_sum[i]+=read_demand[i][label_id];
		}
	}
	preallocation();
}
void test()
{
    std::ofstream outFile;
    outFile.open("./example_log.txt", std::ios::out|std::ios::app); // 以追加模式打开
    outFile << "TIME_STEMP" << time_stemp << ": ";
    for(int i=1;i<=N;i++){
    for(int j=1;j<=disk_block_num;j++)
        outFile << disk_block[i][j] << " " ; 
        outFile << endl;
    }
}
int main()
{
    scanf("%d%d%d%d%d%d", &T, &M, &N, &V, &G, &K);
    time_stamp_all= T/FRE_PER_SLICING;

   for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%d",&delete_demand[j][i]);
        }
    }

    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%d",&write_demand[j][i]);
        }
    }

    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%d",&read_demand[j][i]);
        }
    }
    calculate_demand();
    initialize();
    printf("OK\n");
    fflush(stdout);

  

    for (int i = 1; i <= N; i++) {
        disk_point[i] = 1;
        disk_point_2[i] = 1;
    }

    for (int t = 1; t <= T + EXTRA_TIME; t++) {
        time_stemp=t/FRE_PER_SLICING+1;
        if(t == 0 ) printf("%d ",preallocation_time) ;
        if(t%FRE_PER_SLICING==1) update();
        timestamp_action();
        delete_action();
        write_action();
        read_action(t);
        if (t % FRE_PER_SLICING == 0) {
            gc_action();
        }
    }
    clean();
    
    return 0;
}