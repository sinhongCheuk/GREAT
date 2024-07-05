/*
 * Basic.cpp
 *
 *  
 *     
 */

#include "Basic.h"

pair<unsigned,unsigned> *p_reservoir;    
unsigned M;              // size of reservoir
vector< vector <unsigned> > neighbor;  
unsigned globalN;        // 
unsigned * localN;       // local triangle detection
unsigned cur_graph_size;
unsigned cur_popBack_pos;

//for RFES_Improve
float globalN_improve;

//for RFES_FullyDynamic
unsigned nb;
unsigned ng;
unsigned cur_res_size;
vector<unsigned> cur_popBack_pos_vec;
 
vector<float> localCount(3223589, 0);  // local triangle estimation result


