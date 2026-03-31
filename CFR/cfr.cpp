#include "cfr.h"
#include <iostream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <vector>
#include <iomanip>

CFR::CFR(){}

std::vector<double> CFR::getStrategy(const std::string &infoset, int numActions){
    std::vector<double> strategy(numActions, 0.0);
    std::vector<double>& regrets = cumulativeRegret[infoset];
    
    // if this is the first time this infoset is seen, initialize regrets
    if(regrets.empty()){
        regrets.resize(numActions, 0.0);
    }

    double normalizingSum = 0.0;
    for(int a = 0; a < numActions; ++a){
        strategy[a] = std::max(0.0, regrets[a]);
        normalizingSum += strategy[a];
    }

    for(int a = 0; a < numActions; ++a){
        if(normalizingSum > 0){
            strategy[a] /= normalizingSum;
        }
        else{
            strategy[a] = 1.0 / numActions; // default to uniform if no positive regrets
        }
    }

    return strategy;
}

double CFR::cfr(LeducState& state, int updatePlayer, double p0, double p1){
    // return utility if terminal node
    if(state.isTerminal()){
        return state.getUtil(updatePlayer);
    }

    int currentPlayer = state.player;
    
    // construct infoset key
    // format: [Card][CommunityCard if round 1][History]
    std::string infoset = std::to_string(state.cards[currentPlayer]);
    
    // in leduc, round 0 is pre-flop (no community card). round 1 is flop (community card)
    if(state.round == 1){
        infoset += std::to_string(state.cards[2]);
    }
    infoset += state.infoset; // add history

    // get valid actions for this state
    std::vector<Action> actions = state.getActions();

    // get current strategy from regret matching
    std::vector<double> strategy = getStrategy(infoset, NUM_ACTIONS);
    
    // filter strategy for valid actions and renormalize
    std::vector<double> activeStrategy(NUM_ACTIONS, 0.0);
    double activeSum = 0.0;
    for(Action a : actions){
        activeStrategy[a] = strategy[a];
        activeSum += activeStrategy[a];
    }

    // if no cumulative regrets use default uniform strategy
    if(activeSum > 0){
        for(Action a : actions){
            activeStrategy[a] /= activeSum;
        }
    } else {
        for(Action a : actions){
            activeStrategy[a] = 1.0 / actions.size();
        }
    }

    std::vector<double> nodeUtil(NUM_ACTIONS);
    double expectedNodeUtil = 0.0;

    // recursively call cfr for each action
    for(Action a : actions){
        LeducState nextState = state.takeAction(a);
        
        if(currentPlayer == 0){
            nodeUtil[a] = cfr(nextState, updatePlayer, p0 * activeStrategy[a], p1);
        } else {
            nodeUtil[a] = cfr(nextState, updatePlayer, p0, p1 * activeStrategy[a]);
        }
        
        expectedNodeUtil += activeStrategy[a] * nodeUtil[a];
    }

    // if this is the player we are updating, update regrets and accumulate strategy
    if(currentPlayer == updatePlayer){
        std::vector<double>& regrets = cumulativeRegret[infoset];
        std::vector<double>& cumStrat = cumulativeStrategy[infoset];
        
        if(regrets.empty()) regrets.resize(NUM_ACTIONS, 0.0);
        if(cumStrat.empty()) cumStrat.resize(NUM_ACTIONS, 0.0);

        double opponentProb = (currentPlayer == 0) ? p1 : p0;
        double myProb = (currentPlayer == 0) ? p0 : p1;

        for(Action a : actions){
            // update regrets: Counterfactual Value - Expected Value
            double regret = nodeUtil[a] - expectedNodeUtil;
            regrets[a] += opponentProb * regret;
            
            // update average strategy (weighted by realization probability)
            cumStrat[a] += myProb * activeStrategy[a];
        }
    }

    return expectedNodeUtil;
}

// main training loop
void CFR::train(int iterations, const std::string& csvFilename){
    // deck: 0,0 (jacks), 1,1 (queens), 2,2 (kings)
    std::vector<int> deck = {0, 0, 1, 1, 2, 2}; 
    int deckSize = deck.size();


    std::ofstream csvFile;
    if (!csvFilename.empty()) {
        csvFile.open(csvFilename);
        csvFile << "Iteration,Total_Infosets,Avg_Cumulative_Regret,Avg_Regret_Per_Iteration\n";
    }


    // vanila cfr is deterministic, in a practical implementation, one would probably want to use monte carlo cfr(sampling instead of pure determinism)
    
    for(int i = 0; i < iterations; i++){
        
        // Loop over Player 0 card index
        for(int c1 = 0; c1 < deckSize; c1++){
            
            // Loop over Player 1 card index
            for(int c2 = 0; c2 < deckSize; c2++){
                if(c1 == c2) continue; // Cannot deal the same physical card
                
                // Loop over Community card index
                for(int c3 = 0; c3 < deckSize; c3++){
                    if(c3 == c1 || c3 == c2) continue; // community card must be unique

                    // construct the specific state for this permutation
                    LeducState root;
                    root.cards = {deck[c1], deck[c2], deck[c3]};
                    
                    // the accumulation of regrets across these loops creates the expected regret
                    
                    // update Player 0
                    cfr(root, 0, 1.0, 1.0);
                    
                    // update Player 1
                    cfr(root, 1, 1.0, 1.0);
                }
            }
        }


        // Outputting metrics, not important for logic
        if (csvFile.is_open()) {
            double totalPositiveRegret = 0.0;
            int infosetCount = cumulativeRegret.size();
            
            for (const auto& kv : cumulativeRegret) {
                for (double r : kv.second) {
                    if (r > 0) totalPositiveRegret += r;
                }
            }
            
            double avgCumRegret = (infosetCount > 0) ? (totalPositiveRegret / infosetCount) : 0.0;
            // divided by (i + 1) to get the average regret with respect to time T
            double avgRegretPerIter = avgCumRegret / (i + 1);

            csvFile << (i + 1) << "," << infosetCount << "," << avgCumRegret << "," << avgRegretPerIter << "\n";
        }
        //



        if(i % 100 == 0 && i > 0){ // progress
             std::cout << "Iteration " << i << " complete." << std::endl;
        }
    }
}

void CFR::printStrategy(){
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Final Strategy (Infoset: Fold/Call/Raise):" << std::endl;

    // use a map to sort output by infoset key for consistent deterministic reading
    std::map<std::string, std::vector<double>> sortedStrategy(cumulativeStrategy.begin(), cumulativeStrategy.end());

    for(const auto& pair : sortedStrategy){
        std::string infoset = pair.first;
        std::vector<double> stratSum = pair.second;
        double sum = 0.0;
        for(double v : stratSum) sum += v;
        
        if(sum > 0){
            std::cout << infoset << ": "; 

            std::cout << "[";
            std::cout << "F:" << (stratSum[0] / sum) << " ";
            std::cout << "C:" << (stratSum[1] / sum) << " ";
            std::cout << "R:" << (stratSum[2] / sum);
            std::cout << "]" << std::endl;
        }
    }
}