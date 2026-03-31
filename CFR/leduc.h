#ifndef LEDUC_H
#define LEDUC_H

#include <vector>
#include <string>

enum Action {
    FOLD = 0,
    CALL = 1,
    RAISE = 2
};

class LeducState {
public:
    int player;
    int round;
    bool terminal;
    std::vector<int> cards;
    std::vector<int> bets;
    std::string infoset; // tracks action history
    int numRaises;

    LeducState();

    // state evaluation
    bool isTerminal() const;
    std::string getInformationSet(int p) const;
    double getUtil(int p) const;
    int getWinner() const;
    int getHandStrength(int p) const;

    // action execution
    std::vector<Action> getActions() const;
    LeducState takeAction(Action action) const;
};

#endif