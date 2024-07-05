

#ifndef FURL_H_INCLUDED
#define FURL_H_INCLUDED

///////       Header files        ///////
#include "set"
#include "unordered_set"
#include "iostream"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>
#include <cmath>

namespace std {
    /**
     * @brief Hashing pairs
     */
    template <> struct hash<std::pair<int, int>> {
        inline size_t operator()(const std::pair<int, int> &v) const {
            std::hash<int> int_hasher;
            return int_hasher(v.first) ^ int_hasher(v.second);
        }
    };

    /**
     * @brief Hashing pairs
     */
    template <> struct hash<std::pair<unsigned long long, int>> {
        inline size_t operator()(const std::pair<unsigned long long, int> &v) const {
            std::hash<int> int_hasher;
            return int_hasher(v.first) ^ int_hasher(v.second);
        }
    };
}

using namespace std;

/**
 * @brief Compare function for priority queue
 */
struct compare {
    bool operator()(const pair<pair<int,int>,double> &edge1, const pair<pair<int,int>,double> &edge2){
        return edge1.second < edge2.second;
    }
};

/**
 * @brief This is a class for Furl, a triangle counting algorithm in a multigraph stream
 */
class Furl {
public:
    /**
     * @brief Constructor
     * @param _wgt Flag to select either binary or weighted counting (0: binary, 1: weighted)
     * @param _ext Flag to select either FURL-0 or FURL (0: FURL-0, 1: FURL)
     * @param _buffer_size Size of buffer for reservoir sampling
     * @param _delta Decaying factor for past estimations
     * @param _J Bucket length for estimation update
     */
    Furl(int _wgt, int _ext, int _bucket_size, double _delta, int _J) : wgt(_wgt), ext(_ext) , bucket_size(_bucket_size), delta(_delta), J(_J){
        maxnodeID = -1;
        exactcnt = 1;
        time = 0;
        TM = 0;
    }

    /**
     * @brief Destructor
     */
    ~Furl(){}

    /**
     * @brief Prepare memory spaces for a node
     * @param u Node
     */
    void ready_node(const int u);

    /**
     * @brief Run the algorithm on a new edge
     * @param edge New edge
     * @param edge_rank Hash value of the new edge
     */
    void update(pair<int,int>& edge, double edge_rank);

    /**
     * @brief Sample a new edge through reservoir sampling with random hash
     * @param edge New edge
     * @param edge_rank Hash value of the new edge
     * @return True if the edge is sampled
     */
    bool sample_new_edge(pair<int,int>& edge, double edge_rank);

    /**
     * @brief Update triangle counts
     * @param edge New edge
     * @param sampled Flag to check whether an edge is sampled
     */
    void update_triangles(pair<int,int>& edge, bool sampled);

    /**
     * @brief Add an edge to the sampled graph
     * @param u A node of edge
     * @param v The other node of edge
     */
    void add_edge_sample(const int u, const int v);

    /**
     * @brief Remove an edge from the sampled graph
     * @param u A node of edge
     * @param v The other node of edge
     */
    void remove_edge_sample(const int u, const int v);

    /**
     * @brief Update triangle counts changed by a new edge
     * @param u A node of edge
     * @param v The other node of edge
     * @param weight Weight of the triangle
     */
    void add_triangles(const int u, const int v, double weight);

    /**
     * @brief Combine the current estimation with estimations at earlier timesteps through a weighted average scheme
     * @param delta Decaying factor
     */
    void weighted_average(double delta);

    /**
     * @brief Return triangle counts of FURL-0 for all nodes
     * @return List of triangle counts of FURL-0
     */
    vector<double> get_counts();

    /**
     * @brief Return triangle counts of FURL for all nodes
     * @return List of triangle counts of FURL
     */
    vector<double> get_estimations();

    /**
     * @brief Check whether an edge is in the buffer
     * @param u A node of edge
     * @param v The other node of edge
     * @return True if the sampled graph contains the edge
     */
    bool contain_edge(const int u, const int v){
        if(wgt == 0){
            return find(node_map_[u].begin(),node_map_[u].end(),v) != node_map_[u].end();
        }else if(wgt == 1){
            return occurrences[u].find(v) != occurrences[u].end();
        }else return 0;
    }

    /**
      * @brief Increase occurrence number of an edge
      * @param u A node of edge
      * @param v The other node of edge
      * @param weight Weight of occurrence number
      */
    void increase_occurrences(const int u, const int v, int weight){
        occurrences[u][v] += weight;
        occurrences[v][u] += weight;
    }

    /**
	 * @brief Calculate degree of a node
	 * @param u Node
	 * @return degree of node u
	 */
    int degree(const int u) const {
        if(wgt == 0){
            if (node_map_.find(u) == node_map_.end()) {
                return 0;
            }
            return node_map_.find(u)->second.size();
        }else if(wgt == 1){
            if (occurrences.find(u) == occurrences.end()) {
                return 0;
            }
            return occurrences.find(u)->second.size();
        }else return -1;
    }

    double get_global();

    void outputLocal(const std::vector<double>& res, const std::string& filename);

    double computeLAPE();

    void reset();

    double getCountingTime();
    double getSamplingTime();
    long getDiscoverdTriangles();

private:
    /**
     * @brief Triangles estimations for basic method FURL-0
     */
    unordered_map<int, double> counts;

    /**
     * @brief Triangles estimations for main method FURL
     */
    unordered_map<int, double> estimations;

    /**
     * @brief Sampled graph
     */
    unordered_map<int, vector<int> > node_map_;
    /**
     * @brief The number of duplicate edges for each sampled edge
     */
    unordered_map<int, unordered_map<int, int> > occurrences;

    /**
    * @brief Buffer for reservoir sampling with random hash
    */
    priority_queue<pair<pair<int,int>, double>, vector<pair<pair<int,int>, double> >, compare> buffer;

    /**
     * @brief Maximum node ID
     */
    int maxnodeID;

    /**
    * @brief The first time when local triangle estimations equals the true triangle counts
    */
    int TM;

    /**
    * @brief Flag to select either binary or weighted counting (0: binary, 1: weighted)
    */
    int wgt;

    /**
    * @brief Flag to select either FURL-0 or FURL (0: FURL-0, 1: FURL)
    */
    int ext;

    /**
    * @brief Flag that is set when local triangle estimations equals the true triangle counts
    */
    bool exactcnt;

    /**
	 * @brief Size of buffer for reservoir sampling
	 */
    int bucket_size;

    /**
	 * @brief Decaying factor for past estimations
	 */
    double delta;

    /**
	 * @brief Period length for estimation update
	 */
    int J;

    /**
    * @brief Current time
    */
    int time;

    double global = 0;

    double countingTime = 0;
    clock_t countingTime1;
    clock_t countingTime2;
    double samplingTime = 0;
    clock_t samplingTime1;
    clock_t samplingTime2;

    long discoverd_triangles = 0;
};

#endif // FURL_H_INCLUDED
