/*
 * C++ Program to Implement Adjacency List
 */
#include <iostream>
#include<fstream>
#include<cstdlib>
#include<cstring>
#include<string>
#include<vector>
#include<map>
#include"graph_ews.h"
#include<time.h>
#include<set>
#include <algorithm>
#include <map>
#include <random>
#include <sys/time.h>
#include <iomanip> 
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


//
//====================================================main====================================================
//
int main(int argc,char **argv)
{
	cout.setf(ios::fixed,ios::floatfield);//Non-scientific notation
	if (argc!=6)
	{cout<<"please input five parameters: infile, delta, edge sampling P, wedge sampling Q,seed for P!"<<endl; exit(1);}
	string infile=argv[1];
	int delta=atoi(argv[2]); 
	float P=atof(argv[3]);
	float Q=atof(argv[4]);
	int seed_p=atoi(argv[5]);
	int seed_q=seed_p+2;

	cout<<"input file: "<<infile<<endl;
	cout<<"delta: "<<delta<<endl;
	cout<<"P: "<<P<<". Q: "<<Q<<endl;
	cout<<"seed_p: "<<seed_p<<". seed_q: "<<seed_q<<endl;

	ifstream in(infile.c_str());
	if(!in){cout<<"open infile failed\n";exit(1);}
	in.close();

//
//====================================================read the graph====================================================
//
	Graph G;
	char buf[100];
	const char* d=" ";
	long a[3];
	double start,time;
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
			G.addEdge(a[0],a[1],a[2],0L);
		}
	}
	G.Initialize();
	cout<<"Finish reading "<<G.getEn()<<" edges and the last edge is "<<a[0]<<" "<<a[1]<<" "<<a[2]<<endl;
	/*G.Printgraph();
	for (int i=0;i<G.getVn();i++)
	{
		sum_d=sum_d+G.nodeDegree(i);
	}
	cout<<"Sum of in and out degree: "<<sum_d<<endl;
	cout<<"the number of edges in hash table: "<<G.getHashn()<<endl;
	G.printGraph();*/
	in.close();

//
//====================================================sampling====================================================
//
	double rn;
	vector<long> ids;
	default_random_engine e(seed_p);
	uniform_real_distribution<double> u(0.0,1.0);
	for(long i=0;i<G.getEn();i++)
	{
		rn=u(e);
		if(rn<=P)
		{
			ids.push_back(i);
		}
	}
	cout<<"number of sampling:"<<ids.size()<<endl;



//
//====================================================counting====================================================
//
	vector<string> motifs={ "12 21 32","12 12 32","12 23 32","12 32 32",
							"12 13 32","12 31 32","12 32 21","12 32 12",
							"12 21 23","12 12 23","12 32 23","12 23 23",
							"12 13 23","12 31 23","12 23 21","12 23 12",
							"12 21 31","12 12 31","12 13 31","12 31 31",
							"12 23 31","12 32 31","12 31 21","12 31 12",
							"12 21 13","12 12 13","12 31 13","12 13 13",
							"12 23 13","12 32 13","12 13 21","12 13 12"
					      };
	start=get_wall_time();
	map<int,long> Mnum;
	int mis[8]={0,1,2,3,5,6,8,9};
	for (int i=0;i<8;i++)                     //��ʾ����Ш��
	{
		Mnum.insert(make_pair(mis[i],0));    //00 0 000
		Mnum.insert(make_pair(mis[i]+16,0)); //01 0 000
		Mnum.insert(make_pair(mis[i]+32,0)); //10 0 000
		Mnum.insert(make_pair(mis[i]+48,0)); //11 0 000
	}
	G.Cmotif(Mnum,ids,delta,Q,seed_q);
	time=get_wall_time()-start;
	int i=0;
	cout<< fixed << setprecision(2); 
	 for (map<int,long>::iterator it=Mnum.begin();it!=Mnum.end();it++)
	 {	
	 	cout<<motifs[i++]<<"       ";
	 	cout <<it->first<<"    ";
	 	cout<<(double)it->second/(P*Q)<<endl;
	 }
	 cout<<"The count result:";
	 for (map<int,long>::iterator it=Mnum.begin();it!=Mnum.end();it++)
	 {	
	 	cout<<(double)it->second/(P*Q)<<' ';
	 }
	cout<<endl<<"CPU time:"<<time<<endl; 
	return 0;
}



