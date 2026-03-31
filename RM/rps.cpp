#include "rps.h"


RegretMatchingRPS::RegretMatchingRPS(){
    cumulativeRegret.resize(NUM_ACTIONS, 0);
    cumulativeStrategy.resize(NUM_ACTIONS, 0);
}
RegretMatchingRPS::RegretMatchingRPS(std::vector<double> seed){
    cumulativeRegret.resize(NUM_ACTIONS, 0);
    cumulativeStrategy.resize(NUM_ACTIONS, 0);
    for(int i = 0; i < NUM_ACTIONS; i++){
        cumulativeRegret[i] = seed[i];
    }
}
int RegretMatchingRPS::getUtil(int a, int b){
    if(a == b) return 0; // neither player wins
    if((a == ROCK && b == SCISSORS) || (a == PAPER && b == ROCK) || (a == SCISSORS && b == PAPER)) return 1; // a wins
    return -1; // b wins
}

std::vector<double> RegretMatchingRPS::getStrategy(){
    std::vector<double> strategy(NUM_ACTIONS, 0.0);
    double sum = 0;
    for(int i = 0; i < NUM_ACTIONS; i++){
        strategy[i] = (cumulativeRegret[i] > 0)? cumulativeRegret[i] : 0;
        sum += strategy[i];
    }
    for(int i = 0; i < NUM_ACTIONS; i++){
        if(sum > 0){ // normalize the regret sums, to be used as probability distributions
            strategy[i] /= sum;
        }
        else{ // or if they are all zero use a default strategy
            strategy[i] = 1 / NUM_ACTIONS;
        }
        cumulativeStrategy[i] += strategy[i];
    }
    return strategy;
}

void RegretMatchingRPS::updateRegrets(RegretMatchingRPS& b){ // implementation details would need to be changed here for an asymetric game
    std::vector<double> strategyA = this->getStrategy(), strategyB = b.getStrategy();
    double expectedUtilityA = 0, expectedUtilityB = 0;
    for(int i = 0; i < NUM_ACTIONS; i++){ // 
        for(int j = 0; j < NUM_ACTIONS; j++){
            int util = getUtil(i, j);
            expectedUtilityA += strategyA[i] * util * strategyB[j];
            expectedUtilityB += strategyB[i] * util * strategyA[j];
        }
    }
    for(int i = 0; i < NUM_ACTIONS; i++) { // for each action i, get the regret for having played this action rather than the current strategy
        double actionUtilityA = 0.0;
        double actionUtilityB = 0.0;
        
        for(int j = 0; j < NUM_ACTIONS; j++) {
            double util = getUtil(i, j);
            actionUtilityA += util * strategyB[j];
            actionUtilityB += util * strategyA[j];
        }
        
        this->cumulativeRegret[i] += actionUtilityA - expectedUtilityA;
        b.cumulativeRegret[i] += actionUtilityB - expectedUtilityB;
    }
}

void RegretMatchingRPS::updateRegretsSampling(RegretMatchingRPS& b){
    return;
}

std::vector<double> RegretMatchingRPS::getAverageStrategy(){
    std::vector<double> strategy(NUM_ACTIONS, 0.0);
    double sum = 0;
    for(int i = 0; i < NUM_ACTIONS; i++){
        sum += cumulativeStrategy[i];
    }
    for(int i = 0; i < NUM_ACTIONS; i++){
        if(sum > 0){ // normalize the regret sums, to be used as probability distributions
            strategy[i] = cumulativeStrategy[i] / sum;
        }
        else{ // or if they are all  <= zero use a default strategy
            strategy[i] = 1.0 / NUM_ACTIONS;
        }
    }
    averageStrategies.push_back(strategy);
    return strategy;
}
void RegretMatchingRPS::output(){
    std::vector<double> strategy = getAverageStrategy();
    std::cout<<"R: "<<strategy[ROCK]<<", P: "<<strategy[PAPER]<<", S: "<<strategy[SCISSORS]<<std::endl;
}
void RegretMatchingRPS::runIterations(int n, RegretMatchingRPS &b){
    for(int i = 0; i < n; i++){
        updateRegrets(b);
        b.getAverageStrategy();
        this->getAverageStrategy();
    }
}
void RegretMatchingRPS::Export(const std::string& filename){
    std::ofstream file(filename);
    file << "ROCK,PAPER,SCISSORS\n";
    for (size_t i = 0; i < averageStrategies.size(); i++) {
        file<<averageStrategies[i][0]<<","<<averageStrategies[i][1]<<","<<averageStrategies[i][2]<<"\n";
    }
    file.close();
}
int main(){
    std::vector<double> seedA, seedB;
    seedA.push_back(0.2); seedA.push_back(0.5); seedA.push_back(0.3);
    seedB.push_back(0.3); seedB.push_back(0.3); seedB.push_back(0.4);
    RegretMatchingRPS a(seedA), b(seedB);
    a.runIterations(100000, b);
    a.Export("rpsA.csv");
    b.Export("rpsB.csv");
}