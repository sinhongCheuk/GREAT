/**
 * @file main.cpp
 *
 * @brief This file conatins main function to run the algorithm Furl.
 *
 * @author Minsoo Jung (minsoojung@snu.ac.kr)
 * @author Yongsub Lim (yongsub@makinarocks.ai)
 * @author Sunmin Lee (smileeesun@snu.ac.kr)
 * @author U Kang (ukang@snu.ac.kr)
 *
 * @date 2019-01-27
 *
 * @details License: Copyright (c) 2018, Minsoo Jung, Yongsub Lim, Summin Lee, and U Kang
 * @details All rights reserved.
 * @details You may use this code for evaluation.
 * @details For commercial purposes, please contact the author.
 */

///////       Header files        ///////
#include "Furl.h"
#include <iostream>
#include <random>
#include <cassert>
#include <cstring>
#include <stdio.h>
#include <set>
#include "time.h"
#include <fstream>
#include <queue>
#include <string>
#include <sstream>
#include <iomanip>


using namespace std;

// random seed
int random_seed = 0;

// Parameters for Hash function
const unsigned long long arr[8] = {0xFC13C8E7,
                                   0xA2A9FFD4,
                                   0x597ECDDC,
                                   0x8AF8DA7E,
                                   0xAF531D42,
                                   0x842A21DD,
                                   0x1DEE299F,
                                   0xBFEC63E9};

unsigned long long Prime1 = 3584999771;
unsigned long long Prime3 = 67532401;
unsigned long long Prime2 = 4294967291;
unsigned long long Prime4 = 8532401;

/**
 * @brief Hash function
 * @param a An integer
 * @return Hashed integer
 */
uint32_t hash_func(uint32_t a)
{
    a = (a+0x479ab41d) + (a<<8);
    a = (a^0xe4aa10ce) ^ (a>>5);
    a = (a+0x9942f0a6) - (a<<14);
    a = (a^0x5aedd67d) ^ (a>>3);
    a = (a+0x17bea992) + (a<<7);
    return a;
}

/**
 * @brief Hash function for an edge
 * @param u A node of an edge
 * @param v The other node of an edge
 * @param a1 Random integer
 * @param a2 Another random integer
 * @return A real value between 0 and 1
 */
double EdgeHash_p(unsigned long long u, unsigned long long v, unsigned long long a1, unsigned long long b1) {
    unsigned long long edge = (hash_func(u^arr[random_seed%8]) << 32) + hash_func(v^arr[(random_seed+3)%8]);
    return double((((a1*(edge%Prime1)+b1)%Prime1)%Prime4))/Prime4;
}

/**
 * @brief Main function
 */
int main(int argc, char** argv) {

    /*
	if (argc <= 7) {
        // datafile: graph file 
        // output_prefix: for output file path
        // bucket_size:size of reservoir
        // mode:0 means counting without duplicated edges, 1 means counting with duplicated edges
        // extension:1 means output local triangle

        // 加入delta和J是FURL的升级版，但不是无偏的
        //The main idea of FURLB (Algorithm 3) is to build an ensemble estimator efficiently by combining the current estimation with estimations at earlier timesteps through a weighted average scheme.
        //The procedure of FURLB is very similar to that of FURL-0B, but its estimation is computed by a weighted average of estimations obtained at previous times
        //
        // delta:Decaying factor for past estimations
        // J:Bucket length for estimation update
		cerr
				<< "ERROR! Require 7 parameters. datafile (string); output_prefix (string); bucket_size (int); mode (0:bin, 1:wgt); extension (0:off, 1:on); delta (double); J (int)"<< endl;
		exit(1);
	}
    */
    random_seed = time(NULL);
    assert(random_seed>=0);

    /*
    char *datafile = argv[1];
	string output_pfx = argv[2];
	int bucket_size = atoi(argv[3]);
	int wgt = atoi(argv[4]);
	int ext = atoi(argv[5]);
	double delta = atof(argv[6]);
	int J = atoi(argv[7]);
    */
    //char *datafile = "D:\\graphDataset\\Email-Enron-noduplicate";
    std::string datafile = "/data1/zhuoxh/timestamp/sorted_deduplicated_youtube-u-growth.txt";
    //string output_pfx = "C:\\Users\\Administrator\\Desktop\\local-triangle-estimation\\furl\\enron";
    int bucket_size = 100000;
    int wgt = 0;
    int ext = 0;
    double delta = 0.0;
    int J = 2100000000;

    clock_t start,finish;


   // Furl counter(wgt, ext, bucket_size, delta, J);

    mt19937_64 generator(random_seed);

    uniform_int_distribution<unsigned long long> dist2(Prime2/3,Prime2);

    unsigned long long c = dist2(generator);
    unsigned long long d = dist2(generator);

    int sID,tID;
    
    double total_triangle = 0;
    double total_elpased_time = 0;
    double total_lape = 0;
    //double total_maintain_time = 0;
    double total_sampling_time = 0;
    double total_discoverd_trianges = 0;
    double total_counting_time = 0;

    Furl counter(wgt, ext, bucket_size, delta, J);

    counter.reset();

    cout << "Num " << i << " experiment:" << endl;

        //Furl counter(wgt, ext, bucket_size, delta, J);
    std::ifstream file(datafile);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << datafile << std::endl;
        return 1; // 或者其他适当的错误处理
    }

    std::string line;
    // start algorithm
    start = clock();


    long num = 0;

    while(std::getline(file, line)){
        num++;
        if (num % 10000000 == 0) {
           cout << "reading line: " << num << endl;
        }
        std::istringstream iss(line);
        if (!(iss >> sID >> tID)) {
        
          std::cerr << "Error parsing line: " << line << std::endl;
          continue;
        }
        // remove loop
        if(sID==tID) continue;

        // remove edge direction
        unsigned long long maxnode = max(sID, tID);
        unsigned long long minnode = min(sID, tID);
        pair<int,int> edge = make_pair(minnode, maxnode);
        double edge_rank = EdgeHash_p(minnode,maxnode,c,d);

        // run algorithm

        counter.update(edge, edge_rank);

    }

    finish = clock();

    // end algorithm

    vector<double> res = counter.get_counts();
    std::string localfile = "/data1/zhuoxh/local-furl0.txt";
    
    // output local triangle estimation and caculate the error
    counter.outputLocal(res, localfile);
    double lape = counter.computeLAPE();

    double time_s = double(finish-start)/CLOCKS_PER_SEC;
        
    double discovertriangles = counter.getDiscoverdTriangles();
    double global = counter.get_global();

    cout << "elapsed time: " << std::setprecision(8) << time_s << "s" << endl;
    cout << "discovertriangles: " << std::setprecision(8) << discovertriangles << endl;
    cout << "globals: " << std::setprecision(8) << global << endl;
    cout << "\n" << endl;


    return 0;
}
