#include"solutions.h"
void timestamp_action()
{
    int timestamp;
    scanf("%*s%d", &timestamp);
    printf("TIMESTAMP %d\n", timestamp);

    fflush(stdout);
}
void update_mid()
{
    for(int i=1;i<=N;i++)
    {
        if(req_p1[i]*10>12*req_p2[i]&&disk_point_mid[i]>block_size*(req_p1[i]-req_p2[i])/(req_p2[i]+req_p1[i]))  
        {
            // int disk_point_pos=((disk_point_mid[i]-1)/block_size+1)*block_size;
            // bool flag=0;
            // while(find(label_need_read.begin(),label_need_read.end(),disk_block[i][disk_point_pos/block_size])==label_need_read.end()) 
            // {
            //     disk_point_pos=disk_point_pos-block_size;
            //     flag=1;
            // }
            // if(flag&&disk_point_pos>block_size)
            //     disk_point_mid[i]=disk_point_pos-block_size*(req_p1[i]-req_p2[i])/(req_p2[i]+req_p1[i]);
            // else
            //     disk_point_mid[i]-=block_size*(req_p1[i]-req_p2[i])/(req_p2[i]+req_p1[i]);
            disk_point_mid[i]-=block_size*(req_p1[i]-req_p2[i])/(req_p2[i]+req_p1[i]);
        }
        else if(req_p2[i]*10>req_p1[i]*12&&disk_point_mid[i]<disk_begin[i]-block_size*free_space_block[i].size()-block_size*(req_p2[i]-req_p1[i])/(req_p2[i]+req_p1[i]))  
        {
            // int disk_point_pos=((disk_point_mid[i]-1)/block_size)*block_size+1;
            // bool flag=0;
            // while(find(label_need_read.begin(),label_need_read.end(),disk_block[i][disk_point_pos/block_size+1])==label_need_read.end()) 
            // {
            //     disk_point_pos=disk_point_pos+block_size;
            //     flag=1;
            // }
            // if(flag&&disk_point_pos<disk_begin[i]-block_size*free_space_block[i].size()-block_size)
            //     disk_point_mid[i]=disk_point_pos+block_size*(req_p2[i]-req_p1[i])/(req_p2[i]+req_p1[i]);
            // else
            //     disk_point_mid[i]+=block_size*(req_p2[i]-req_p1[i])/(req_p2[i]+req_p1[i]);
        
           disk_point_mid[i]+=block_size*(req_p2[i]-req_p1[i])/(req_p2[i]+req_p1[i]);
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
            for(int j=1;j<=time_stamp_all;j++){
			demand[i][j]=(long int)(write_demand[j][i] + demand[i][j-1]) - delete_demand[j-1][i];
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



bool cmp_demand(pair<long int,long int> label1,pair<long int,long int> label2)
{
    return label1.first>label2.first;
}

void preallocation(){  // 预分配每个label的片片
	int st_preallocation[17];
    for(int i = 0 ; i <= M ;i++) st_preallocation[i] = 0;
    st_preallocation[1] = 1;
    int last_label = 1;
    preallocation_label[1] = 1;
    for(int i = 2 ; i <= M ;i++){
    	for(int j = M ; j >= 1 ; j--){
    		int want_allocation_label = xjb_rank[last_label][j].second;//用到了xjb排名，
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
    //for(int i = 1 ;i <= M ; i++){
    	int want_allocation_label = preallocation_label[i];
    	int demand_preallocation = demand[want_allocation_label][preallocation_time];
    	//int demand_preallocation = demand[i][preallocation_time];
    	int cnt = demand_preallocation / block_size ; 
    	
    	while(cnt){
            free_space_block[now_disk_id].erase(find(free_space_block[now_disk_id].begin(),free_space_block[now_disk_id].end(),now_block));
    		label_block[want_allocation_label].insert({now_disk_id , now_block}) ;
    		//label_block[i].insert({now_disk_id , now_block}) ;
            disk_block[now_disk_id][now_block]=want_allocation_label;
            //disk_block[now_disk_id][now_block]=i;
    		cnt--;
    		now_disk_id++;
    		if(now_disk_id > 10) {
    			now_block++;
    			now_disk_id = 1;
			}
		}
		
	}
}
void test_update()
{
    std::ofstream outFile;
    outFile.open("./example_log_11.txt", std::ios::out|std::ios::app); // 以追加模式打开
    outFile << "TIME_STEMP" << time_stemp << ": ";
    outFile << "read_need:" << endl;
    for(set<int>::iterator it=label_need_read.begin();it!=label_need_read.end();it++)
        outFile << *it << " ";
    outFile << endl;
    outFile << "xjb:" << endl;
    for(int i=1;i<=M;i++)
    {
        outFile << xjb[i] << " ";
    }
    outFile << xjb_avr << endl;
    outFile << "read_demand_all_label" << read_demand_all_label << endl;
    outFile.close();
    
}
void test_req()
{
    std::ofstream outFile;
    outFile.open("./exmple_log_10.txt", std::ios::out|std::ios::app); // 以追加模式打开
    for(int i=1;i<=N;i++)
        outFile << req_p1[i] << " "<< req_p2[i] << endl;
    outFile.close();
}
int confference[48*20+100]={440,675,179,498,96,264,76,209,25,132,12,75,8,16,0,10,0,2,0,0,212,722,211,119,173,114,219,732,191,764,107,813,14,688,74,355,1,0,0,0,3437,2843,0,0,0,2,2680,2770,2876,3329,3489,2623,2881,3039,3049,456,0,0,0,0,6452,5352,0,0,0,0,4986,5146,5390,5950,6473,4920,5396,5572,5605,1363,0,0,0,0,3027,16770,2963,2075,2933,1804,3109,14477,3096,16358,3294,15661,41,14021,0,10367,0,0,0,0,14338,21533,9725,12238,9779,9992,14500,19598,14176,21076,11263,19685,4974,14114,0,15865,0,1453,0,0,14070,25005,14736,12951,13660,10796,14412,23596,14093,24839,11192,17850,9193,13861,22,19928,0,5232,0,0,4463,22138,4589,11013,4324,8909,4503,20675,4459,22129,3563,13261,3000,12811,1041,18321,0,7451,0,134,2174,19008,1447,7695,1516,7104,1435,15028,1482,16728,1546,14750,1454,14373,1175,15625,817,9860,0,902,3890,25418,3546,13898,3669,13162,3720,15838,3614,17118,3823,20571,3712,21040,3059,21441,1310,17891,0,4315,6751,31886,6619,21500,6728,20334,6705,18428,6735,19500,7015,24399,6636,26017,8197,27472,3262,23555,479,12018,13499,44768,13160,34746,13995,32169,13591,27079,13256,29110,14272,30830,13210,31317,13796,33090,11804,29483,4253,22029,17912,60834,17800,50118,18558,47368,18432,37737,17736,41107,20606,41767,19291,42349,19998,37926,18505,34113,15360,32607,29104,77299,26222,64728,26262,64170,26826,49908,26520,51406,29315,57779,28215,56499,25579,52127,27940,46407,27943,49991,31933,72676,27706,61472,30858,58398,28414,47245,28321,46330,31097,55371,32955,48198,26644,52111,31327,45927,30304,54078,29350,51935,21893,43842,25444,44318,22535,42158,22570,41912,25816,36168,24487,33995,24265,43390,25860,38868,24398,45727,21785,36723,18621,31432,19206,32355,19814,33672,18423,35169,15475,22680,14659,23859,14272,35068,19020,27217,17033,32091,22552,31054,21726,25939,20539,27615,20185,30932,18540,31203,16830,18849,18863,18308,16011,27499,17649,23991,18321,20849,15591,23248,16437,19920,17049,21785,17087,26081,15642,23179,15148,14354,16518,16030,16047,16378,16662,16631,14347,15095,14597,32254,12012,28499,13865,29747,14155,32628,13579,29315,11591,27200,13161,28736,11584,24691,12261,22141,11934,23596,14691,26322,12872,20745,14241,22992,13407,23496,13052,20937,13204,22544,13398,25642,12949,19719,12562,16223,12519,18693,12660,21198,12642,17230,13262,20658,13392,20313,12157,16607,12446,18620,12041,23618,11701,18175,13382,15229,11620,15033,12737,24057,11811,22117,13474,24880,12737,25892,11542,20295,12595,22771,11507,28390,11724,23387,12781,21478,10760,16897,15039,25765,14190,22209,16202,24697,15325,30816,14844,22717,15889,25840,14609,31851,14751,30321,15687,23084,13758,17001,17024,22194,15910,19316,16694,22713,16491,34719,16062,23099,16761,29258,15602,29103,15085,28491,14695,22299,16195,15735,17862,20215,16947,19803,17240,22632,16956,43316,16509,25351,17582,37299,16380,26526,16449,23184,15154,24886,16371,19133,13223,22882,10634,27811,12816,26981,15766,54603,12435,31492,14866,46018,11865,28057,11664,25934,12178,28955,15030,25511,15694,17409,11487,27163,11341,28170,24386,48494,14534,29730,33646,27504,9809,25807,12803,23766,21259,21256,22298,25839,14789,24544,12140,31602,12871,35538,18611,58577,12574,34378,13149,50584,10862,26410,15360,27361,19316,31150,21917,38882,16738,32110,13675,38578,15867,44069,21114,62689,15057,36874,16566,49516,13632,28292,14830,35679,19201,40596,19900,49563,23996,30014,18547,37038,23138,38342,30901,50110,22196,30803,17546,39283,16956,24032,23397,30368,20896,37637,27943,44546,30107,46614,24448,47673,21548,48547,28213,56514,20136,43122,20191,41632,22844,30047,26131,42599,21979,48374,30337,52987,37769,35308,31638,29235,30720,30825,30898,41200,27680,28525,22745,29094,23594,30889,23935,39221,27063,42180,27568,45572,42220,32471,32552,24870,32996,24925,32291,32898,28103,24071,23981,23370,25968,31581,28474,32089,28705,37547,29408,37410,28962,56978,18581,41169,28775,36984,21365,45855,18977,36823,18935,33953,24065,39792,19805,41152,24024,43589,23453,39061,28070,58506,18514,38367,26171,37959,19003,39998,18217,36467,17755,31694,24984,40877,17511,38601,26446,33981,26506,31736,29420,59377,20250,36973,25905,40261,19356,38000,18883,36288,18945,31384,25362,40393,18312,36166,26135,32981,26279,31463,45388,39917,19287,37431,32895,34818,20833,33510,25734,25058,23337,28453,25149,35644,25653,26781,31426,29324,31350,25519,39606,39790,17789,35162,27149,37788,17030,33086,21920,23740,19341,30474,20892,31460,22151,25931,26881,33478,25375,28932,28417,35169,14442,29419,19144,36765,15292,25089,15096,20445,15364,27829,14906,24833,15185,21840,17655,34586,16961,27326,21199,28814,15140,20031,13471,33553,12299,20182,11731,15434,14062,22602,9877,19908,12444,18154,20098,24329,17414,17962,18010,21509,9597,18915,19523,21775,11459,17152,7213,13984,14583,16874,8606,13280,7728,18873,20973,20913,14657,17775,19272,12736,4936,18531,14940,20669,7000,16365,6790,9267,12287,15007,6346,10681,9574,13717,18276,19740,12141,16693,14588,10460,2827,16348,12008,19284,4832,15026,4638,8098,9793,13423,4199,9554,7395,12600,15719,17886,9802,15433,14160,2016,8720,4961,18377,5455,10024,4212,5956,2147,10658,6518,7969,1465,11583,3157,18348,7509,13131,5966,10161,1548,6617,3162,13334,4141,6877,2686,4049,1679,7593,5013,5924,1023,8380,2091,13068,4986,8804,3859,6636,9814,4886,6929,8001,10968,5864,7228,4274,5249,6130,8246,6832,5546,5329,6357,5728,9542,5894,6157,8154,8378,4151,6730,5361,10207,5127,6471,3312,6152,4409,7203,5398,6665,3677,6541,4460,6808,4671,4821};
void update()
{
    
    read_demand_all_label=0;
    for(int i=1;i<=M;i++)
    {
        xjb[i]=100*read_demand[time_stemp][i]/demand[i][time_stemp];
        read_demand_all_label+=read_demand[time_stemp][i];
    }
    xjb_avr=72*read_demand_all_label/demand_all_label[time_stemp];
    
    label_need_read.clear();
    for(int i=1;i<=M;i++)
    {
        if(xjb[i]>=xjb_avr)
            label_need_read.insert(i);
    }
    test_update();
    for(int i=1;i<=N;i++)
    {
        req_p1[i]=0;
        req_p2[i]=0;
        is_left[i]=0;
        is_right[i]=0;
    }
    // label_need_read.clear();
    // for(int i=1;i<=M;i++)
    // {
    // 	if(time_stemp <= 2 ){
    // 		if(read_demand[time_stemp][i]>=read_sum[time_stemp]/20){
    // 			label_need_read.insert(i);
	// 		}
            
	// 	}
	// 	else if(time_stemp == 47){
	// 		if(read_demand[time_stemp][i]>=read_sum[time_stemp]/22)
    //         label_need_read.insert(i);
	// 	}
	// 	else if(time_stemp == 48){
	// 		if(read_demand[time_stemp][i]>=read_sum[time_stemp]/20)
    //         label_need_read.insert(i);
	// 	}
	// 	else if(time_stemp >= 13 || time_stemp <= 38 ){
	// 		if(read_demand[time_stemp][i]>=read_sum[time_stemp]/18)
    //         label_need_read.insert(i);
	// 	}
    //     else if(read_demand[time_stemp][i]>=read_sum[time_stemp]/20)
    //         label_need_read.insert(i);
    // }
    
    /*for(int disk_id = 1 ; disk_id <= 10 ; disk_id++){
    	for(int block_id = 1 ; block_id <= disk_block_num ; block_id++){
    		if( find(label_need_read.begin(),label_need_read.end(),disk_block[disk_id][block_id])!=label_need_read.end()){
    			block_information[disk_id][time_stemp].all++;
			}
		}
	}
	 int free_to_divide=V-max_demand_all_label*2/N-k_new;
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
			//while( disk_block[disk_id][block_information[disk_id][time_stemp].middle] == disk_block[disk_id][block_information[disk_id][time_stemp].middle+1]) block_information[disk_id][time_stemp].middle++;
		}
		if(block_information[disk_id][time_stemp].all == 0 || block_information[disk_id][time_stemp].all == 1)disk_point_mid[disk_id] = max( (block_information[disk_id][time_stemp].middle ) * block_size - block_size/2 , 1);
		else {
		   if(time_stemp <= 24 )disk_point_mid[disk_id] = (block_information[disk_id][time_stemp].middle ) * block_size - block_size/2;
		   else disk_point_mid[disk_id] = (block_information[disk_id][time_stemp].middle ) * block_size;
		}
	}*/
	if(time_stemp<=100){//计算所有需要读取的label在当前disk中有多少个块
	for(int disk_id = 1 ; disk_id <= 10 ; disk_id++){
    	for(int block_id = 1 ; block_id <= disk_block_num ; block_id++){
    		if( find(label_need_read.begin(),label_need_read.end(),disk_block[disk_id][block_id])!=label_need_read.end()){
    			block_information[disk_id][time_stemp].all ++ ;
			}
		}
	}
	 int free_to_divide=V-max_demand_all_label*2/N-k_new;
    block_size=free_to_divide/disk_block_num;
	//预设起始、末尾、中间的块
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
    // int left=0;
    // int right=0;
    // for(int i=1;i<=10;i++)
    // {
    //     left = confference[time_stemp-1]
    //     sscanf(line_neirong.c_str(), "%d %d", &left, &right);   // 创建一个字符串流，用于解析每一行

    //     if(left>right&&(left-right)>3000)
    //     {
    //         disk_point_mid[i]-=block_size;
    //     }
    //     else if(left<right&&(right-left)>3000)
    //     {
    //         disk_point_mid[i]+=block_size;
    //     }
    // }

    // ifstream ifs("../../confference.txt");
    // int line=1;
    // int left=0;
    // int right=0;
    // string line_neirong;
    // if (!ifs.is_open()) {
    //     std::cerr << "无法打开文件！" << std::endl;
    // }
    // while(line<(time_stemp-1)*10+1)
    // {
    //     getline(ifs,line_neirong);
    //     line++;
    // }
    // for(int i=1;i<=10;i++)
    // {
    //     getline(ifs,line_neirong);
    //     sscanf(line_neirong.c_str(), "%d %d", &left, &right);   // 创建一个字符串流，用于解析每一行

    //     if(left>right&&(left-right)>3000)
    //     {
    //         disk_point_mid[i]-=block_size;
    //     }
    //     else if(left<right&&(right-left)>3000)
    //     {
    //         disk_point_mid[i]+=block_size;
    //     }
    // }
    // ifs.close();
    }
    else
    {
        for(int disk_id = 1 ; disk_id <= 10 ; disk_id++){
    	for(int block_id = 1 ; block_id <= disk_block_num ; block_id++){
    		if( find(label_need_read.begin(),label_need_read.end(),disk_block[disk_id][block_id])!=label_need_read.end()){
    			block_information[disk_id][time_stemp].all += read_demand[time_stemp][disk_block[disk_id][block_id]]/((block_size-disk_label_able[disk_id][block_id].size()/demand[disk_block[disk_id][block_id]][time_stemp])) ;
			}
		}
	}
        int free_to_divide=V-max_demand_all_label*2/N-k_new;
    block_size=free_to_divide/disk_block_num;
	for(int disk_id = 1 ; disk_id <= 10 ; disk_id++){
		int cnt = 0;
        bool flag=false;
    	for(int block_id = 1 ; block_id <= disk_block_num ; block_id++){
    		if( find(label_need_read.begin(),label_need_read.end(),disk_block[disk_id][block_id])!=label_need_read.end()){
    			cnt+=read_demand[time_stemp][disk_block[disk_id][block_id]]/((block_size-disk_label_able[disk_id][block_id].size()/demand[disk_block[disk_id][block_id]][time_stemp])) ;
    			if(cnt > (block_information[disk_id][time_stemp].all  / 2 + 1)&&!flag) {block_information[disk_id][time_stemp].middle = block_id; flag=true;}
    			if(cnt == block_information[disk_id][time_stemp].all ) block_information[disk_id][time_stemp].end = block_id;
			}
		}
		if(block_information[disk_id][time_stemp].all == 0 || block_information[disk_id][time_stemp].middle == block_information[disk_id][time_stemp].end)disk_point_mid[disk_id] = max( (block_information[disk_id][time_stemp].middle ) * block_size - block_size/2 , 1);
		else {
		   if(time_stemp <= 24 )disk_point_mid[disk_id] = (block_information[disk_id][time_stemp].middle ) * block_size;
		   else disk_point_mid[disk_id] = (block_information[disk_id][time_stemp].middle ) * block_size;
		}
	}
    }
	
    
    if(time_stemp<10)
        bias_read=0;
    else
        bias_read=0;
        
        
}

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

void xjb_allocate(){
	int xjb[49][MAX_LABEL_NUM];
   // int read_demand_all_label[49];
    for(int time = 1 ; time <= 48 ;time++){
	    for(int i=1;i<=M;i++)
	    {
	        xjb[time][i]=read_demand[time][i]* 100/demand[i][time] ;
	        //read_demand_all_label[time] += read_demand[time_stemp][i];
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

void initialize()
{
	hot_allocate();
	xjb_allocate();
    int free_to_divide=V-max_demand_all_label*2/N-k_new;
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
int disk_label_new[11][3];
bool disk_label_new_st[17];
vector<int> label_disk_new[17];

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
void test_map(int t)
{
    std::ofstream outFile;
    outFile.open("./example_log.txt", std::ios::out|std::ios::app); // 以追加模式打开
    outFile << "TIME_STEMP" << time_stemp ;
    outFile << " time" << t << ": ";
    outFile << endl;
    for(int i=1;i<=N;i++){
        for(int j=1;j<=V-max_demand_all_label*2/N-k_new;j++)
        {    outFile << read_map[i][j] << " " ;}
        outFile << ";";
    }
    outFile << endl;
    outFile.close();
    
}

void test_pos()
{
    std::ofstream outFile;
    outFile.open("./example_log.txt", std::ios::out|std::ios::app); // 以追加模式打开
    outFile << "TIME_STEMP" << time_stemp << ": ";
    for(int i=1;i<=N;i++)
    outFile << "disk_point_1: " << 1 << " "<< (disk_begin[i]-block_size*free_space_block[i].size())/2 << " disk_point_2: " << (disk_begin[i]-block_size*free_space_block[i].size())/2 << " " << (disk_begin[i]-block_size*free_space_block[i].size()) << endl;
    outFile << endl;
    outFile.close();
    
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

    std::ofstream outFile;
    outFile.open("./example_log.txt", std::ios::out); // 以追加模式打开
    outFile.close();

    for (int i = 1; i <= N; i++) {
        disk_point[i] = 1;
        disk_point_2[i] = 1;
    }

    for (int t = 1; t <= T + EXTRA_TIME; t++) {
        time_stemp=t/FRE_PER_SLICING+1;
        if(t%FRE_PER_SLICING == 1&&t!=86401) update();
		if(t == 0 ) printf("%d ",xjb_rank[1][14].second) ;
        if(t%100==99) update_mid();
        timestamp_action();
        delete_action();
        write_action();
        read_action(t);
        if (t % FRE_PER_SLICING == 0) {
            gc_action();
            test_req();
        }
    }
    clean();
    
    return 0;
}