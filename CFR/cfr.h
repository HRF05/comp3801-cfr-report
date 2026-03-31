#include "leduc.h"
#include <map>
#include <vector>
#include <string>

// Global constant for action dimensions
const int NUM_ACTIONS = 3;

class CFR {
private:
    // Mappings from Information Set strings to their respective regret and strategy arrays
    std::map<std::string, std::vector<double>> cumulativeRegret;
    std::map<std::string, std::vector<double>> cumulativeStrategy;

public:
    CFR();

    // Retrieves current strategy profile via Regret Matching
    std::vector<double> getStrategy(const std::string &infoset, int numActions);

    // Core recursive CFR algorithm
    double cfr(LeducState& state, int updatePlayer, double p0, double p1);

    // Entry point for model training
    void train(int iterations, const std::string& csvFilename);

    // Debugging and evaluation
    void printStrategy();
};