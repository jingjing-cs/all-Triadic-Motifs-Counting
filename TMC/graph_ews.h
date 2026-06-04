#ifndef graph_ews_H
#define graph_ews_H
#include<iostream>
#include<vector>
#include<map>
#include<unordered_map>
#include<string>
#include <climits>
#include<random>
#include<algorithm>
using namespace std;

//#define N 2147483647
//#define bit 32


struct TEdge
{
public:
	long src,dest,tim,id;
	TEdge(){}
	TEdge(long src,long dest,long tim,long id)
	{
		this->src=src;
		this->dest=dest;
		this->tim=tim;
		this->id=id;
	}
	const bool operator<(const TEdge& e) const{
		if(e.tim!=tim) return tim<e.tim;
		if(e.src!=src) return src<e.src;
		if(e.dest!=dest) return dest<e.dest;
		if(e.id!=id) return id<e.id;
		return false;
	}
};

class Etim
{
public:
	long node,tim,id; 
	Etim(long node,long tim,long id)
	{
		this->node=node;
		this->tim=tim;
		this->id=id;
	}
	const bool operator<(const Etim& e) const{
		return tim<e.tim;
	}
};

class Graph
{
public:	
	long Vn;
	long En;
	vector<vector<Etim>> Oadjlist;
	vector<vector<Etim>> Iadjlist;
	vector< unordered_map<long, vector<Etim> > > Hash;
	vector<TEdge> edges;

	inline void addEdge(long src,long dest,long tim,long id)
	{
		TEdge e(src, dest, tim, id);
		edges.push_back(e);
	}

	void Initialize()
	{
		unordered_map<long,long> vmap;
		int id=0;
		for(vector<TEdge>::iterator it=edges.begin();it!=edges.end();it++)
		{
			if(!vmap.count((*it).src)){
				vmap[(*it).src]=id++;
			}
			if(!vmap.count((*it).dest)){
				vmap[(*it).dest]=id++;
			}
		}
		Vn=id;
		En=edges.size();
		for(vector<TEdge>::iterator it=edges.begin();it!=edges.end();it++)
		{
			(*it).src=vmap[(*it).src];
			(*it).dest=vmap[(*it).dest];
		}
		vmap.clear();
		sort(edges.begin(),edges.end());

		for (int i = 0; i < (int) edges.size(); i++) {
			edges[i].id = i;
		}

		Oadjlist.clear();
		Iadjlist.clear();
		Hash.clear();

		Oadjlist.resize(id);
		Iadjlist.resize(id);
		Hash.resize(id);

		
		for(vector<TEdge>::iterator it=edges.begin();it!=edges.end();it++)
		{
			Etim _es((*it).src,(*it).tim,(*it).id);
			Etim _ed((*it).dest,(*it).tim,(*it).id);
			Etim _eh(0L,(*it).tim,(*it).id);
			Oadjlist[(*it).src].push_back(_ed);
			Iadjlist[(*it).dest].push_back(_es);
			Hash[(*it).src][(*it).dest].push_back(_eh);
		}
	}
	long nodeDegree(long node)
	{
		return Oadjlist[node].size()+Iadjlist[node].size();
	}
	long getVn()
	{
		return Vn;
	}

	long getEn()
	{
		return En;
	}

	long getHashn()
	{
		long sum=0;
		for (int i=0;i<Vn;i++)
		{
			for(int j=0;j<Vn;j++)
				if(Hash[i].find(j)!=Hash[i].end())
				{sum=sum+Hash[i][j].size();}
		}
		return sum;
	}
	void Printgraph()
	{
		for (int i=0;i<En;i++)
			cout<<edges[i].src<<" "<<edges[i].dest<<" "<<edges[i].tim<<" "<<edges[i].id<<endl;
	}
//e0,e1,e2分别指第1，2，3条被采样的边
//m_e0,m_e1,m_e2分别指大小为1，2，3的边，即m_e0<m_e1<m_e2 
//e_a=e_b，即e_a和e_b具有相同的节点
	void Cmotif(map<int,long>& Mnum, vector<long>&ids,int delta,float Q,int seed)
	{
		//ids数组中存储被采样的所有边
		for(int i=0;i<ids.size();i++)
		{
			TEdge e=edges[ids[i]];  //e0
			long s=e.src;
			long d=e.dest;
			//mids[0]入射边 mids[1]出射边
			//以起始节点e.src作为中心节点，即s
			int mids[6];   
			vector<Etim>* X; 
			X=&Oadjlist[s];
			mids[0]=1;          //取值由中心节点确定，以src为中心节点，mids[0]=1,以dest为中心节点，mids[0]=1
			mids[1]=1;          //取值由X确定，X=&Oadjlist[s]，mids[1]=1; X=&Iadjlist[s],mids[1]=0
			C1(X,mids,Mnum,delta,e,s,Q,seed);
			X=&Iadjlist[s];
			mids[1]=0;          
			C1(X,mids,Mnum,delta,e,s,Q,seed);
			//以起始节点e.dest为中心节点，即d
			X=&Oadjlist[d];
			mids[0]=0;
			mids[1]=1;
			C1(X,mids,Mnum,delta,e,d,Q,seed);
			X=&Iadjlist[d]; 
			mids[1]=0;
			C1(X,mids,Mnum,delta,e,d,Q,seed);
		}
	}
	void C1(vector<Etim>*X,int *mids, map<int,long>&Mnum,int delta, TEdge e,long center, float Q,int seed)
	{
		TEdge e3[3];     //存储e0，e1，e2
		long n3[3];    //n3[0]存储center，n3[1]和n3[2]存储另外两个节点
		e3[0]=e;
		n3[0]=center;
		long t=e.tim;
		long left=t+1;     //e1>e0，e2大于e0，其余大小未知
		long right=t+delta;  
		long head=0,it,tmp,mi;
		if (center==e.src)
		{
			n3[1]=e.dest;
		}
		if (center==e.dest)
		{	
			n3[1]=e.src;
		}
		//out adj
		vector<Etim>* Y;
		bool small=(X->size()<16);  //大于16使用二分，小于16直接遍历
		if(X->size())
		{	
			if(!small)
			{
				head=lower_bound(X->begin(),X->end(),Etim(0,left,0))-X->begin();
			}				
			for(it=head;it<X->size();it++)
			{	
				long _t=X->at(it).tim;  //获取e1的tim
				if(_t<left||_t>right)
				{
					if(small) continue;
					else break;
				}    //e1的tim值不符合要求
				long _e=X->at(it).node;//edges[X->at(it).id].dest;
				if(_e!=n3[1])   //判断是否构成楔形形状 
				{
					double rn=Rnum(seed);
					if(rn<=Q)   //对改边进行楔形采样
					{
						e3[1]=edges[X->at(it).id];   //获取e1的id
						n3[2]=_e;    //获取第二个节点
						//find the second edge;
						mids[2]=0;    // m_e1=m_e0或m_e1=m_e2 
						long ll=left;   //e0<e2<e1
						long rr=_t-1;
						mids[3]=0;    //计算的子图为楔形子图
						//Opposite to edge e0;
						Y=&Hash[e3[0].dest][e3[0].src];
						if (Y->size())
						{
							mids[4]=0;     //e2=e0，满足m_e1=m_e0
							mids[5]=0;     //方向与e0相反
							tmp=Check_F(Y,ll,rr);      //计算该条件下时序子图数目 
							mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
							Mnum[mi]=Mnum[mi]+tmp;
							//cout<<mids[0]<<" "<<mids[1]<<" "<<mids[2]<<" "<<mids[3]<<" "<<mids[4]<<" "<<mids[5]<<"mi="<<mi<<" tmp0="<<tmp<<endl;
						}
						//Same to edge e0;
						Y=&Hash[e3[0].src][e3[0].dest];
						if (Y->size())
						{
							mids[4]=0;   //e2=e0
							mids[5]=1;  //方向与e0相同
							tmp=Check_F(Y,ll,rr);
							mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
							Mnum[mi]=Mnum[mi]+tmp;
							//cout<<mids[0]<<" "<<mids[1]<<" "<<mids[2]<<" "<<mids[3]<<" "<<mids[4]<<" "<<mids[5]<<"mi="<<mi<<" tmp1="<<tmp<<endl;
						}
						//Opposite to edge e1
					
						Y=&Hash[e3[1].dest][e3[1].src];
						if (Y->size())
						{
							mids[4]=1;    //e2=e1,满足m_e1=m_e2
							mids[5]=0;	  //方向与e1相反
							tmp=Check_F(Y,ll,rr);
							mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
							Mnum[mi]=Mnum[mi]+tmp;
							//cout<<mids[0]<<" "<<mids[1]<<" "<<mids[2]<<" "<<mids[3]<<" "<<mids[4]<<" "<<mids[5]<<"mi="<<mi<<" tmp2="<<tmp<<endl;
						}

						//Same to edge e1;
						Y=&Hash[e3[1].src][e3[1].dest];
						if (Y->size())
						{
							mids[4]=1;    //e2=e1
							mids[5]=1;    //方向与e1相同
							tmp=Check_F(Y,ll,rr);
							mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
							Mnum[mi]=Mnum[mi]+tmp;
							//cout<<mids[0]<<" "<<mids[1]<<" "<<mids[2]<<" "<<mids[3]<<" "<<mids[4]<<" "<<mids[5]<<"mi="<<mi<<" tmp3="<<tmp<<endl;
						}
						//from low to high 计算三角形子图形状
					
						Y=&Hash[n3[1]][n3[2]];
						if (Y->size())
						{
							mids[3]=1;   //计算的是三角形子图
							mids[4]=0;   //mids[4] mids[5]的值为01，e2的方向n3[1]--->n3[2] 
							mids[5]=1;
							tmp=Check_F(Y,ll,rr);
							mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
							Mnum[mi]=Mnum[mi]+tmp;
							//cout<<mids[0]<<" "<<mids[1]<<" "<<mids[2]<<" "<<mids[3]<<" "<<mids[4]<<" "<<mids[5]<<"mi="<<mi<<" tmp4="<<tmp<<endl;
						}
					
						//from high to low
					
						Y=&Hash[n3[2]][n3[1]];
						if (Y->size())
						{
							mids[3]=1;
							mids[4]=1;  //mids[4] mids[5]的值为10，e2的方向n3[2]--->n3[1] 
							mids[5]=0;  
							tmp=Check_F(Y,ll,rr);
							mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
							Mnum[mi]=Mnum[mi]+tmp;
							//cout<<mids[0]<<" "<<mids[1]<<" "<<mids[2]<<" "<<mids[3]<<" "<<mids[4]<<" "<<mids[5]<<"mi="<<mi<<" tmp5="<<tmp<<endl;
						}
					
						//find the third edge;  
						long lll=_t+1;       //e0<e1<e2 
						long rrr=t+delta;
					
						//Opposite to edge e0;
					
						Y=&Hash[e3[0].dest][e3[0].src];
						if (Y->size())
						{ 
							mids[2]=1;   //m_e0=m_e2 
							mids[3]=0;
							mids[4]=0;  
							mids[5]=0;
							tmp=Check_F(Y,lll,rrr);
							mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
							Mnum[mi]=Mnum[mi]+tmp;
							//cout<<mids[0]<<" "<<mids[1]<<" "<<mids[2]<<" "<<mids[3]<<" "<<mids[4]<<" "<<mids[5]<<"mi="<<mi<<" tmp6="<<tmp<<endl;
						}
						//Same to edge e0;
						Y=&Hash[e3[0].src][e3[0].dest];
						if (Y->size())
						{
							mids[2]=1;
							mids[3]=0;
							mids[4]=0;
							mids[5]=1;
							tmp=Check_F(Y,lll,rrr);
							mi=mids[0]<<5|mids[1]<<4|mids[2]<<3|mids[3]<<2|mids[4]<<1|mids[5];
							Mnum[mi]=Mnum[mi]+tmp;
							//cout<<mids[0]<<" "<<mids[1]<<" "<<mids[2]<<" "<<mids[3]<<" "<<mids[4]<<" "<<mids[5]<<"mi="<<mi<<" tmp7="<<tmp<<endl;
						}
					
					}
				}
			}
		}
	}		
	long Check_F(vector<Etim>*X,long left,long right);
	double Rnum(int seed);
};

long Graph::Check_F(vector<Etim>* X, long left,long right)
{
	long pos0,pos1,tr=0;
	pos0=lower_bound(X->begin(),X->end(),Etim(0,left,0))-X->begin();
	pos1=upper_bound(X->begin(),X->end(),Etim(0,right,0))-X->begin()-1;
	if (pos0>X->size()||pos1<0||(pos0>pos1))
	{
		tr=0;
	}
	else 
	{
		tr=pos1-pos0+1;
	}
	return tr;
}


double Graph::Rnum(int seed)
{
	static std::default_random_engine e(seed);
	static std::uniform_real_distribution<double> u(0.0,1.0);
	return u(e);
}

#endif
