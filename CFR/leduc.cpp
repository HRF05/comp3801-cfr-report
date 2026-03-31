#include "leduc.h"

LeducState::LeducState() : player(0), round(0), terminal(false), cards(3, -1), bets(2, 1), numRaises(0) {}

bool LeducState::isTerminal() const{
    return terminal;
}

std::string LeducState::getInformationSet(int p) const{
    std::string I = "";
    I += std::to_string(cards[p]);
    if(cards[2] != -1) I += "," + std::to_string(cards[2]);
    I += "[" + std::to_string(round) + "]";
    I += std::to_string(bets[0]) + "," + std::to_string(bets[1]);
    return I;
}

double LeducState::getUtil(int p) const{
    if(!terminal) return 0;

    if(bets[p] > bets[1-p]) return  bets[1-p]; // opposing player folded
    if(bets[1-p] > bets[p]) return -bets[p]; // current player folded

    int winner = getWinner(); // players reveal their hands and better hand wins
    if(winner == -1) return 0; // draw
    if(winner == p) return bets[0]; // p0 win
    return -bets[0]; // p1 win
}

int LeducState::getWinner() const{
    int hand0 = getHandStrength(0);
    int hand1 = getHandStrength(1);
    if(hand0 > hand1) return 0; // p0 win
    if(hand1 > hand0) return 1; // p1 win
    return -1; // draw
}

int LeducState::getHandStrength(int p) const{
    int personal = cards[p], community = cards[2];
    
    if(personal == community) return (personal + 100); // pairs beat non pairs
    return personal;
}

std::vector<Action> LeducState::getActions() const{
    std::vector<Action> actions;
    bool canRaise = (numRaises < 2); // standard leduc limits to 2 raises per round

    if(bets[player] < bets[1-player]){ // facing a bet
        actions.push_back(FOLD);
        actions.push_back(CALL);
        if(canRaise) actions.push_back(RAISE);
    }
    else{
        actions.push_back(CALL); // check
        if(canRaise) actions.push_back(RAISE); // bet
    }
    return actions;
}

LeducState LeducState::takeAction(Action action) const{
    LeducState next = *this;
    next.player = 1-player;
    int betSize = (round == 0) ? 2 : 4;

    if(action == FOLD){
        next.terminal = true;
        next.infoset += "F"; // append history for CFR
    }
    else if(action == CALL){
        next.infoset += "C"; // append history for CFR
        
        if(bets[player] == bets[1-player]){ // check
            if(player == 1){
                // the round ends.
                if(round == 0){
                    next.round = 1;
                    next.player = 0;
                    next.numRaises = 0; // Reset raises for Round 1
                } else {
                    next.terminal = true;
                }
            }
        }
        else{ // call
            next.bets[player] = bets[1-player];

            if(round == 0){
                next.round = 1;
                next.player = 0;
                next.numRaises = 0; // Reset raises for Round 1
            }
            else{
                next.terminal = true;
            }
        }
    }
    else if(action == RAISE){
        next.infoset += "R"; // append history for CFR
        
        // Match the opponents bet, then add the raise amount
        next.bets[player] = bets[1-player] + betSize;
        next.numRaises++;
    }

    return next;
}