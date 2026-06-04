/*
 * C++ Program to Implement Adjacency List
 */
#include <iostream>
#include<fstream>
#include<cstdlib>
#include<cstring>
#include<string>
#include<vector>
#include"graph_es_stream.h"
#include<time.h>
#include<set>
#include <algorithm>
#include <map>
#include <random>
#include <sys/time.h>
using namespace std;



//
//====================================================get_wall_time====================================================
//
double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

void result_print(int m,int E,const map<int,long>& Mnum)
{
	if(m>=E)
	{
		for(auto it:Mnum)	cout<<it.second<<' ';
	}
	else
	{
		cout<<fixed<<setprecision(2);
		for(auto it:Mnum)	cout<<double(it.second)/(double(m)/double(E))<<' ';		
	}
	cout<<endl;
}

void reservoir_print(int m,map<int,long>Mnum,const vector<vector<int>>& reservoir) 
{
	vector<int> mp(64,0);
	for(int i=0;i<m;i++)
	{
		for(auto it:Mnum )
		{
			int temp=reservoir[i][it.first];
			if(temp!=0)
			{
				mp[it.first]+=temp;
			}
		}
	}
	cout<<"reservoir value : ";
	cout<<fixed<<setprecision(2);
	for(auto it:Mnum) cout<<mp[it.first]<<' ';
	cout<<endl;  
}


//
//====================================================main====================================================
//
int main(int argc,char **argv)
{
	cout.setf(ios::fixed,ios::floatfield);//Non-scientific notation
	if (argc!=5)
	{cout<<"please input five parameters: infile, delta, size of reservoir seed !"<<endl; exit(1);}
	string infile=argv[1];
	int delta=atoi(argv[2]);
	int m=atoi(argv[3]);
	int seed=atoi(argv[4]);
	cout<<"input file: "<<infile<<endl;
	cout<<"delta: "<<delta<<endl;
	cout<<"size of reservoir: "<<m<<endl;
	cout<<"seed: "<<seed<<endl;
	std::mt19937 gen(seed);
	ifstream in(infile.c_str());
	if(!in){cout<<"open infile failed\n";exit(1);}
	in.close();




//
//====================================================read the graph====================================================
//
	vector<TEdge> e_stream;
	const char* d=" ";
	long a[3];
	char buf[100];
	in.open(infile.c_str());
	while (in.getline(buf,100)) // 
	{ 
		char* p=strtok(buf,d);
		int i=0;
		while (p){			
			a[i]=atol(p); // 
			p=strtok(NULL,d);
			i=i+1;	
		}
		delete p;
		if (a[0] != a[1]) { 
			TEdge *e=new TEdge(a[0],a[1],a[2]);
			e_stream.push_back(*e);
			delete e;
		}
	}
	in.close();
	long E=e_stream.size();
	cout<<"Finish reading "<<" "<<E<<" edges."<<endl;

//
//====================================================sampling====================================================
//
	Graph G;
	vector<vector<int>> reservoir(m,vector<int>(64,0));//sampled edge id
	long rn;
	double start,time;
	map<int,long> Mnum;
	vector<int> ids(m,0);
	int mis[8]={0,1,2,3,5,6,8,9};
	for (int i=0;i<8;i++)                   
	{
		Mnum.insert(make_pair(mis[i],0));    //00 0 000
		Mnum.insert(make_pair(mis[i]+16,0)); //01 0 000
		Mnum.insert(make_pair(mis[i]+32,0)); //10 0 000
		Mnum.insert(make_pair(mis[i]+48,0)); //11 0 000
	}   //插入记录楔形子图的位置
	//for(int i=0;i<4;i++)	Mnum.insert(make_pair(59+i,0));  //插入记录两节点子图位置

	start=get_wall_time();
	int month=5;
	for (int e_id=0;e_id<E;e_id++)
	{
		G.updates(e_stream[e_id],delta);
		if(e_id<m)
		{
			ids[e_id]=e_id;
			G.ExactCountMotifs_3_edges(reservoir[e_id],delta,e_stream[e_id],Mnum);
		}
		else
		{
			std::uniform_int_distribution<> dis(0, e_id);
 			rn=dis(gen);  
			if(rn<m)
			{
				ids[rn]=e_id; 
				G.delete_digital(reservoir[rn],Mnum); 
				G.ExactCountMotifs_3_edges(reservoir[rn],delta,e_stream[e_id],Mnum);
			}
		}
	}
	time=get_wall_time()-start;
	// for(int i=0;i<m;i++)
	// {
	// 	cout<<"reservoir_value:  "<<i<<endl;
	// 	cout<<"TEdge id: "<<ids[i]<<endl;
	// 	for(auto it:Mnum )
	// 	{
	// 		int temp=reservoir[i][it.first];
	// 		if(temp!=0)
	// 		{
	// 			cout<<it.first<<"      ";
	// 			cout<<reservoir[i][it.first]<<endl;
	// 		}
	// 	}
	// 	cout<<endl;
	// }          //输出蓄水池每个格子里的具体元素
	// if(m>=E)
	// for (map<int,long>::iterator it=Mnum.begin();it!=Mnum.end();it++)
	// {	
	// 	cout <<it->first<<"    ";
	// 	cout<<it->second<<endl;
	// }
	// else 
	// {
	// 	cout<< fixed << setprecision(2);
	// 	for (map<int,long>::iterator it=Mnum.begin();it!=Mnum.end();it++)
	// 	{	
	// 		cout <<it->first<<"    ";
	// 		cout<<double(it->second)/(double(m)/double(E))<<endl;
	// 	}
	// 	cout<<endl;
	// }
	result_print(m,E,Mnum);
	cout<<endl<<"cpu Time:"<<time<<endl; 
	return 0;
}



