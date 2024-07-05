
#include "Furl.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip> // For std::setw

using namespace std;

/**
 * @brief Prepare memory spaces for a node
 * @param u Node
 */
void Furl::ready_node(const int u){
    maxnodeID = max(maxnodeID, u);

    if(wgt == 0){
        if( node_map_.find(u) == node_map_.end() ){
            node_map_[u] = vector<int>();
        }
    }else if(wgt == 1){
        if( occurrences.find(u) == occurrences.end() ){
            occurrences[u] = unordered_map<int,int>();
        }
    }

    if( counts.find(u) == counts.end() ){
        counts[u] = 0.0;
    }
}

/**
 * @brief Run the algorithm on a new edge
 * @param edge New edge
 * @param edge_rank Hash value of the new edge
 */
void Furl::update(pair<int,int>& edge, double edge_rank){
    time++;
    ready_node(edge.first);
    ready_node(edge.second);
    if(ext == 1){
        if((!exactcnt) && ((time - TM) % J == 0)){
            weighted_average(delta);
        }
    }
    if(wgt == 0){
        if(!contain_edge(edge.first, edge.second)){
            samplingTime1 = clock();
            bool sampled = sample_new_edge(edge, edge_rank);
            samplingTime2 = clock();
            samplingTime += double(samplingTime2 - samplingTime1) / CLOCKS_PER_SEC;

            countingTime1 = clock();
            update_triangles(edge, sampled);
            countingTime2 = clock();
            countingTime += double(countingTime2 - countingTime1) / CLOCKS_PER_SEC;
        }
    }else if(wgt == 1){
        update_triangles(edge, 1);
        if(contain_edge(edge.first, edge.second)){
            increase_occurrences(edge.first, edge.second, 1);
        }else{
            sample_new_edge(edge, edge_rank);
        }
    }
}

/**
 * @brief Sample a new edge through reservoir sampling with random hash
 * @param edge New edge
 * @param edge_rank Hash value of the new edge
 * @return True if the edge is sampled
 */
bool Furl::sample_new_edge(pair<int,int>& edge, double edge_rank){
    if(buffer.size() < bucket_size){
        buffer.push(make_pair(edge, edge_rank));
        add_edge_sample(edge.first, edge.second);
        return 1;
    }else{
        if(exactcnt){
            if(wgt == 0) TM = time - 1;
            else if(wgt == 1) TM = time;
            if(ext == 1)weighted_average(0.0);
            exactcnt = 0;
        }
        if(edge_rank < buffer.top().second){
            remove_edge_sample(buffer.top().first.first, buffer.top().first.second);
            buffer.pop();
            buffer.push(make_pair(edge, edge_rank));
            add_edge_sample(edge.first, edge.second);
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Update triangle counts
 * @param edge New edge
 * @param sampled Flag to check whether an edge is sampled
 */
void Furl::update_triangles(pair<int,int>& edge, bool sampled){
    if(exactcnt){
        add_triangles(edge.first, edge.second, 1.0);
    }
    else{
        double qT = 0;
        if(sampled){
            if(wgt == 0)qT = (((double)bucket_size - 3.0) / (double)bucket_size) / pow(buffer.top().second, 3.0);
            else if(wgt == 1)qT = (((double)bucket_size - 2.0) / (double)bucket_size) / pow(buffer.top().second, 2.0);
            add_triangles(edge.first, edge.second, qT);
        }
    }
}

/**
 * @brief Add an edge to the sampled graph
 * @param u A node of edge
 * @param v The other node of edge
 */
void Furl::add_edge_sample(const int u, const int v){
    assert(u!=v);
    if(wgt == 0){
        node_map_[u].push_back(v);
        node_map_[v].push_back(u);
    }else if(wgt == 1){
        occurrences[u][v] = 1;
        occurrences[v][u] = 1;
    }
}

/**
 * @brief Remove an edge from the sampled graph
 * @param u A node of edge
 * @param v The other node of edge
 */
void Furl::remove_edge_sample(const int u, const int v){
    assert(u!=v);
    if(wgt == 0){
        if (node_map_[u].size() == 1) {
            node_map_.erase(u);
        } else {

            *std::find(node_map_[u].begin(), node_map_[u].end(),
                       v) = node_map_[u][node_map_[u].size() - 1];
            node_map_[u].resize(node_map_[u].size() - 1);
        }

        if (node_map_[v].size() == 1) {
            node_map_.erase(v);
        } else {

            *std::find(node_map_[v].begin(), node_map_[v].end(),
                       u) = node_map_[v][node_map_[v].size()
                                         - 1];
            node_map_[v].resize(node_map_[v].size() - 1);
        }
    }else if(wgt==1){
        if(occurrences[u].size() == 1)occurrences.erase(u);
        else occurrences[u].erase(v);
        if(occurrences[v].size() == 1)occurrences.erase(v);
        else occurrences[v].erase(u);
    }
}

/**
 * @brief Update triangle counts changed by a new edge
 * @param u A node of edge
 * @param v The other node of edge
 * @param weight Weight of the triangle
 */
void Furl::add_triangles(const int u, const int v, double weight){
    assert(u!=v);
    int min_deg_n = (degree(u) <= degree(v) ? u : v);
    int max_deg_n = (min_deg_n == u ? v : u);

    double weightSum = 0;

    if(wgt == 0){
        for(const auto& n : node_map_[min_deg_n]){
            if(n!= max_deg_n){
                if(find(node_map_[max_deg_n].begin(),node_map_[max_deg_n].end(),n) != node_map_[max_deg_n].end()){
                    discoverd_triangles++;

                    counts[n] += weight;
                    weightSum += weight;
                }
            }
        }
    }else if (wgt == 1){
        for(const auto& n : occurrences[min_deg_n]){
            if(n.first!= max_deg_n){
                if(occurrences[max_deg_n].find(n.first) != occurrences[max_deg_n].end()){
                    double value = (double)occurrences[u][n.first] * (double)occurrences[v][n.first] * weight;
                    counts[n.first] += value;
                    weightSum += value;
                }
            }
        }
    }

    counts[u] += weightSum;
    counts[v] += weightSum;

    global += weightSum;
}

/**
 * @brief Combine the current estimation with estimations at earlier timesteps through a weighted average scheme
 * @param delta Decaying factor
 */
void Furl::weighted_average(double delta){
    for(auto& el : counts) {
        if(delta == 0.0)
            estimations[el.first] = el.second;
        else{
            double wgt_val = delta * estimations[el.first] + (1.0 - delta) * el.second;
            estimations[el.first] = wgt_val;
        }
    }
}

/**
 * @brief Return triangle counts of FURL-0 for all nodes
 * @return List of triangle counts of FURL-0
 */
vector<double> Furl::get_counts(){
    vector<double> ret;

    ret.resize(maxnodeID+1);

    for(auto& el : counts){
        ret[el.first] = el.second;
    }

    return ret;
}

/**
 * @brief Return triangle counts of FURL for all nodes
 * @return List of triangle counts of FURL
 */
vector<double> Furl::get_estimations(){

    if(ext==1){
        if((!exactcnt) && (time > TM)){
            if((time - TM) % J != 0)weighted_average(delta);
        }
    }

    vector<double> ret;

    ret.resize(maxnodeID+1);

    for(auto& el : estimations){
        ret[el.first] = el.second;
    }

    return ret;
}

/**
 * @return Global triangle estimation
 */
double Furl::get_global(){
    return global;
}


/**
 * @brief output local triangle estimation to file
 * @param res A map of vertex-local triangle estimation
 * @param filename The output file path
 */
void Furl::outputLocal(const std::vector<double>& res, const std::string& filename){
    std::ofstream outFile(filename);

    if (outFile.is_open()) {
        for (int i = 0; i < res.size(); ++i) {
            outFile << i << "\t" << res[i] << std::endl;
        }
        outFile.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}
/**
 * @brief Caculate the local triangle estimation error and print it
 */
double Furl::computeLAPE() {
    std::string algorithmOutputFile = "/data1/zhuoxh/local-furl0.txt";        // local triangle estimation file path
    std::string groundTruthFile = "/data1/zhuoxh/local-youtube-u-growth.txt"; // local triangle groundtruth file path

    std::ifstream groundTruthStream(groundTruthFile);
    std::ifstream algorithmOutputStream(algorithmOutputFile);

    if (!groundTruthStream.is_open() || !algorithmOutputStream.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return -1;
    }

    std::string groundTruthLine;
    std::string algorithmOutputLine;

    double totalLAPE = 0;
    int vertexCount = 0;

    while (std::getline(groundTruthStream, groundTruthLine) &&
           std::getline(algorithmOutputStream, algorithmOutputLine)) {
        std::istringstream groundTruthSS(groundTruthLine);
        std::istringstream algorithmOutputSS(algorithmOutputLine);

        int groundTruthVertex;
        int algorithmOutputVertex;
        double groundTruthValue;
        double algorithmOutputValue;

        groundTruthSS >> groundTruthVertex >> groundTruthValue;
        algorithmOutputSS >> algorithmOutputVertex >> algorithmOutputValue;

        double lape = std::abs(algorithmOutputValue - groundTruthValue) / (groundTruthValue + 1);
        totalLAPE += lape;

        vertexCount++;
    }

    groundTruthStream.close();
    algorithmOutputStream.close();

    double averageLAPE = totalLAPE / vertexCount;
    std::cout << "vertex num in local file: " << vertexCount << std::endl;
    std::cout << "Average Local Absolute Percentage Error (LAPE): " << std::setprecision(8) << averageLAPE << std::endl;

    return averageLAPE;
}

/**
 * @return triangle detection
 */

long Furl::getDiscoverdTriangles() {
    return discoverd_triangles;
}

/**
 * @brief Return minimum integer
 * @param a An integer
 * @param b The other integer
 * @return Minimum integer
 */
int min(int a, int b){
    return a<b ? a : b;
}

/**
 * @brief Return maximum integer
 * @param a An integer
 * @param b The other integer
 * @return Maximum integer
 */
int max(int a, int b){
    return a>b ? a : b;
}


