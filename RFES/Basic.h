/*
 * Basic.h
 *
 *  
 *      
 */

#ifndef BASIC_H_
#define BASIC_H_


#include<iostream>
#include<string.h>
#include<stdio.h>
#include<cstdio>
#include<vector>
#include<fstream>
#include<algorithm>
#include<map>
#include <math.h>
#include <sstream>
#include <sys/time.h>


using namespace std;

extern pair<unsigned,unsigned> *p_reservoir;    // 水库中每个元素都是一条边
extern unsigned M;
extern vector< vector <unsigned> > neighbor;    // 它的大小一定是水库大小的两倍。这是为了能够记录在水库的每条边的两个顶点的邻居
extern unsigned globalN;
extern unsigned * localN;
extern unsigned cur_graph_size;
extern unsigned cur_popBack_pos;

//for Improve
extern float globalN_improve;
//double detected_triangles = 0;

//for Fully Dynamic
extern unsigned nb;
extern unsigned ng;
extern unsigned cur_res_size;
extern vector<unsigned> cur_popBack_pos_vec;

extern vector<float> localCount;



#endif /* BASIC_H_ */
