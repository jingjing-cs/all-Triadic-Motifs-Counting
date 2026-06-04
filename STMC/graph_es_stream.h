#ifndef graph_es_H
#define graph_es_H
#include<iostream>
#include<deque>
#include<vector>
#include<unordered_map>
#include<string>
#include <climits>
#include<random>
#include<map>
#include<algorithm>
#include <sys/time.h>
#include<set>
#include <unordered_set>
#include <iomanip>
#include<fstream>
#include<time.h> 
using namespace std;

#define INF  2147483600
//#define bit 32


double get_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

struct TEdge
{
public:
	long src,dest,tim;
	TEdge()
	{	}
	TEdge(long src,long dest,long tim)
	{
		this->src=src;
		this->dest=dest;
		this->tim=tim;
	}
	const bool operator<(const TEdge& e) const{
		if(e.tim!=tim) return tim<e.tim;
		return false;
	}
	bool operator==(const TEdge& other) const {
        return src == other.src && dest == other.dest && tim == other.tim;
    }
};

namespace std 
{
    template <>
    struct hash<TEdge> 
	{
        size_t operator()(const TEdge& edge) const 
		{
            return hash<long>()(edge.src) ^ hash<long>()(edge.dest) ^ hash<long>()(edge.tim);
        }
    };

    // Ϊ vector<TEdge> ���� std::hash
    template <>
    struct hash<vector<TEdge>> {
        size_t operator()(const vector<TEdge>& edges) const 
		{
            size_t hash_value = 0;
            for (const auto& edge : edges) 
			{
                hash_value ^= hash<TEdge>()(edge);
            }
            return hash_value;
        }
    };
}


class Graph
{
public:	
	deque<TEdge> sedges;   //˫�˶��У��������˽��в����ɾ��
	unordered_map<int,deque<TEdge>> Oadjlist;  //����� 
	unordered_map<int,deque<TEdge>> Iadjlist;  //�����
	unordered_map<int,unordered_map<int, deque<TEdge>>> Hash;
	vector<TEdge> path;     //�洢�ҵ���ÿһ����ͼ 
	void updates(TEdge e, int delta) 
	{
		long t_min=e.tim-delta;   //������С��tim 
		//if(sedges.size()==0||sedges[0].tim>=t_min)
		if(sedges.size()==0||sedges[0].tim>=t_min)
		{
			//add eid into sedges, and updates adj and hash
			sedges.push_back(e);
			Oadjlist[e.src].push_back(e);
			Iadjlist[e.dest].push_back(e);
			Hash[e.src][e.dest].push_back(e);
		}  //sedgesΪ��ʱ/�±�ʱ��-ʱ�����<delta
		else
		{ 
			//add eid into sedges, and delete the first edges;
			deque<TEdge> *S=&sedges;
			if(S->size()>=16)
			{
				int id_min=lower_bound(S->begin(),S->end(),TEdge(0,0,t_min)) - S->begin();
				//�Ƴ����б�ʱ��С��t_min�ı�
				for (int i=0;i<id_min;i++)
				{
					TEdge e_del=sedges[i];
					Oadjlist[e_del.src].pop_front();
					Iadjlist[e_del.dest].pop_front();
					Hash[e_del.src][e_del.dest].pop_front();
					if (Oadjlist[e_del.src].size()==0)
					{
						Oadjlist.erase(e_del.src);
					}
					if (Iadjlist[e_del.dest].size()==0)
					{
						Iadjlist.erase(e_del.dest);
					}
					if (Hash[e_del.src][e_del.dest].size()==0)
					{
						Hash[e_del.src].erase(e_del.dest);
					}
				}
				sedges.erase(sedges.begin(),sedges.begin()+id_min);		
			}
			else
			{
				int id_del=0;
				for (int i=0;i<sedges.size();i++)
				{
					TEdge e_del=sedges[i];
					if(e_del.tim<t_min)
					{
						id_del++;
						Oadjlist[e_del.src].pop_front();
						Iadjlist[e_del.dest].pop_front();
						Hash[e_del.src][e_del.dest].pop_front();
						if (Oadjlist[e_del.src].size()==0)
						{
							Oadjlist.erase(e_del.src);
						}
						if (Iadjlist[e_del.dest].size()==0)
						{
							Iadjlist.erase(e_del.dest);
						}
						if (Hash[e_del.src][e_del.dest].size()==0)
						{
							Hash[e_del.src].erase(e_del.dest);
						}
					}
					else
					{
						break;
					}
				}
				sedges.erase(sedges.begin(),sedges.begin()+id_del);
			}
			sedges.push_back(e);
			Oadjlist[e.src].push_back(e);
			Iadjlist[e.dest].push_back(e);
			Hash[e.src][e.dest].push_back(e);
		}
		
	}
   

	void ExactCountMotifs_3_edges(vector<int> &reservoir_value,int delta,TEdge e,map<int,long>&Mnum)   
	{
		if (sedges.size() < 3) return ; 
		int s,d;
		s=e.src;
		d=e.dest;
		deque<TEdge> *X_1=&Oadjlist[s];
		deque<TEdge> *X_2=&Iadjlist[s];
		deque<TEdge> *Y_1=&Oadjlist[d];
		deque<TEdge> *Y_2=&Iadjlist[d];
		int mids[6];
		mids[0]=0,mids[1]=0;      //0--->入射边，1--->出射边
		C1(reservoir_value,Y_2,mids,Mnum,delta,e,d);
		mids[0]=0,mids[1]=1;
		C1(reservoir_value,Y_1,mids,Mnum,delta,e,d);
		mids[0]=1,mids[1]=0;
		C1(reservoir_value,X_2,mids,Mnum,delta,e,s);
		mids[0]=1,mids[1]=1;
		C1(reservoir_value,X_1,mids,Mnum,delta,e,s);
        //C_2_node(reservoir_value,&Hash[s][d],Mnum,e);
        //C_2_node(reservoir_value,&Hash[d][s],Mnum,e);
		return;
	}
	void C1(vector<int> &reservoir_value,deque<TEdge> *X,int *mids, map<int,long>&Mnum,int delta, TEdge e,long center)
	{
		if (X->size()==0) return;
		TEdge path[3];     //存储e0,e1,e2
 		long path_node[3];    //n3[0]存储center，n3[1]和n3[2]存储另外两个节点
		path[0]=e;
		path_node[0]=center;
		long t=e.tim;       
		long it,tmp,mi;
		if (center==e.src)
		{
			path_node[1]=e.dest;
		}
		if (center==e.dest)
		{	
			path_node[1]=e.src;
		}
		//out adj
		deque<TEdge>* Y;
		for(it=0;it<X->size();it++)
		{	
			long _t=X->at(it).tim;  //获取e1的tim
			long _e_src=X->at(it).src; 
			long _e_dest=X->at(it).dest; //获取e1的的两个节点，用于后续判断是否构成楔形形状
			if(_e_src!=path_node[1]&&_e_dest!=path_node[1])   //判断是否构成楔形形状 
			{
				path[1]=X->at(it);  
				path_node[2]= _e_src==center ? _e_dest : _e_src;    //获取第二个节点
				//find the second edge;
				mids[2]=0;    // e0>e2>e1
				long ll=_t+1;  
				long rr=t-1;
				mids[3]=0;    //计算的子图为楔形子图
				if(ll<=rr)
				{
					//Opposite to edge e0;
					Y=&Hash[path[0].dest][path[0].src];
					if (Y->size())
					{
						mids[4]=0;     //e2=e0
						mids[5]=0;     //方向与e0相反
						tmp=Check_F(Y,ll,rr);      //计算该条件下时序子图数目 
						mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
						Mnum[mi]=Mnum[mi]+tmp;
						reservoir_value[mi]+=tmp;
						//cout<<mids[0]<<" "<<mids[1]<<" "<<mids[2]<<" "<<mids[3]<<" "<<mids[4]<<" "<<mids[5]<<"mi="<<mi<<" tmp0="<<tmp<<endl;
					}
					//Same to edge e0;
					Y=&Hash[path[0].src][path[0].dest];
					if (Y->size())
					{
						mids[4]=0;   //e2=e0
						mids[5]=1;  //方向与e0相同
						tmp=Check_F(Y,ll,rr);
						mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
						Mnum[mi]=Mnum[mi]+tmp;
						reservoir_value[mi]+=tmp;
						//cout<<mids[0]<<" "<<mids[1]<<" "<<mids[2]<<" "<<mids[3]<<" "<<mids[4]<<" "<<mids[5]<<"mi="<<mi<<" tmp1="<<tmp<<endl;
					}
					//Opposite to edge e1
				
					Y=&Hash[path[1].dest][path[1].src];
					if (Y->size())
					{
						mids[4]=1;    //e2=e1
						mids[5]=0;	  //方向与e1相反
						tmp=Check_F(Y,ll,rr);
						mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
						Mnum[mi]=Mnum[mi]+tmp;
						reservoir_value[mi]+=tmp;
						//cout<<mids[0]<<" "<<mids[1]<<" "<<mids[2]<<" "<<mids[3]<<" "<<mids[4]<<" "<<mids[5]<<"mi="<<mi<<" tmp2="<<tmp<<endl;
					}

					//Same to edge e1;
					Y=&Hash[path[1].src][path[1].dest];
					if (Y->size())
					{
						mids[4]=1;    //e2=e1
						mids[5]=1;    //方向与e1相同
						tmp=Check_F(Y,ll,rr);
						mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
						Mnum[mi]=Mnum[mi]+tmp;
						reservoir_value[mi]+=tmp;
					}
					//from low to high 计算三角形子图形状
				
					Y=&Hash[path_node[1]][path_node[2]];
					if (Y->size())
					{
						mids[3]=1;   //计算的是三角形子图
						mids[4]=0;   //mids[4] mids[5]的值为01，e2的方向n3[1]--->n3[2] 
						mids[5]=1;
						tmp=Check_F(Y,ll,rr);
						mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
						Mnum[mi]=Mnum[mi]+tmp;
						reservoir_value[mi]+=tmp;
					}
				
					//from high to low
				
					Y=&Hash[path_node[2]][path_node[1]];
					if (Y->size())
					{
						mids[3]=1;
						mids[4]=1;  //mids[4] mids[5]的值为10，e2的方向n3[2]--->n3[1] 
						mids[5]=0;  
						tmp=Check_F(Y,ll,rr);
						mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
						Mnum[mi]=Mnum[mi]+tmp;
						reservoir_value[mi]+=tmp;
					}
				}
				//find the third edge;  
				long lll=t-delta;       //e0>e1>e2
				long rrr=_t-1;
				if(lll<=rrr)
				{
					//Opposite to edge e0;
					Y=&Hash[path[0].dest][path[0].src];
					if (Y->size())
					{ 
						mids[2]=1;   //e0>e1>e2
						mids[3]=0;
						mids[4]=0;  
						mids[5]=0;
						tmp=Check_F(Y,lll,rrr);
						mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
						Mnum[mi]=Mnum[mi]+tmp;
						reservoir_value[mi]+=tmp;
					}
					//Same to edge e0;
					Y=&Hash[path[0].src][path[0].dest];
					if (Y->size())
					{
						mids[2]=1;
						mids[3]=0;
						mids[4]=0;
						mids[5]=1;
						tmp=Check_F(Y,lll,rrr);
						mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
						Mnum[mi]=Mnum[mi]+tmp;          
						reservoir_value[mi]+=tmp;
					}
				}
			}
		}
	}
	void C_2_node(vector<int> &reservoir_value,deque<TEdge> *X,map<int,long>&Mnum,TEdge e)
	{
		for(int it=0;it<X->size();it++)
		{
			int t=e.tim;
			int _t=X->at(it).tim,_e_src=X->at(it).src,_e_dest=X->at(it).dest;
			if(_t==t)  continue;
			int tmp=0;
			deque<TEdge>* Y_1=&Hash[_e_src][_e_dest];
			deque<TEdge>* Y_2=&Hash[_e_dest][_e_src];
			if(_e_src==e.src&&_e_dest==e.dest)      //e0>e2>e1
			{
				tmp=Check_F(Y_1,_t+1,t-1);
				Mnum[59]+=tmp;     //"12 12 12"
				reservoir_value[59]+=tmp;
				tmp=Check_F(Y_2,_t+1,t-1);
				Mnum[60]+=tmp;       //"12 21 12"    
				reservoir_value[60]+=tmp;             
			}
			else 
			{
				tmp=Check_F(Y_1,_t+1,t-1);
				Mnum[61]+=tmp;     //"12 12 21"
				reservoir_value[61]+=tmp;
				tmp=Check_F(Y_2,_t+1,t-1);
				Mnum[62]+=tmp;    // "12 21 21"
				reservoir_value[62]+=tmp;
			}
		}
	}
	void delete_digital(vector<int> &reservoir_value,map<int,long> &Mnum)
	{
		for(map<int,long>::iterator it=Mnum.begin();it!=Mnum.end();it++)
		{
			long r = reservoir_value[it->first];
			if (r != 0)
			{
				it->second = (it->second >= r) ? (it->second - r) : 0;
				reservoir_value[it->first] = 0;
			}
		}
	}		
	long Check_F(deque<TEdge>*X,long left,long right);
};


	long Graph::Check_F(deque<TEdge>* X, long left,long right)
	{
		long pos0,pos1,tr=0;
		pos0=lower_bound(X->begin(),X->end(),TEdge(0,0,left))-X->begin();
		pos1=upper_bound(X->begin(),X->end(),TEdge(0,0,right))-X->begin()-1;
		if (pos0>X->size()||pos1<0||(pos0>pos1))   tr=0;
		else    tr=pos1-pos0+1;
		return tr;
	}

#endif


