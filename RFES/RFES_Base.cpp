/*
 * RFES_Base.cpp

 */

#include "RFES_Base.h"
#include <time.h>

void RFES_Base(char * input_graph_filename)
{
    // 水库大小、邻居向量、local_cnt[]数组初始化
	p_reservoir=(pair<unsigned,unsigned> *)malloc(sizeof(pair<unsigned,unsigned>)*M);
	neighbor.resize(2*M);
	localN=(unsigned *)malloc(sizeof(unsigned)*M);

	globalN=0;
	for(unsigned i=0;i<M;i++)
	{
		localN[i]=0;
		neighbor[i].clear();
		neighbor[i+M].clear();
	}
	cur_popBack_pos=0;
	cur_graph_size=0;   // 流入的边数

    // hash_table记录了水库中每个顶点的邻居
	pair<int,int> edge;
	unsigned u,v,w;
	hash_table_initial();

	struct timeval tvs,tve;
	gettimeofday(&tvs,NULL);

	ifstream stream(input_graph_filename);
	string line;
 
  double begin = clock();
 
	while(!stream.eof())
	{
		line.clear();
		getline(stream, line);
		istringstream iss(line, istringstream::in);
		iss >> u >> v;
		cur_graph_size++;
		edge=make_pair(u,v);

		if(cur_graph_size%10000==0)
		{
			gettimeofday(&tve,NULL);
			double span = tve.tv_sec-tvs.tv_sec + (tve.tv_usec-tvs.tv_usec)/1000000.0;
			cout <<"time cost for " << cur_graph_size/10000 << ":" <<span<<endl;
			cout << "the number of triangles:" << globalN <<endl;
		}

		SampleEdge(edge);
		UpdateNeiborNodeList(edge);
	}
 
  double end = clock();
  cout << "elapsed time: " << (double)(end - begin) / CLOCKS_PER_SEC << endl;
	cout << "globalN: " << globalN << endl;
	cout << "cur_popBack_pos: " << cur_popBack_pos << endl;

	free(p_reservoir);
	free(localN);
	neighbor.clear();
}
