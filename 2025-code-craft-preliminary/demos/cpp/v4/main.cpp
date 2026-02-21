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
void initialize_partitions(int N, int V, int M) {
    for (int disk_id = 1; disk_id <= N; disk_id++) {
        int partition_size = V / TEST_BLOCK_DIVIDE;  // 平均分配给每个标签的空间
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
void calculate_demand()
{
    for(int time_stamp=1;time_stamp < time_stamp_all ;time_stamp++)
    {
        for(int i=1;i<=M;i++)
        {
            delete_demand[0][i]=0;
            demand[0][i]=0;
            for(int j=1;j<=time_stamp;j++)
                demand[i][j]=(long int)(write_demand[j][i] + demand[i][j-1]) - delete_demand[j-1][i];
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
void calculate_two_demand()
{
    // for(int time_stamp=1;time_stamp < time_stamp_all ;time_stamp++)
    // {
        for(int disk_id=1;disk_id<=N;disk_id++)
        {

            int label1=disk_label[disk_id][0].first;
            int label2=disk_label[disk_id][1].first;
            for(int j=1;j<=time_stamp_all;j++)
            {
                if(disk_id<=8)
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
int max_exist(int label1, int label2) {
    int max_size = 0;
    for (int j = 1; j <= time_stamp_all; j++) {
        long int sum = 0;
        if (label_disk_new[label1].size() >= 2) {
            sum += (demand[label1][j] + 1) / 2;
        } else {
            sum += demand[label1][j];
        }
        if (label_disk_new[label2].size() >= 2) {
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
    int disk_begin[MAX_DISK_NUM];
    sort(max_demand+1,max_demand+M+1,cmp_demand);
    
    for(int disk_id=1;disk_id<=N/2-1;disk_id++) // 把前5个label的需求按照 1→1 2 这样的方式分配给磁盘
    {
        int label_tmp=max_demand[disk_id].second;
        disk_label_new[disk_id*2-1][1] = label_tmp;
        disk_label_new[disk_id*2][1] = label_tmp;
        disk_label_new_st[label_tmp] = true;
        label_disk_new[label_tmp].push_back(disk_id*2-1);
        label_disk_new[label_tmp].push_back(disk_id*2);
    }
    for(int disk_id=9;disk_id<=10;disk_id++) // 把前5个label的需求按照 1→1 2 这样的方式分配给磁盘
    {
        int label_tmp=max_demand[disk_id-4].second;
        disk_label_new[disk_id][1] = label_tmp;
        disk_label_new_st[label_tmp] = true;
        label_disk_new[label_tmp].push_back(disk_id);
    }
    
    for(int disk_id = 1 ; disk_id <= N ; disk_id++){
    	int now_have_label = disk_label_new[disk_id][1];
    	for(int j = M ; j >= 1 ;j--){
    		int want_allocation_label = hot_rank[now_have_label][j].second;
    		if(disk_label_new_st[want_allocation_label] == true) continue;
            disk_label_new[disk_id][2] = want_allocation_label;
            label_disk_new[want_allocation_label].push_back(disk_id);
            disk_label_new_st[want_allocation_label] = true;
            break;
		}
	}

    ofstream outFile;
    outFile.open("./example_log.txt", std::ios::out | std::ios::app); // 以追加模式打开
    for(int i=1;i<=N;i++)
    {
        outFile << "disk_id:" << i << ":" ;
        for(int j=1;j<=2;j++)
            outFile << disk_label_new[i][j] << " " ;
        outFile << endl;
    }
    
    
    for(int disk_id=1;disk_id<=N;disk_id++)
    {
        int label_1=disk_label_new[disk_id][1];//1 16 2
        int label_2=disk_label_new[disk_id][2];
        int size_tmp=max_exist(label_1,label_2);
        // if((disk_id==5)||(disk_id==6)||(disk_id==9)||(disk_id==10))
        //     size_tmp=size_tmp*1;
        if(disk_id<=8)
            disk_label[disk_id].push_back({label_1,{1,max_demand_beifen[label_1]/2+1}});
        else
            disk_label[disk_id].push_back({label_1,{1,max_demand_beifen[label_1]+1}});
        disk_label[disk_id].push_back({label_2,{size_tmp-max_demand_beifen[label_2]-1,size_tmp}});
        label_disk[label_1].push_back(disk_id);
        label_disk[label_2].push_back(disk_id);
        for(int x=1;x<=size_tmp;x++){
            free_blocks[disk_id][1].insert(x);
        }
        disk_begin[disk_id]=size_tmp+1;
    }
    


    // for(int disk_id=2;disk_id<=N/2;disk_id++) // 把前5个label的需求按照 1→1 2 这样的方式分配给磁盘
    // {
    //     int label_tmp=disk_label[((disk_id-diff)*2-1+N-1)%N+1][0].first;
    //     int size_tmp=max_demand_beifen[label_tmp]/2+1;
    //     for(int x=disk_begin[disk_id*2-1];x<=disk_begin[disk_id*2-1]+size_tmp;x++){
    //         free_blocks[disk_id*2-1][3].insert(x);   
    //     }
    //     for(int x=disk_begin[disk_id*2];x<=disk_begin[disk_id*2]+size_tmp;x++){
    //         free_blocks[disk_id*2][3].insert(x);   
    //     }
        
    //     disk_label[disk_id*2-1].push_back({label_tmp , {disk_begin[disk_id*2-1] , disk_begin[disk_id*2-1]+size_tmp} });
    //     disk_label[disk_id*2].push_back({label_tmp , {disk_begin[disk_id*2] , disk_begin[disk_id*2]+size_tmp} });
    //     label_disk_2[label_tmp].push_back(disk_id*2-1);
    //     label_disk_2[label_tmp].push_back(disk_id*2);
    //     disk_begin[disk_id*2-1]+=size_tmp+1;
    //     disk_begin[disk_id*2]+=size_tmp+1;
    // }
    // // diff是1的时候是1,2，其余可以变
    // for(int disk_id=1;disk_id<=2;disk_id++) // 把前5个label的需求按照 1→1 2 这样的方式分配给磁盘
    // {
    //     int label_tmp=disk_label[disk_id+8][0].first;
    //     int size_tmp=max_demand_beifen[label_tmp]+1;
    //     for(int x=disk_begin[disk_id];x<=disk_begin[disk_id]+size_tmp;x++){
    //         free_blocks[disk_id][3].insert(x);
    //     }
    //     disk_label[disk_id].push_back({label_tmp , {disk_begin[disk_id] , disk_begin[disk_id]+size_tmp} });
    //     label_disk_2[label_tmp].push_back(disk_id);
    //     disk_begin[disk_id]+=size_tmp+1;
    // }




// 备份二整齐版
    // for(int disk_id=1;disk_id<=N;disk_id++)
    // {
    //     int label_tmp=disk_label[(disk_id-diff*2-1+N)%N+1][0].first;
    //     int size_tmp=disk_label[(disk_id-diff*2-1+N)%N+1][0].second.second-disk_label[(disk_id-diff*2-1+N)%N+1][0].second.first+1;
    //     for(int x=disk_begin[disk_id];x<=disk_begin[disk_id]+size_tmp;x++)
    //         free_blocks[disk_id][3].insert(x);
    //     disk_label[disk_id].push_back({label_tmp , {disk_begin[disk_id] , disk_begin[disk_id]+size_tmp} });
    //     label_disk_2[label_tmp].push_back(disk_id);
    //     disk_begin[disk_id]+=size_tmp+1;
    // }
    // for(int disk_id=1;disk_id<=N;disk_id++)
    // {
    //     int label_tmp=disk_label[(disk_id-diff*2-1+N)%N+1][1].first;
    //     int size_tmp=max_demand_beifen[label_tmp]+1;
    //     for(int x=disk_begin[disk_id];x<=disk_begin[disk_id]+size_tmp;x++)
    //         free_blocks[disk_id][4].insert(x);
    //     disk_label[disk_id].push_back({label_tmp , {disk_begin[disk_id] , disk_begin[disk_id]+size_tmp} });
    //     label_disk_2[label_tmp].push_back(disk_id);
    //     disk_begin[disk_id]+=size_tmp+1;
    // }
// 备份二乱序版
    calculate_two_demand();
    //备份二只分配需求的0.5作为预分配的位置
    for(int disk_id=1;disk_id<=N;disk_id++)
    {
        int label1_tmp=disk_label[(disk_id-diff*2-1+N)%N+1][0].first;
        int label2_tmp=disk_label[(disk_id-diff*2-1+N)%N+1][1].first;
        int size_tmp=max_demand_two[(disk_id-diff*2-1+N)%N+1]*0.5;
        for(int x=disk_begin[disk_id];x<=disk_begin[disk_id]+size_tmp;x++)
            free_blocks[disk_id][2].insert(x);
        disk_label[disk_id].push_back({label1_tmp , {disk_begin[disk_id] , disk_begin[disk_id]+size_tmp} });
        disk_label[disk_id].push_back({label2_tmp , {disk_begin[disk_id] , disk_begin[disk_id]+size_tmp} });
        label_disk_2[label1_tmp].push_back(disk_id);
        label_disk_2[label2_tmp].push_back(disk_id);
        disk_begin[disk_id]+=size_tmp+1;
    }

/*	int last_disk = 5;
	int label_tmp=max_demand[M].second;
    int size_tmp=max_demand[M].first;*/
    
    /*int label_tmp;
    for(int i = 1 ; i <= M ;i++) if(allocation_st[i] == false) label_tmp = i;
    int size_tmp=max_demand[label_tmp].first;
    
    for(int x=disk_begin[last_disk];x<=disk_begin[last_disk]+size_tmp;x++)
            free_blocks[last_disk][3].insert(x);
    disk_label[last_disk].push_back({label_tmp , {disk_begin[last_disk] , disk_begin[last_disk] + size_tmp}});
	disk_begin[last_disk]+=size_tmp+1;
    label_disk[label_tmp].push_back(last_disk);*/
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
            if(i<=8) xjb1=xjb1/2;
            double xjb2 = static_cast<double>(read_demand[i][label2]) / size2;
            double xjb3 = static_cast<double>(read_demand[i][label3] + read_demand[i][label4]) / (size3 + size4);
            if(xjb3 > (xjb1 + xjb2 ) * 4 )//除非备份2很有必要读，否则只读备份一区域，而且不存在只读某一个Label的备份一，因为Label A和Label B的备份一是重合存储的
            {
                disk_point_range[i][j] = {{disk_label[j][3].second.first,disk_label[j][3].second.second},3};
            }
      		// else if(xjb1 > xjb2 * ratio )   disk_point_range[i][j] = {{disk_label[j][0].second.first,disk_label[j][0].second.second},1};
      		// else if(xjb1 * ratio < xjb2 ) disk_point_range[i][j] = {{disk_label[j][1].second.first,disk_label[j][1].second.second},2};
      		// else disk_point_range[i][j] = {{disk_label[j][0].second.first,disk_label[j][1].second.second},2};
            else disk_point_range[i][j] = {{disk_label[j][0].second.first,disk_label[j][1].second.second},2};
		  }
      	//else disk_point_range[i][j] = {disk_label[j][0].second.first,disk_label[j][2].second.second};
	  }
   }


//     int ratio = 2.7;
//    for(int i = 1 ;i <= (T - 1) / FRE_PER_SLICING + 2 ; i++){
//       for(int j = 1 ; j <= N ; j++){
//       	if(j != 888){
//       		int label1 = disk_label[j][0].first;
//       		int label2 = disk_label[j][1].first;
//             int label3 = disk_label[j][2].first;
//             int label4 = disk_label[j][3].first;
//             if((read_demand[i][label3] + read_demand[i][label4] ) > (read_demand[i][label1] + read_demand[i][label2] ) * 8 )
//             {
//                 disk_point_range[i][j] = {{disk_label[j][3].second.first,disk_label[j][3].second.second},3};
//             }
//       		else if(read_demand[i][label1] > read_demand[i][label2] * 23/10){   
//                 disk_point_range[i][j] = {{disk_label[j][0].second.first,disk_label[j][0].second.second},1};
//             }
//       		else if(read_demand[i][label1] * 23/10< read_demand[i][label2] )
//             { 
//                 disk_point_range[i][j] = {{disk_label[j][1].second.first,disk_label[j][1].second.second},2};
//             }
//       		else
//             { 
//                 disk_point_range[i][j] = {{disk_label[j][0].second.first,disk_label[j][1].second.second},2};
//             }
// 		  }
//       	//else disk_point_range[i][j] = {disk_label[j][0].second.first,disk_label[j][2].second.second};
//       	else{
//       		// int label1 = disk_label[j][0].first;
//       		// int label2 = disk_label[j][1].first;
//       		// int label3 = disk_label[j][2].first;
//       		// if(read_demand[i][label1] > read_demand[i][label2] * ratio && read_demand[i][label1] > read_demand[i][label3] * ratio) 
// 			//   disk_point_range[i][j] = {disk_label[j][0].second.first,disk_label[j][0].second.second};
// 			// else if(read_demand[i][label2] > read_demand[i][label1] * ratio && read_demand[i][label2] > read_demand[i][label3] * ratio) 
// 			//   disk_point_range[i][j] = {disk_label[j][1].second.first,disk_label[j][1].second.second};
// 			// else if(read_demand[i][label3] > read_demand[i][label1] * ratio && read_demand[i][label3] > read_demand[i][label2] * ratio)
// 			//  disk_point_range[i][j] = {disk_label[j][2].second.first,disk_label[j][2].second.second};
// 			// else if(read_demand[i][label1] + read_demand[i][label2] >  read_demand[i][label3] * ratio) 
// 			//   disk_point_range[i][j] = {disk_label[j][0].second.first,disk_label[j][1].second.second};
// 			// else if(read_demand[i][label3] + read_demand[i][label2] >  read_demand[i][label1] * ratio) 
// 			//   disk_point_range[i][j] = {disk_label[j][1].second.first,disk_label[j][2].second.second};
// 			// else disk_point_range[i][j] = {disk_label[j][0].second.first,disk_label[j][2].second.second};
// 		  }
// 	  }
//    }
}

void hot_allocate(){
	for(int i = 1 ;i <= M ;i++){
		for(int j = 1 ; j <= M ;j++){
			for(int k = 1; k <= (T - 1) / FRE_PER_SLICING + 1; k++ ){
				hot_company[i][j] += (read_demand[k][i] / 100)* (read_demand[k][j] / 100);
			}
			hot_rank[i][j] = {hot_company[i][j] , j};
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
    outFile.open("./example_log.txt", std::ios::out | std::ios::app); // 以追加模式打开
    outFile << "disk_label" << endl;
    for(int i=1;i<=N;i++){
        outFile << i << ": ";
        for(int j=0;j<disk_label[i].size();j++)
            outFile << disk_label[i][j].first << " " << disk_label[i][j].second.first << " " << disk_label[i][j].second.second << "       ";
        outFile << endl;

    }
    outFile << "label_disk" << endl;
    for(int i=1;i<=M;i++){
        outFile << i << ": ";
        for(int j=0;j<label_disk[i].size();j++)
            outFile << label_disk[i][j] << " ";
        outFile << endl;

    }
    outFile << "label hot" << endl;
    vector<pair<int,pair<int,int>>> label_hot[17];
    for(int i=1;i<=M;i++)
    {
        for(int j=1;j<=M;j++)
        label_hot[i].push_back({max_exist(i,j),pair<int,int>{i,j}});
    }
    for(int i=1;i<=M;i++)
        sort(label_hot[i].begin(),label_hot[i].end());
    for(int i=1;i<=M;i++){
        outFile << "Label" << i << ": ";
        for(int j=0;j<label_hot[i].size();j++)
            outFile << label_hot[i][j].first << " (" << label_hot[i][j].second.first << "," << label_hot[i][j].second.second << ")    ";
        outFile << endl;

    }

    // outFile << "read_demand" << endl;
    // for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++){
    //     outFile << "PERIOD" << j << endl;
    //     for(int i=1;i<=M;i++)
    //         outFile << read_demand[j][i] << " ";
    //     outFile << endl;
    // }
    // outFile << "disk_range" << endl;
    // for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++)
    // {
    //     outFile << "PERIOD" << j << endl;
    //     for(int i=1;i<=N;i++){
    //         outFile << "disk" << i <<": ";
    //         // outFile << disk_point_range[j][i].first << " " ;
    //         outFile << disk_point_range[j][i].second;
    //         outFile << endl;
    //     }
    //     outFile << endl;
    // } 
    outFile.close();

    for (int i = 1; i <= N; i++) {
        disk_point[i] = 1;
    }

    for (int t = 1; t <= T + EXTRA_TIME; t++) {
        if(t == 0) {
			printf("%d  OK\n",disk_label[10][0].first);
		}
        time_stemp=t/FRE_PER_SLICING+1;
        timestamp_action();
        delete_action();
        write_action();
        read_action();
    }
    clean();
    
    return 0;
}