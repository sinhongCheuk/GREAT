/*
 * hash.h
 *
 */

#ifndef HASH_H_
#define HASH_H_
#include "Basic.h"


extern vector< vector<unsigned> > hash_table;   // 记录顶点在reservoir中的下标
extern unsigned hash_table_size;

void hash_table_initial();
void hash_table_insert(unsigned node,unsigned res_index);
void hash_table_delete(unsigned node,unsigned res_index);
unsigned hash_function(unsigned a);
void hash_table_insert_edge(pair<unsigned,unsigned> edge,unsigned res_index);
void hash_table_delete_edge(pair<unsigned,unsigned> edge,unsigned res_index);
int find_in_res(pair<unsigned,unsigned> edge);

#endif /* HASH_H_ */