/**************************************
Selective Jamming Demonstration Program
***************************************/
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <numeric>
#include <cstdlib>
#include <ctime>
#include <map>
#include <algorithm>
#include <time.h>
#include <math.h>
#include <unordered_set>


using namespace std;

#define DATA_PERIOD 1024               
#define RUNTIMES 10
#define ATT_NUM  140
#define PRR_T 90
#define FAIL_RATIO 88
#define BUFFER	4

#define MAXNODE 200
#define ACCESS_POINT1 121
#define ACCESS_POINT2 124

#define HOP 4
#define HOP1 6
#define TRANSMISSION 8000
#define	UPERIOD	400
#define	PRRTH 0.70
#define	TAR 0.85
#define JAMR 0.88

ifstream ScheduleReader;
ifstream TopologyReader;
ifstream TraceReader;
ofstream EnergyRecord;
ofstream LatencyRecord;
ofstream LatencyRecord_B;
ofstream PdrRecord;

ofstream CoutRecord;

ifstream TraceReader_1;


int globalround;

int gindex = 0;
int findex = 0;
int cof=2;
int total_a[HOP];
int success_a[HOP];
int fail_a[HOP];
int block_a[HOP];
int single_a[HOP];
int total_b[HOP];
int success_b[HOP];
int fail_b[HOP];
int block_b[HOP];
int single_b[HOP];
int total_c[HOP1];
int success_c[HOP1];
int fail_c[HOP1];
int block_c[HOP1];
int single_c[HOP1];
float pre_prr[HOP];
int label[HOP];
int label_c[HOP1];
int block_base[HOP];
int effect[HOP];
int tri_num;
float mod=0.12;
vector<int> testbed_trace;
int testbed_trace_index = 0;

vector<int> testbed_trace2;
int testbed_trace_index2 = 0;

vector<int> testbed_trace_1;
int testbed_trace_index_1 = 0;
float stab=2.3;
struct node_attribute {
	int receive;
	int send;
	int receive_wait;
}; 
struct node_attribute node[MAXNODE];
map<int, map<int, map<int, int> > > mmmPRR;  //key1 node1; key2 node2; key3 channel; value PRR
map<int, map<int, map<int, int> > >::iterator itmmmPRR;
		 map<int, map<int, int> >::iterator itmmPRR;
		 		  map<int, int>::iterator itmPRR;
vector<double> energy_v;


void put_trace_to_vector(){
	TraceReader.open("flowtrace/flow1-ha.txt");
	int tt;
	if(TraceReader.is_open()){
		while(1){
			TraceReader>>tt;
			if(TraceReader.eof()){
				break;
			}
			testbed_trace.push_back(tt);
		}
	}

	TraceReader.close();
}

void put_trace_to_vector_1(){
	TraceReader_1.open("flowtrace/flow2.txt");
	int tt;
	if(TraceReader_1.is_open()){
		while(1){
			TraceReader_1>>tt;
			if(TraceReader_1.eof()){
				break;
			}
			testbed_trace_1.push_back(tt);
		}
	}

	TraceReader_1.close();

//	cout << testbed_trace_1.size() << endl;
}

/* read prr of the target flow */
int read_prr()
{
	if(gindex < 1000000)
	{
		int temp = testbed_trace[gindex];
		gindex++;
		if(temp == 1)
		{
			if(gindex%2 == 1)
			{
				gindex++;
			}
			return 0;
		}
		else 
		{
			return 100;
		}
	}
	return 0;
}

int read_prr1()
{
	if(findex < 1000000)
	{
		int temp = testbed_trace_1[findex];
		findex++;
		if(temp == 1)
		{
			if(findex%2 == 1)
			{
				findex++;
			}
			return 0;
		}
		else 
		{
			return 100;
		}
	}
	return 0;
}

int calculate_baseline1_3(int start, int Trans, int link, int fix)
{
	int i, j, k, totala, successa, faila, blocka; 
	int total[HOP], success[HOP], fail[HOP], block[HOP], single[HOP];
	int SU[HOP];
	int singlea;
	float drop;


	gindex = start*8;
	totala = successa = faila = blocka = 0; 
	for (i = 0; i < HOP; i++)
	{
		total[i] = success[i] = fail[i] = block[i] = 0;
	}
	
	for(i=0; i<Trans - fix; i++)
	{
		for(j = 0; j < HOP; j++)
		{
			SU[j] = 0;
		}
		for(j=0; j<HOP; j++)
		{
			if(j == 1)
			{
				if(SU[0] == 0)
				{
					gindex = gindex+6;
					break;
				}
			}
			if(j == 2)
			{
				if(SU[1] == 0)
				{
					gindex = gindex+4;
					break;
				}
			}
			if(j == 3)
			{
				if(SU[2] == 0)
				{
					gindex = gindex+2;
					break;
				}
			}
			if(read_prr() == 0)
			{
				totala++;
				successa++;
				total[j]++;
				success[j]++;
				SU[j] = 1;
			}
			else
			{
				totala++;
				faila++;
				total[j]++;
				fail[j]++;
				if(read_prr() == 0)
				{
					totala++;
					successa++;	
					total[j]++;
					success[j]++;
					SU[j] = 1;
				}
				else
				{
					totala++;
					faila++;
					blocka++;
					total[j]++;
					fail[j]++;
					block[j]++;
				}
			}
		}
	}
//	cout<<"Scheduled transmission is "<< Trans<<endl;
//	cout<<"Total="<<totala<<"	Success="<<successa<<"	Fail="<<faila<<"   Block="<<blocka<<"	PRR = "<< (float)successa/(float)totala<<endl; 
	for(i=0; i<HOP; i++)
	{
		single[i] = fail[i] - 2*block[i];
		block_base[i] = block[i];

	}
	drop = ((1-PRRTH)*(success[link] - single[link]) - 2*PRRTH*(block[link]+single[link]))/(1+PRRTH);
	return single[link] + floor(drop);
}

int update_prr(int start, int Trans, int link)
{
	int i, j, k, totala, successa, faila, blocka; 
	int total[HOP], success[HOP], fail[HOP], block[HOP], single[HOP];
	int SU[HOP];
	int singlea;
	float drop;


	gindex = start*8;
	totala = successa = faila = blocka = 0; 
	for (i = 0; i < HOP; i++)
	{
		total[i] = success[i] = fail[i] = block[i] = 0;
	}
	
	for(i=0; i<Trans; i++)
	{
		for(j = 0; j < HOP; j++)
		{
			SU[j] = 0;
		}
		for(j=0; j<HOP; j++)
		{
			if(j == 1)
			{
				if(SU[0] == 0)
				{
					gindex = gindex+6;
					break;
				}
			}
			if(j == 2)
			{
				if(SU[1] == 0)
				{
					gindex = gindex+4;
					break;
				}
			}
			if(j == 3)
			{
				if(SU[2] == 0)
				{
					gindex = gindex+2;
					break;
				}
			}
			if(read_prr() == 0)
			{
				totala++;
				successa++;
				total[j]++;
				success[j]++;
				SU[j] = 1;
			}
			else
			{
				totala++;
				faila++;
				total[j]++;
				fail[j]++;
				if(read_prr() == 0)
				{
					totala++;
					successa++;	
					total[j]++;
					success[j]++;
					SU[j] = 1;
				}
				else
				{
					totala++;
					faila++;
					blocka++;
					total[j]++;
					fail[j]++;
					block[j]++;
				}
			}
		}
	}
	for(i=0; i<HOP; i++)
	{
		single[i] = fail[i] - 2*block[i];
	}

	total_a[link] = total[link];
	success_a[link] = success[link];
	fail_a[link] = fail[link];
	block_a[link] = block[link];
	single_a[link] = single[link];
	return 0;
}

/* update PRR of All Links */
int update_allprr(int start, int Trans)
{
	int i, j, k, totala, successa, faila, blocka; 
	int total[HOP], success[HOP], fail[HOP], block[HOP], single[HOP];
	int SU[HOP];
	int singlea;
	float drop;
	int link;

	gindex = start*8;
	totala = successa = faila = blocka = 0; 
	for (i = 0; i < HOP; i++)
	{
		total[i] = success[i] = fail[i] = block[i] = 0;
	}
	
	for(i=0; i<Trans; i++)
	{
		for(j = 0; j < HOP; j++)
		{
			SU[j] = 0;
		}
		for(j=0; j<HOP; j++)
		{
			if(j == 1)
			{
				if(SU[0] == 0)
				{
					gindex = gindex+6;
					break;
				}
			}
			if(j == 2)
			{
				if(SU[1] == 0)
				{
					gindex = gindex+4;
					break;
				}
			}
			if(j == 3)
			{
				if(SU[2] == 0)
				{
					gindex = gindex+2;
					break;
				}
			}
			if(read_prr() == 0)
			{
				totala++;
				successa++;
				total[j]++;
				success[j]++;
				SU[j] = 1;
			}
			else
			{
				totala++;
				faila++;
				total[j]++;
				fail[j]++;
				if(read_prr() == 0)
				{
					totala++;
					successa++;	
					total[j]++;
					success[j]++;
					SU[j] = 1;
				}
				else
				{
					totala++;
					faila++;
					blocka++;
					total[j]++;
					fail[j]++;
					block[j]++;
				}
			}
		}
	}

	for(i=0; i<HOP; i++)
	{
		single[i] = fail[i] - 2*block[i];

		link = i;
		total_a[link] = total[link];
		success_a[link] = success[link];
		fail_a[link] = fail[link];
		block_a[link] = block[link];
		single_a[link] = single[link];
	//	cout<<"Total="<<total[i]<<"	Success="<<success[i]<<"	Fail="<<fail[i]<<"    Block="<<block[i]<<"	Single="<<single[i]<<"	PRR = "<< (float)success[i]/(float)total[i]<<endl; 
	}
	return 0;
}


/* update PRR for single transmission*/
int update_single(int start)
{
	int i, j, k; 
	int total[HOP], success[HOP], fail[HOP], block[HOP], single[HOP];
	int SU[HOP];
	int singlea;
	int link;

	gindex = start*8;
	
	for(j = 0; j < HOP; j++)
	{
		SU[j] = 0;
	}
	for(j=0; j<HOP; j++)
	{
		if(j == 1)
		{
			if(SU[0] == 0)
			{
				gindex = gindex+6;
				break;
			}
		}
		if(j == 2)
		{
			if(SU[1] == 0)
			{
				gindex = gindex+4;
				break;
			}
		}
		if(j == 3)
		{
			if(SU[2] == 0)
			{
				gindex = gindex+2;
				break;
			}
		}
		if(read_prr() == 0)
		{
			total_a[j]++;
			success_a[j]++;
			SU[j] = 1;
		}
		else
		{
			total_a[j]++;
			fail_a[j]++;
			if(read_prr() == 0)
			{
				total_a[j]++;
				success_a[j]++;
				SU[j] = 1;
			}
			else
			{
				total_a[j]++;
				fail_a[j]++;
				block_a[j]++;
			}
		}
	}

	for(i=0; i<HOP; i++)
	{
		single_a[i] = fail_a[i] - 2*block_a[i];
	}
	return 0;
}

/* update PRR for single link*/
int update_single_link(int start, int link)
{
	int label = 0;

	gindex = start*8 + link * 2;	

	if(read_prr() == 0)
	{
		total_a[link]++;
		success_a[link]++;
		label = 1;
	}
	else
	{
		total_a[link]++;
		fail_a[link]++;
		if(read_prr() == 0)
		{
			total_a[link]++;
			success_a[link]++;
			label = 1;
		}
		else
		{
			total_a[link]++;
			fail_a[link]++;
			block_a[link]++;
		}
	}
	single_a[link] = fail_a[link] - 2*block_a[link];
	return label;
}

/* confirm there is any transmission */
int update_label(int start)
{
	int i, j; 
	int singlea;
	int link;

	gindex = start*8;
	
	for(j = 0; j < HOP; j++)
	{
		label[j] = 0;
	}

	for(j=0; j<HOP; j++)
	{
		if(j == 1)
		{
			if(label[0] == 0)
			{
				gindex = gindex+6;
				break;
			}
		}
		if(j == 2)
		{
			if(label[1] == 0)
			{
				gindex = gindex+4;
				break;
			}
		}
		if(j == 3)
		{
			if(label[2] == 0)
			{
				gindex = gindex+2;
				break;
			}
		}

		if(read_prr() == 0)
		{
			label[j] = 1;
		}
		else
		{
			if(read_prr() == 0)
			{
				label[j] = 1;
			}
		}
	}
	return 0;
}

int generate_random()
{
	return rand()%100+1;
}

/* Jamming from start*/
float Jam_threshold_b1(int start, int Trans)
{
	int Jam_success;
	int i, j;
	int temp, margin, jam;
	float test, test1, max;
	int newcp[HOP], jam_rec[HOP];
	int target_link;
	int link, counter;
	float fr;

	update_allprr(start-400,400);
	for(link=0; link<HOP; link++)
	{
		pre_prr[link] = (float)success_a[link]/(float)total_a[link] + mod;	
		block_base[link] = block_a[link]; 
		total_a[link] = success_a[link] = fail_a[link] = block_a[link] = single_a[link] = 0;
		jam_rec[link] = 0;
	}

	for(i=start; i<start+UPERIOD; i++)
	{
		/* choose the proper target */
		target_link = -1; 
		max = 0;
		Jam_success = 0;
		for(link=0; link<HOP; link++)
		{	
			fr = 1- pre_prr[link];
			margin = ceil((fr + fr * fr) * (start+UPERIOD -i -1)); 
			test = (float)(success_a[link])/(float)(total_a[link] + cof + margin);
			if(test > PRRTH)
			{
				if(test > max)
				{
					if(jam_rec[link] == 0)
					{	
						max = test;
						target_link = link;
					}
					else
					{
						continue; 
					}
				}
			}
		}
		if(target_link > -1)
		{
		//	cout<<i+1<<"th transmission, schedule attacking link "<<target_link<<endl;
		//  should read the record first
			update_label(i);
			if(target_link > 0)
			{
				if(label[target_link-1] == 0)
				{
					update_single(i);
					continue;		// transmission failure before this link, so skip attacking
				}
			}
			jam = generate_random();
			if(jam > FAIL_RATIO)
			{
				success_a[target_link]++;
				total_a[target_link]++;
			}
			else
			{
				fail_a[target_link]++;
				total_a[target_link]++;
				jam = generate_random();
				if(jam > FAIL_RATIO)
				{
					success_a[target_link]++;
					total_a[target_link]++;
				}
				else
				{
					fail_a[target_link]++;
					total_a[target_link]++;
					block_a[target_link]++;
					jam_rec[target_link] = 1;
					Jam_success = 1;
				}
			}
			single_a[target_link]= fail_a[target_link] - 2 * block_a[target_link];
			for(j=0; j<HOP; j++)
			{
				if(j < target_link)
				{
					jam_rec[j] = 0;
					update_single_link(i, j);
				}
				else if(j > target_link)
				{
					jam_rec[j] = 0;
					if(Jam_success == 0)
					{
						if(j == target_link +1)
						{
							label[j] = update_single_link(i, j);
						}
						else if(j > target_link +1)
						{
							if(label[j-1] ==1)
							{
								label[j] = update_single_link(i, j);
							}
						}
					}
				}
			}

		}
		else
		{	
			update_single(i);
		}
	}

	counter = 0;
	for(link=0; link<HOP; link++)
	{
		if( (float)success_a[link]/(float)total_a[link] < PRRTH )
		{
			counter++;
		}	
		effect[link] = effect[link] + block_a[link];
	}
//	cout<<"Total="<<total_a[link]<<"	Success="<<success_a[link]<<"	Fail="<<fail_a[link]<<"    Block="<<block_a[link]<<"	Single="<<single_a[link]<<"	PRR = "<< (float)success_a[link]/(float)total_a[link]<<endl<<endl; 
//	return (float)success_a[link]/(float)total_a[link];
	return counter;
}


/*  more accurate approximation */
float Jam_threshold_b2(int start, int Trans)
{
	int checkpoint, enable, Jam_success;
	int i, j;
	int temp, margin, jam;
	float test, test1, max;
//	float newcp[HOP];
	int newcp[HOP];
	int jam_rec[HOP];
	int target_link;
	int link;
	int counter;
	float fr;

	enable = 0;
	temp = ceil((1-PRRTH)/(JAMR*(JAMR + PRRTH)) * UPERIOD);
	checkpoint = start + UPERIOD - temp*HOP;
	
	for(i=start; i<start+UPERIOD; i++)
	{
		if(enable == 0)
		{
			if(i == (checkpoint -1))
			{
				update_allprr(start, checkpoint-start);
				for(j=0; j<HOP; j++)
				{
					if(j==0)			
					{
						newcp[j] =  round(((1-PRRTH)*UPERIOD - (1+PRRTH)*block_a[j] - PRRTH*single_a[j])/(JAMR*(JAMR+PRRTH)));
					}
					else if(j==1)
					{
						newcp[j] =  round(((1-PRRTH)*(UPERIOD - JAMR*JAMR*newcp[j-1]) - (1+PRRTH)*block_a[j] - PRRTH*single_a[j])/(JAMR*(JAMR+PRRTH)));
					}
					else if(j==2)
					{
						newcp[j] =  round(((1-PRRTH)*(UPERIOD - JAMR*JAMR*(newcp[j-1] + newcp[j-2])) - (1+PRRTH)*block_a[j] - PRRTH*single_a[j])/(JAMR*(JAMR+PRRTH)));
					}
					else if(j==3)
					{
						newcp[j] =  round(((1-PRRTH)*(UPERIOD - JAMR*JAMR*(newcp[j-1] + newcp[j-2] + newcp[j-3])) - (1+PRRTH)*block_a[j] - PRRTH*single_a[j])/(JAMR*(JAMR+PRRTH)));
					}
				}
				temp = newcp[0] + newcp[1] + newcp[2] + newcp[3];    		//more accurate
			//	cout<<"Total new checkpoint is "<<temp<<endl;
				if(checkpoint < (start + UPERIOD - temp))
				{
					checkpoint = start + UPERIOD - temp;
				}
				else if(checkpoint == (start +UPERIOD - temp))
				{
					enable = 1;
					for(link=0; link<HOP; link++)
					{
						pre_prr[link] = (float)success_a[link]/(float)total_a[link];
						jam_rec[link] = 0;
					}
				//	cout<<"JAMMING START!"<<endl;
				}
			}
		}
		else
		{
			/* choose the proper target */
			target_link = -1; 
			max = 0;
			Jam_success = 0;
			for(link=0; link<HOP; link++)
			{
				fr = 1- pre_prr[link];
				margin = ceil((fr + fr * fr) * (start+UPERIOD -i -1));
				test = (float)(success_a[link])/(float)(total_a[link] + 2*cof + margin);
				if(test > PRRTH)
				{
					if(test > max)
					{
						if(jam_rec[link] == 0)
						{	
							max = test;
							target_link = link;
						}
						else
						{
							continue; 
						}
					}
				}
			}
			if(target_link > -1)
			{
			// should read the record first
				update_label(i);
				if(target_link > 0)
				{
					if(label[target_link-1] == 0)
					{
						update_single(i);
						continue;		// transmission failure before this link, so skip attacking
					}
				}
				jam = generate_random();
				if(jam > FAIL_RATIO)
				{
					success_a[target_link]++;
					total_a[target_link]++;
				}
				else
				{
					fail_a[target_link]++;
					total_a[target_link]++;
					jam = generate_random();
					if(jam > FAIL_RATIO)
					{
						success_a[target_link]++;
						total_a[target_link]++;
					}
					else
					{
						fail_a[target_link]++;
						total_a[target_link]++;
						block_a[target_link]++;
						jam_rec[target_link] = 1;
						Jam_success = 1;
					}
				}
				single_a[target_link]= fail_a[target_link] - 2 * block_a[target_link];
				for(j=0; j<HOP; j++)
				{
					if(j < target_link)
					{
						jam_rec[j] = 0;
						update_single_link(i, j);
					}
					else if(j > target_link)
					{
						jam_rec[j] = 0;
						if(Jam_success == 0)
						{
							if(j == target_link +1)
							{
								label[j] = update_single_link(i, j);
							}
							else if(j > target_link +1)
							{ 
								if(label[j-1] ==1)
								{
									label[j] = update_single_link(i, j);
								}
							}
						}
					}
				}
			}
			else
			{	
				update_single(i);
			}
		}
	}

//	cout<<"Plan_C JAMMING FINISH, ending at  "<<gindex<<"	";
	counter = 0;
	for(link=0; link<HOP; link++)
	{
		if( (float)success_a[link]/(float)total_a[link] < PRRTH )
		{
			counter++;
		}	
		effect[link] = effect[link] + block_a[link];
	}
	return counter;
}

/*  more accurate approximation but disable check */
float Jam_threshold_b3(int start, int Trans)
{
	int checkpoint, enable, Jam_success;
	int i, j;
	int temp, margin, jam;
	float test, test1, max;
//	float newcp[HOP];
	int newcp[HOP];
	int jam_rec[HOP];
	int target_link;
	int link;
	int counter;
	int checkpoint2;
	int temp2;

	enable = 0;
	temp = ceil((1-PRRTH)/(JAMR*(JAMR + PRRTH)) * UPERIOD);
	checkpoint = start + UPERIOD - temp*HOP;
	
	for(i=start; i<start+UPERIOD; i++)
	{
		if(enable == 0)
		{
			if(i == (checkpoint -1))
			{
				update_allprr(start, checkpoint-start);
				for(j=0; j<HOP; j++)
				{
					if(j==0)
					{
						newcp[j] =  round(((1-PRRTH)*UPERIOD - (1+PRRTH)*block_a[j] - PRRTH*single_a[j])/(JAMR*(JAMR+PRRTH)));
					}
					else if(j==1)
					{
						newcp[j] =  round(((1-PRRTH)*(UPERIOD - JAMR*JAMR*newcp[j-1]) - (1+PRRTH)*block_a[j] - PRRTH*single_a[j])/(JAMR*(JAMR+PRRTH)));
					}
					else if(j==2)
					{
						newcp[j] =  round(((1-PRRTH)*(UPERIOD - JAMR*JAMR*(newcp[j-1] + newcp[j-2])) - (1+PRRTH)*block_a[j] - PRRTH*single_a[j])/(JAMR*(JAMR+PRRTH)));
					}
					else if(j==3)
					{
						newcp[j] =  round(((1-PRRTH)*(UPERIOD - JAMR*JAMR*(newcp[j-1] + newcp[j-2] + newcp[j-3])) - (1+PRRTH)*block_a[j] - PRRTH*single_a[j])/(JAMR*(JAMR+PRRTH)));
					}
				}
		
				temp = newcp[0] + newcp[1] + newcp[2] + newcp[3];    
				if(checkpoint < (start + UPERIOD - temp))
				{
					checkpoint = start + UPERIOD - temp;
				}
				else if(checkpoint == (start +UPERIOD - temp))
				{
					enable = 1;
					temp2 = 0;
					for(link=0; link<HOP; link++)
					{
						pre_prr[link] = (float)success_a[link]/(float)total_a[link];
						jam_rec[link] = 0;
						temp2 = temp2 + floor(pre_prr[link] * newcp[link] * (1+pre_prr[link])/stab);
						newcp[link] = floor(pre_prr[link] * newcp[link]);
	//					cout<<"Total="<<total_a[link]<<"	Success="<<success_a[link]<<"	Fail="<<fail_a[link]<<"    Block="<<block_a[link]<<"	Single="<<single_a[link]<<"	PRR = "<< (float)success_a[link]/(float)total_a[link]<<endl;
					}
					checkpoint2 = start + UPERIOD - temp2;
	//				cout<<"JAMMING START!"<<endl;
				}
			}
		}
		else
		{
			if(i< checkpoint2)
			{
				update_single(i);
			}
			target_link = -1; 
			Jam_success = 0;
			for(link=0; link<HOP; link++)
			{
				if(newcp[link]>0)
				{
					newcp[link]--;
					target_link = link;
					update_label(i);
					if(target_link > 0)
					{
						if(label[target_link-1] == 0)
						{
							update_single(i);
							break;		// transmission failure before this link, so skip attacking
						}
					}
					jam = generate_random();
					if(jam > FAIL_RATIO)
					{
						success_a[target_link]++;
						total_a[target_link]++;
					}
					else
					{
						fail_a[target_link]++;
						total_a[target_link]++;
						jam = generate_random();
						if(jam > FAIL_RATIO)
						{
							success_a[target_link]++;
							total_a[target_link]++;
						}
						else
						{
							fail_a[target_link]++;
							total_a[target_link]++;
							block_a[target_link]++;
							jam_rec[target_link] = 1;
							Jam_success = 1;
						}
					}
					single_a[target_link]= fail_a[target_link] - 2 * block_a[target_link];
					for(j=0; j<HOP; j++)
					{
						if(j < target_link)
						{
							jam_rec[j] = 0;
							update_single_link(i, j);
						}
						else if(j > target_link)
						{
							jam_rec[j] = 0;
							if(Jam_success == 0)
							{
								if(j == target_link +1)
								{
									label[j] = update_single_link(i, j);
								}
								else if(j > target_link +1)
								{
									if(label[j-1] ==1)
									{
										label[j] = update_single_link(i, j);
									}
								}
							}
						}
					}
					break;
				}
				else
				{
					continue;
				}
			}
		}
	}
//	cout<<endl<<"Plan_D JAMMING FINISH"<<endl;
	counter = 0;
	for(link=0; link<HOP; link++)
	{
		if( (float)success_a[link]/(float)total_a[link] < PRRTH )
		{
			counter++;
		}	
		effect[link] = effect[link] + block_a[link];
	}	
	return counter;
}


int Jam_threshold2(int start, int num)
{
	int i, j;
	int max[num][HOP];
	int real;
	int counter;
	int risk;

	for(i=0; i<num; i++)
	{
		for(j=0; j<HOP; j++)
		{
			max[i][j] = 0;
		}
	}
	counter = 0;
	risk = 0;
	for(j=0; j<HOP; j++)
	{
		effect[j] = 0;
	}

	for(i=0; i<num; i++)
	{
		risk = Jam_threshold_b1(start+400*i, 400);
		counter = counter + risk;
		for(j=0; j<HOP; j++)
		{
			real = block_a[j];
			if(j == 0)
			{
				max[i][j] = calculate_baseline1_3(start+400*i,400,j,0);
			}
			else if(j == 1)
			{
				max[i][j] = calculate_baseline1_3(start+400*i,400,j,max[i][j-1]);
			}
			else if(j == 2)
			{
				max[i][j] = calculate_baseline1_3(start+400*i,400,j,max[i][j-1]+max[i][j-2]);
			}
			else if(j == 3)
			{
				max[i][j] = calculate_baseline1_3(start+400*i,400,j,max[i][j-1]+max[i][j-2]+max[i][j-3]);
			}
		}
		cout<<i+1<<" UPERIOD"<<"	"<<max[i][0]<<"	"<<block_a[0]<<"	"<<max[i][1]<<"	"<<block_a[1]<<"	"<<max[i][2]<<"	"<<block_a[2]<<"	"<<max[i][3]<<"	"<<block_a[3]<<"	"<<max[i][0]+max[i][1]+max[i][2]+max[i][3]<<"	"<<block_a[0]+block_a[1]+block_a[2]+block_a[3]<<"	"<<risk<<endl;
	}
	cout<<"Total trigger threshold is "<<counter<<endl<<endl;
	
	return counter;
}


int Jam_threshold3(int start, int num)
{
	int i, j;
	int max[num][HOP];
	int min[num][HOP];
	int mid[num][HOP];
	int fix[num][HOP];
	int real;
	int counter;
	int risk;

	for(i=0; i<num; i++)
	{
		for(j=0; j<HOP; j++)
		{
			max[i][j] = 0;
			min[i][j] = 0;
			mid[i][j] = 0;
			fix[i][j] = 0;
		}
	}
	for(j=0; j<HOP; j++)
	{
		effect[j] = 0;
	}
	counter = 0;
	risk = 0;
	for(i=0; i<num; i++)
	{
		risk = Jam_threshold_b2(start+400*i, 400);
		counter = counter + risk;
		for(j=0; j<HOP; j++)
		{
			real = block_a[j];

			if(j == 0)
			{
				max[i][j] = calculate_baseline1_3(start+400*i,400,j,0);
			}
			else if(j == 1)
			{
				max[i][j] = calculate_baseline1_3(start+400*i,400,j,max[i][j-1]);
			}
			else if(j == 2)
			{
				max[i][j] = calculate_baseline1_3(start+400*i,400,j,max[i][j-1]+max[i][j-2]);
			}
			else if(j == 3)
			{
				max[i][j] = calculate_baseline1_3(start+400*i,400,j,max[i][j-1]+max[i][j-2]+max[i][j-3]);
			}
		}
		cout<<i+1<<" UPERIOD"<<"	"<<max[i][0]<<"	"<<block_a[0]<<"	"<<max[i][1]<<"	"<<block_a[1]<<"	"<<max[i][2]<<"	"<<block_a[2]<<"	"<<max[i][3]<<"	"<<block_a[3]<<"	"<<max[i][0]+max[i][1]+max[i][2]+max[i][3]<<"	"<<block_a[0]+block_a[1]+block_a[2]+block_a[3]<<"	"<<risk<<endl;
	}
	cout<<"Total trigger threshold is "<<counter<<endl;
	return counter;
}

/* no check */
int Jam_threshold4(int start, int num)
{
	int i, j;
	int max[num][HOP];
	int real;
	int counter;
	int risk;

	for(i=0; i<num; i++)
	{
		for(j=0; j<HOP; j++)
		{
			max[i][j] = 0;
		}
	}
	for(j=0; j<HOP; j++)
	{
		effect[j] = 0;
	}
	risk = 0;
	counter = 0;
	for(i=0; i<num; i++)
	{
		risk = Jam_threshold_b3(start+400*i, 400);
		counter = counter + risk; 
		for(j=0; j<HOP; j++)
		{
			real = block_a[j] - block_base[j];
			if(j == 0)
			{
				max[i][j] = calculate_baseline1_3(start+400*i,400,j,0);
			}
			else if(j == 1)
			{
				max[i][j] = calculate_baseline1_3(start+400*i,400,j,max[i][j-1]);
			}
			else if(j == 2)
			{
				max[i][j] = calculate_baseline1_3(start+400*i,400,j,max[i][j-1]+max[i][j-2]);
			}
			else if(j == 3)
			{
				max[i][j] = calculate_baseline1_3(start+400*i,400,j,max[i][j-1]+max[i][j-2]+max[i][j-3]);
			}
	
		}
		cout<<i+1<<" UPERIOD"<<"	"<<max[i][0]<<"	"<<block_a[0]<<"	"<<max[i][1]<<"	"<<block_a[1]<<"	"<<max[i][2]<<"	"<<block_a[2]<<"	"<<max[i][3]<<"	"<<block_a[3]<<"	"<<max[i][0]+max[i][1]+max[i][2]+max[i][3]<<"	"<<block_a[0]+block_a[1]+block_a[2]+block_a[3]<<"	"<<risk<<endl;
	}
	cout<<"Total trigger threshold is "<<counter<<endl;
	return counter;
}

//-----------------------------------------------------------main function--------------------------------//

int main(int argc, char const *argv[])
{
	int link, j, suc_ratio;
	float prr, suc;
	int counter;

	srand((unsigned)time(NULL)); 

	put_trace_to_vector();
	put_trace_to_vector_1();

	cout<<"Trace size is "<<testbed_trace.size()<<endl<<endl;

	counter = 0;
	int i=4;
		counter = counter + Jam_threshold2(400+150*i,19-i);
		counter = counter + Jam_threshold3(400+150*i,19-i);
		counter = counter + Jam_threshold4(400+150*i,19-i);

	return 0;
}
