/*
 * counter.cpp
 *
 *  Created on: Oct 26, 2021
 *      Author: bio
 */

#include "counter.h"

unsigned find_node(vector<unsigned>a,unsigned node)
{
	for(unsigned i=0;i<a.size();i++)
	{
		if(a[i]==node)
		{
			return 1;
		}
	}
	return 0;
}
void UpdateCounters(pair<unsigned,unsigned> edge)
{

}

double computeLAPE() {

    std::string algorithmOutputFile = "/data1/zhuoxh/local-RFES-I.txt";
    std::string groundTruthFile = "/data1/zhuoxh/local-youtube-u-growth.txt";

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

