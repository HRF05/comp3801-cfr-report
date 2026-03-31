#include <iostream>
#include "cfr.h"

int main(){
    CFR solver;
    int iterations = 1000;
    std::string filename = "regret_metrics.csv";

    std::cout<<"Starting" << std::endl;
    std::cout << iterations << " iterations" << std::endl;
    
    solver.train(iterations, filename);
    
    std::cout<<"\nTraining complete\n\n";
    
    solver.printStrategy();

    return 0;
}