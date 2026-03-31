#ifndef RPS_H
#define RPS_H

#include <vector>
#include <iostream>
#include <fstream>

static const int ROCK = 0, PAPER = 1, SCISSORS = 2, NUM_ACTIONS = 3;

class RegretMatchingRPS {
    private:
        std::vector<double> cumulativeRegret;
        std::vector<double> cumulativeStrategy;
        std::vector<std::vector<double>> averageStrategies;
    public:
        RegretMatchingRPS();
        RegretMatchingRPS(std::vector<double> seed);
        int getUtil(int a, int b);
        std::vector<double> getStrategy();
        void updateRegrets(RegretMatchingRPS& b);
        void updateRegretsSampling(RegretMatchingRPS& b);
        std::vector<double> getAverageStrategy();
        void runIterations(int n, RegretMatchingRPS &b);
        void output();
        void Export(const std::string& filename);
};

#endif