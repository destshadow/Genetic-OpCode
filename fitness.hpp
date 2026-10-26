#pragma once

#include <vector>
#include "chromosome.hpp"

//the genes are instructions in byte format. We need to calculate the fitness of each individual based on an alphabet ? " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.," to decide
inline float calculateFitness(InstructionDNA* individual){
    // Example fitness calculation: sum of gene values
    float fitness = 0.0f;
    for(const auto& gene : individual->getGenes()){
        fitness += static_cast<float>(gene);
    }
    return fitness;
}

inline void evaluatePopulationFitness(std::vector<InstructionDNA*>& population){
    for(auto& individual : population){
        float fitness = calculateFitness(individual);
        individual->setFitness(fitness);
    }
}
