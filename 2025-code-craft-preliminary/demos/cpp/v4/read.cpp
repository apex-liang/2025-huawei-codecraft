#include"solutions.h"
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
        q[++tt]=request_id;
        request_all++;
    }
    if(tt-hh>1100) sort(q+tt-700,q+tt,cmp);
    else if(tt-hh>=0) sort(q+hh,q+tt,cmp);
    int n_resp = 0;
    
if (tt>=hh) {  // Check if the queue has elements
    bool done = false;  // Flag to exit loops if the queue becomes empty
    for (int i = 1; i <= N && !done; i++) {  // Exit outer loop if done
        for (int j = 1; j <= N && !done; j++) {  // Exit inner loop if done
            int current_id = q[hh];  // Get the front of the queue
            if (request[current_id].is_done) continue;  // Skip if request is done
            
            int object_id = request[current_id].object_id;
            for (int k = 1; k <= 1; k++) {
                if (object[object_id].replica[k] == j && work_object[j].first == 0) {
                    work_object[j] = {current_id, k};  // Assign work object
                    hh++; // Remove the processed request from the queue
                    
                    if (tt<hh) {  // If the queue is empty, exit all loops
                        done = true;  // Set flag to exit all loops
                        break;  // Exit the k-loop
                    }
                    break;  // Exit the k-loop
                }
            }
        }
    }
}
	for(int i=1;i<=N;i++){
		int object_id=request[work_object[i].first].object_id,replica=work_object[i].second;//
        // if((disk_point_range[time_stemp][i].second==2)&&(disk_point[i]>disk_point_range[time_stemp][i].first.second))
        // {
        //     int max_pos=max(disk_point_range[time_stemp][i].first.first,min_have_data_pos[i]);
        //     printf("j %d\n",max_pos);
		// 	disk_point[i]=max_pos;
		// 	pre_token_disk[i]=0;

        // }
        // // if((disk_point_range[time_stemp][i].second==1)&&((disk_point[i]> max_have_data_pos[i][1])||))
        // else if( ((disk_point[i]> max_have_data_pos[i][disk_point_range[time_stemp][i].second-1]))||( disk_point[i] > disk_point_range[time_stemp][i].first.second )){
		// 	printf("j %d\n",disk_point_range[time_stemp][i].first.first);
		// 	disk_point[i]=disk_point_range[time_stemp][i].first.first;
		// 	pre_token_disk[i]=0;
		// }
        if((disk_point[i]>disk_point_range[time_stemp][i].first.second))
        {
            int max_pos=max(disk_point_range[time_stemp][i].first.first,min_have_data_pos[i]);
            printf("j %d\n",1);
			disk_point[i]=1;
			pre_token_disk[i]=0;
        }
		else{
			int pre_token=pre_token_disk[i];
			int current_token=G;
			while(current_token>0){
				int current_unit=disk[i][disk_point[i]];
				if(current_unit||(time_stemp>=15)){
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
                    if(request[work_object[i].first].is_done==true){
                    	work_object[i].first=0;
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
            for(int j=1;j<=n_resp;j++) printf("%d\n",successful_request[j]);
    fflush(stdout);
}