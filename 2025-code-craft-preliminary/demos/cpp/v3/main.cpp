#include"solutions.h"
#include"utils.h"
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
void initialize_partitions(int N, int V, int M) {
    for (int disk_id = 1; disk_id <= N; disk_id++) {
        int partition_size = V / TEST_BLOCK_DIVIDE;  // 扇区大小
        for (int label = 1; label <= TEST_BLOCK_DIVIDE; label++) {
            int start = (label - 1) * partition_size + 1;
            int end = label * partition_size;
            label_range[disk_id][label] = {start, end};
            for (int i = start; i <= end; i++) {
                free_blocks[disk_id][label].insert(i);  // 初始化空闲块
            }
        }
    }
}
int time_stamp_all;
int max_demand_beifen[MAX_LABEL_NUM];
int demand_two[MAX_DISK_NUM][MAX_TIMESTAMP];
int max_demand_two[MAX_DISK_NUM];
void calculate_demand()
{
    for(int time_stamp=1;time_stamp < time_stamp_all ;time_stamp++)
    {
        for(int i=1;i<=M;i++)
        {   int max_num = 0;
            int top_time = 0;
            delete_demand[0][i]=0;
            demand[0][i]=0;
            for(int j=1;j<=time_stamp;j++){
			demand[i][j]=(long int)(write_demand[j][i] + demand[i][j-1]) - delete_demand[j-1][i];
                if(demand[i][j] > max_num){
                	max_num = demand[i][j];
                	top_time = j;
				}
			}  
			label_top_time[i] = top_time;
        }
    }
    
    for(int i=1;i<=M;i++)
    {
        max_demand[i]={*max_element(demand[i],demand[i]+MAX_STMP),i};
        max_demand_beifen[i]=max_demand[i].first;

    }
}
bool cmp_demand(pair<long int,long int> label1,pair<long int,long int> label2)
{
    return label1.first>label2.first;
}
void calculate_two_demand()//计算每个磁盘匹配到的两个label的最大存量需求
{
        for(int disk_id=1;disk_id<=N;disk_id++)
        {

            int label1=disk_label[disk_id][0].first;
            int label2=disk_label[disk_id][1].first;
            for(int j=1;j<=time_stamp_all;j++)
            {
                if(disk_id<=8)//前8个磁盘的第一个扇区的label是被分成了两个备份存储的
                    demand_two[disk_id][j]=demand[label1][j]/2+1+demand[label2][j];  
                else
                    demand_two[disk_id][j]=demand[label1][j]+demand[label2][j];  
            }
                
            
        }
    // }
    for(int i=1;i<=N;i++)
    {
        max_demand_two[i]=*max_element(demand_two[i],demand_two[i]+MAX_STMP);
    }
    
}


void hot_disk_allocate(){
	for(int i = 1 ;i <= N ;i++){
		int label1=disk_label[i][0].first;
        int label2=disk_label[i][1].first; 
		for(int j = 1 ; j <= 48 ;j++){
			disk_hot[i][j] = read_demand[j][label1] + read_demand[j][label2];
		}
	}
	for(int i = 1 ;i <= N ;i++){
		for(int j = 1 ; j <= N ;j++){
			for(int k = 1; k <= (T - 1) / FRE_PER_SLICING + 1; k++ ){
				hot_disk_company[i][j] += (disk_hot[i][k] / 1000)* (disk_hot[j][k] / 1000);
			}
			hot_disk_rank[i][j] = {hot_disk_company[i][j] , j };
		}
		sort(hot_disk_rank[i]+1,hot_disk_rank[i]+N+1);
	}	
}

void disk_label_allocate(){//为每个磁盘寻找干扰最小的邻居，用于排除一些峰值可能重合的标签，让磁盘的负载均衡少受干扰
	for(int i = 1 ;i <= N ;i++){
		int label1=disk_label[i][0].first;
        int label2=disk_label[i][1].first; 
		for(int j = 1 ; j <= 48 ;j++){
			disk_hot[i][j] = read_demand[j][label1] + read_demand[j][label2];//磁盘热度分时计算
		}
	}
	for(int i = 1 ;i <= N ;i++){
		for(int j = 1 ; j <= M ;j++){
			for(int k = 1; k <= (T - 1) / FRE_PER_SLICING + 1; k++ ){
				hot_disk_label_company[i][j] += (disk_hot[i][k] / 1000)* (read_demand[k][j] / 1000);
			}
			hot_disk_label_rank[i][j] = {hot_disk_label_company[i][j] , j };
		}
		sort(hot_disk_label_rank[i]+1,hot_disk_label_rank[i]+M+1);
	}	
}

int max_exist(int label1, int label2) {
    int max_size = 0;
    for (int j = 1; j <= time_stamp_all; j++) {
        long int sum = 0;
        if (label_disk[label1].size() >= 2) {
            sum += (demand[label1][j] + 1) / 2;
        } else {
            sum += demand[label1][j];
        }
        if (label_disk[label2].size() >= 2) {
            sum += (demand[label2][j] + 1) / 2;
        } else {
            sum += demand[label2][j];
        }
        if (sum > max_size) {
            max_size = sum;
        }
    }
    return max_size;
}


void initialize()
{
    int begin=1;
    int diff=3;//存储错开位置
    //默认N=10来说，以下分配因为4+2+10=16，所以4*2=8 2 +10 刚好走完两轮磁盘，占用磁盘1~10的第一个和第二个小扇区
    sort(max_demand+1,max_demand+M+1,cmp_demand);//按照存储需求最大进行排名
    for(int disk_id=1;disk_id<=N/2-1;disk_id++) // 把前4个label的需求按照 1→disk1 disk2 这样的方式分配给磁盘,共占用1~8的第一个小区间
    {
        int label_tmp=max_demand[disk_id].second;
        int size_tmp=max_demand[disk_id].first/2+1;
        for(int x=1;x<=size_tmp;x++){
            free_blocks[disk_id*2-1][1].insert(x);
            free_blocks[disk_id*2][1].insert(x);
        }
        disk_label[disk_id*2-1].push_back({label_tmp , {1 , size_tmp} });
        disk_label[disk_id*2].push_back({label_tmp , {1 , size_tmp} });
        label_disk[label_tmp].push_back(disk_id*2-1);
        label_disk[label_tmp].push_back(disk_id*2);
        disk_begin[disk_id*2-1]=size_tmp+1;
        disk_begin[disk_id*2]=size_tmp+1;
        allocation_st[label_tmp] = true;
    }
    for(int disk_id=9;disk_id<=10;disk_id++) // 把排名5~6label的需求按照5->9 6->10这样的方式分配给磁盘
    {
        int label_tmp=max_demand[disk_id-4].second;
        int size_tmp=max_demand[disk_id-4].first+1;
        for(int x=1;x<=size_tmp;x++){
            free_blocks[disk_id][1].insert(x);
        }
        disk_label[disk_id].push_back({label_tmp , {1 , size_tmp} });
        label_disk[label_tmp].push_back(disk_id);
        disk_begin[disk_id]=size_tmp+1;
        allocation_st[label_tmp] = true;
    }
    
    
    for(int disk_id = 1 ; disk_id <= N ; disk_id++){
    	int now_have_label = disk_label[disk_id][0].first;
    	for(int j = 1 ; j <= M ;j++){
    		int want_allocation_label = hot_rank[now_have_label][j].second;//找到与该label热度匹配最合适的label且还没被预分配位置的
    		if(allocation_st[want_allocation_label] == true) continue;
    		int size_tmp=max_demand_beifen[want_allocation_label]  + 1;
    		for(int x=disk_begin[disk_id];x<=disk_begin[disk_id]+size_tmp;x++)
            free_blocks[disk_id][2].insert(x);
            disk_label[disk_id].push_back({want_allocation_label , {disk_begin[disk_id] , disk_begin[disk_id] + size_tmp}});
            disk_begin[disk_id]+=size_tmp+1;
            label_disk[want_allocation_label].push_back(disk_id);
            allocation_st[want_allocation_label] = true;
            break;
		}
	}

    
    calculate_two_demand();
	disk_label_allocate();
    for(int disk_id=1;disk_id<=N;disk_id++)
    {
		//int label1_tmp=disk_label[(disk_id-diff*2-1+N)%N+1][0].first;
        //int label2_tmp=disk_label[(disk_id-diff*2-1+N)%N+1][1].first;
        //int size_tmp=max_demand_two[(disk_id-diff*2-1+N)%N+1] * 1.00;
       int label1_tmp = 0;
        int label2_tmp = 0;
        int label1 = disk_label[disk_id][0].first;
        int label2 = disk_label[disk_id][1].first;
        //找到两个与当前diskid存储的两个label最没有热度冲突的label_tmp，allocation_disk_label_st[now_label] >=label_disk[now_label].size()是为了控制不要过多分配某个过热标签
        for(int i = 1 ; i <= M ;i++){
        	int now_label = hot_disk_label_rank[disk_id][i].second;
        	if(now_label == label1 || now_label == label2 || allocation_disk_label_st[now_label] >=label_disk[now_label].size() ) continue;
        	label1_tmp = now_label;
        	allocation_disk_label_st[now_label]++;
        	break;
		}
		
		for(int i = 1 ; i <= M ;i++){
        	int now_label = hot_disk_label_rank[disk_id][i].second;
        	if(now_label == label1 || now_label == label2 || allocation_disk_label_st[now_label] >= label_disk[now_label].size() || now_label == label1_tmp) continue;
        	label2_tmp = now_label;
        	allocation_disk_label_st[now_label]++;
        	break;
		}
		//计算两个标签的最大存储需求
        int size_tmp = max_exist(label1_tmp ,label2_tmp ) * 0.92;
        //把扇区三分配好，扇区三的大小是size_tmp大小
        for(int x=disk_begin[disk_id];x<=disk_begin[disk_id]+size_tmp;x++)
            free_blocks[disk_id][3].insert(x);
        disk_label[disk_id].push_back({label1_tmp , {disk_begin[disk_id] , disk_begin[disk_id]+size_tmp} });
        disk_label[disk_id].push_back({label2_tmp , {disk_begin[disk_id] , disk_begin[disk_id]+size_tmp} });
        label_disk_2[label1_tmp].push_back(disk_id);
        label_disk_2[label2_tmp].push_back(disk_id);
        disk_begin[disk_id]+=size_tmp+1;
    }
    //未分配区域，放进free_space中，
    for(int disk_id=1;disk_id<=N;disk_id++)
    {
        for(int x=disk_begin[disk_id];x<=V;x++)
            free_space[disk_id].insert(x);
    }
    double ratio = 3.4;
   for(int i = 1 ;i <= (T - 1) / FRE_PER_SLICING + 2 ; i++){
      for(int j = 1 ; j <= N ; j++){
      	if(j != 888){
            int label1 = disk_label[j][0].first;
      		int label2 = disk_label[j][1].first;
            int label3 = disk_label[j][2].first;
            int label4 = disk_label[j][3].first;
            int size1 = max_demand_beifen[label1];
      		int size2 = max_demand_beifen[label2];
            int size3 = max_demand_beifen[label3];
      		int size4 = max_demand_beifen[label4];
      		double xjb1 = static_cast<double>(read_demand[i][label1]) / size1;
            double xjb2 = static_cast<double>(read_demand[i][label2]) / size2;
            double xjb3 = static_cast<double>(read_demand[i][label3] + read_demand[i][label4]) / (size3 + size4);
            if(xjb3 > (xjb1 + xjb2 ) * 3.3 )
            {
                disk_point_range[i][j] = {{disk_label[j][3].second.first,disk_label[j][3].second.second},3};
            }//只覆盖第三个扇区
      		else if(xjb1 > xjb2 * ratio )   disk_point_range[i][j] = {{disk_label[j][0].second.first,disk_label[j][0].second.second},1};
      		else if(xjb1 * ratio < xjb2 ) disk_point_range[i][j] = {{disk_label[j][1].second.first,disk_label[j][1].second.second},2};
      		else disk_point_range[i][j] = {{disk_label[j][0].second.first,disk_label[j][1].second.second},2};//覆盖扇区1和扇区2
		  }
	  }
   }
   
   for (int label_id = 1; label_id <= M; label_id++) {  
    int cnt = 1;
    for (int hot_rank_idx = M; hot_rank_idx >= 1; hot_rank_idx--) {  //倒着找，找与label_id热度最相近的
        if (cnt > cnt_max) break;
        int label_replica_id = hot_rank[label_id][hot_rank_idx].second;
        if (label_replica_id == label_id) continue;  // 跳过自身标签

        for (int j = 0; j < label_disk[label_replica_id].size(); j++) {
            int disk_id = label_disk[label_replica_id][j]; 
            bool is_label_in_disk = false;

            for (int k = 0; k < disk_label[disk_id].size(); k++) {
                if (disk_label[disk_id][k].first == label_id) {
                    is_label_in_disk = true;
                    break;
                }
            }

            if (!is_label_in_disk) {//label_id不在这个disk里有分配
                int block_id = -1;
                for (int k = 0; k < disk_label[disk_id].size(); k++) {
                    if (disk_label[disk_id][k].first == label_replica_id) {
                        block_id = k ;  
                        break;
                    }
                }
                if (block_id != -1) {//label_replica_id在这个disk里有分配，则说明当前disk可以存放label_id的内容，作为一个备选项
                    label_replica_disk[label_id][cnt] = {disk_id, block_id};
                    cnt++;
                    if (cnt > cnt_max) break;  
                }
            }
        }
    }
}
   

}
bool cmp_hot(pair<int,int> a, pair<int,int> b){
	return a.first > b.first;
}

// 计算hot_hank[i]的最大匹配热度j,即label i与label j最不容易同时读取，因为负载均衡的逻辑是，每个磁盘不要在一个时间内同时读取两个高需求的
void hot_allocate(){
	for(int i = 1 ;i <= M ;i++){
		for(int j = 1 ; j <= M ;j++){
			for(int k = 1; k <= (T - 1) / FRE_PER_SLICING + 1; k++ ){
				hot_company[i][j] += (read_demand[k][i] / 100)* (read_demand[k][j] / 100);
			}
			hot_rank[i][j] = {hot_company[i][j] , j };
		}
		sort(hot_rank[i]+1,hot_rank[i]+M+1);
	}
}


int main()
{
    scanf("%d%d%d%d%d", &T, &M, &N, &V, &G);
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
    hot_allocate();
    calculate_demand();
    initialize();
    printf("OK\n");
    fflush(stdout);

    std::ofstream outFile;
    /*outFile.open("./example_log.txt", std::ios::out | std::ios::app); // 以追加模式打开
    outFile << "disk_label" << endl;*/
    for(int i=1;i<=N;i++){
        outFile << i << ": ";
        for(int j=0;j<disk_label[i].size();j++)
            outFile << disk_label[i][j].first << " " << disk_label[i][j].second.first << " " << disk_label[i][j].second.second << "       ";
        outFile << endl;

    }
    outFile << "label_replica_disk" << endl;
    for(int i=1;i<=M;i++)
    {
        outFile << "Label " << i << ":";
        for(int j=0;j<5;j++)
            outFile << label_replica_disk[i][j].first <<" "  << label_replica_disk[i][j].second << "       " ;
        outFile << endl;
    }
    outFile << "read_demand" << endl;
    for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++){
        outFile << "PERIOD" << j << endl;
        for(int i=1;i<=M;i++)
            outFile << read_demand[j][i] << " ";
        outFile << endl;
    }
    outFile << "LABEL TOP TIME" << endl;
    for(int i=1;i<=M;i++)
    {
        outFile << label_top_time[i] << " " ;
    }
    outFile << endl;
    outFile << "disk_range" << endl;
    for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++)
    {
        outFile << "PERIOD" << j << endl;
        for(int i=1;i<=N;i++){
            outFile << "disk" << i <<": ";
            // outFile << disk_point_range[j][i].first << " " ;
            outFile << disk_point_range[j][i].second;
            outFile << endl;
        }
        outFile << endl;
    } 
    
    outFile.close();

    for (int i = 1; i <= N; i++) {
        disk_point[i] = 1;
    }

    for (int t = 1; t <= T + EXTRA_TIME; t++) {
        if(t == 0) {
			printf("%d  OK\n",disk_label[10][3].first );
		}
        time_stemp=t/FRE_PER_SLICING+1;
        timestamp_action();
        delete_action();
        write_action();
        read_action();
    }
    clean();

    outFile.open("./example_log.txt", std::ios::out | std::ios::app); // 以追加模式打开
    outFile << "cnt_in:" << cnt_in << endl;
    outFile.close();
    
    return 0;
}