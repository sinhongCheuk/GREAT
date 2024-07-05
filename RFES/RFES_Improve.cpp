/*
 * RFES_Improve.cpp
 *
 */

#include "RFES_Improve.h"
#include <iomanip>
#include "time.h"


void RFES_Improve(char * input_graph_filename)
{
	p_reservoir=(pair<unsigned,unsigned> *)malloc(sizeof(pair<unsigned,unsigned>)*M);      // allocate space for reservoir
	neighbor.resize(2*M);                                                                  // allocate space for neighbor
	globalN_improve=0;
  //detected_triangles = 0.0;
  globalN = 0;

	for(unsigned i=0;i<M;i++)
	{
		neighbor[i].clear();
		neighbor[i+M].clear();
	}
	cur_popBack_pos=0;
	cur_graph_size=0;

	pair<int,int> edge;
	unsigned u,v,w;
	hash_table_initial();
 
  clock_t start,finish;
  
  
  double read_time = 0.0;
  double triangle_time = 0.0;
  double elapsed_time = 0.0;
  double LAPE = 0.0;

	struct timeval tvs,tve;
	gettimeofday(&tvs,NULL);
 
  start = clock();

	ifstream stream(input_graph_filename);
	string line;
  int num = 0;
  
  double globalSum = 0;
   
	while(!stream.eof())
	{

    num++;
    if (num % 1000000 == 0) {
        cout << "reading line: " << num << endl;
    }
       
		line.clear();
		getline(stream, line);
		istringstream iss(line, istringstream::in);
		iss >> u >> v>>w;
    
		cur_graph_size++;
		edge=make_pair(u,v);
    
		SampleEdge_improve(edge);
		UpdateNeiborNodeList_improve(edge);
  
	}
  finish = clock();
  elapsed_time += double(finish-start)/CLOCKS_PER_SEC;
 
  cout << "elapsed time: " << std::setprecision(8) << elapsed_time << "s" << endl;
  
 
  cout << "outputing local triangle..." << endl;
  std::ofstream outfile("/data1/zhuoxh/local-RFES-I.txt");
  if (!outfile.is_open()) {
        std::cerr << "can't open file!" << std::endl;
  }
  for (int i = 0; i < localCount.size(); ++i) {
      outfile << i << "\t" << localCount[i] << std::endl;
      globalSum += localCount[i];
  }
  
  cout << "triangle detection: " << std::fixed << std::setprecision(4) << globalN << endl;

  cout << "there are global " << std::setprecision(8) << globalSum / 3 << endl;
  
  outfile.close();
  LAPE = computeLAPE();
  cout << "LAPE: " << std::setprecision(8) << LAPE << endl;
  
  
	free(p_reservoir);
	neighbor.clear();
}


