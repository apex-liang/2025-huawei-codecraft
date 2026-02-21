#include"solutions.h"
void timestamp_action()
{
    int timestamp;
    scanf("%*s%d", &timestamp);
    printf("TIMESTAMP %d\n", timestamp);

    fflush(stdout);
}

void calculate_demand()
{
    for(int time_stamp=1;time_stamp < time_stamp_all ;time_stamp++)
    {
        for(int i=1;i<=M;i++)
        {
        	int maxnum = 0;
        	int toptime = 0;
            delete_demand[0][i]=0;
            demand[0][i]=0;
            for(int j=1;j<=time_stamp;j++){
			demand[i][j]=(long int)(write_demand[j][i] + demand[i][j-1]) - delete_demand[j-1][i];
			if(demand[i][j] > maxnum ) {
			    maxnum = demand[i][j];
			    toptime = j;
			  }
              
			}
			max_demand[i] =  {maxnum , i};
			max_demand_beifen[i]=max_demand[i].first;
			top_time[i] = toptime;    
        }
    }
    for(int time_stamp=1;time_stamp < time_stamp_all ;time_stamp++)
    {
        for(int i=1;i<=M;i++)
            demand_all_label[time_stamp]+=demand[i][time_stamp];
    }
    max_demand_all_label=*max_element(demand_all_label+1,demand_all_label+48);//所有时间中的所有Label最大存量需求
}



bool cmp_demand(pair<long int,long int> label1,pair<long int,long int> label2)
{
    return label1.first>label2.first;
}

void initialize()
{
    int k=20;//预留空间
    int free_to_divide=V-max_demand_all_label*2/N-k;//有两个备份不管，只管备份一，只分配备份一的大小
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
}
int disk_label_new[11][3];
bool disk_label_new_st[17];
vector<int> label_disk_new[17];

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
void update()
{
    label_need_read.clear();
    for(int i=1;i<=M;i++)
    {
        if(read_demand[time_stemp][i]>=read_sum[time_stemp]/28)//调参点，卡需要读取的label
            label_need_read.insert(i);
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
    }
}
int main()
{
    scanf("%d%d%d%d%d", &T, &M, &N, &V, &G);
    block_size=V/disk_block_num;
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

    std::ofstream outFile;
    outFile.open("./example_log.txt", std::ios::out); // 以追加模式打开
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
    outFile << "demand" << endl;
    for(int time_stamp=1;time_stamp < time_stamp_all ;time_stamp++)
    {
        outFile << "timestemp " <<time_stamp  << ": ";
        for(int i=1;i<=M;i++)
        {
            outFile << demand[i][time_stamp] << " ";
        }
        outFile << endl;
    }
    outFile << "max_demand_all" << max_demand_all_label << endl;
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
        time_stemp=t/FRE_PER_SLICING+1;
        if(t%FRE_PER_SLICING==1) update();
        timestamp_action();
        delete_action();
        write_action();
        read_action();
        if(t==80000) test(); 
    }
    clean();
    
    return 0;
}