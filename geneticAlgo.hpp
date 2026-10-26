#pragma once

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <utility>
#include <vector>
#include "chromosome.hpp"

extern std::mt19937 gen;

// Cut-and-splice crossover: head of parent1 + tail of parent2, with independent cut points,
// so the child can be longer or shorter than both parents (between 1 and MAX_INSTRUCTION_LENGTH)
inline InstructionDNA* crossover(InstructionDNA* parent1, InstructionDNA* parent2){
    const std::vector<uint8_t>& genes1 = parent1->getGenes();
    const std::vector<uint8_t>& genes2 = parent2->getGenes();

    std::uniform_int_distribution<size_t> distribution1(1, genes1.size());
    std::uniform_int_distribution<size_t> distribution2(0, genes2.size());
    size_t crossoverPoint1 = distribution1(gen);
    size_t crossoverPoint2 = distribution2(gen);

    InstructionDNA* child = new InstructionDNA(0);
    std::vector<uint8_t>& childGenes = child->getGenes();

    childGenes.insert(childGenes.end(), genes1.begin(), genes1.begin() + crossoverPoint1);
    childGenes.insert(childGenes.end(), genes2.begin() + crossoverPoint2, genes2.end());

    if(childGenes.size() > MAX_INSTRUCTION_LENGTH){
        childGenes.resize(MAX_INSTRUCTION_LENGTH);
    }

    return child;
}

inline size_t selectParent(const std::vector<InstructionDNA*>& population){
    float totalFitness = 0.0f;
    for(const auto& individual : population){
        totalFitness += individual->getFitness();
    }

    std::uniform_real_distribution<float> distribution(0.0f, totalFitness);
    float randomValue = distribution(gen);

    float cumulativeFitness = 0.0f;
    for(size_t i = 0; i < population.size(); ++i){
        cumulativeFitness += population[i]->getFitness();
        if(cumulativeFitness >= randomValue){
            return i;
        }
    }

    return population.size() - 1; // Fallback in case of rounding errors
}

inline std::pair<InstructionDNA*, InstructionDNA*> selectParents(const std::vector<InstructionDNA*>& population){
    size_t parent1Index = selectParent(population);
    size_t parent2Index = selectParent(population);

    // Ensure that the two parents are not the same individual
    for(int attempt = 0; parent2Index == parent1Index && attempt < 20; ++attempt){
        parent2Index = selectParent(population);
    }
    if(parent2Index == parent1Index){
        parent2Index = (parent1Index + 1) % population.size();
    }

    return {population[parent1Index], population[parent2Index]};
}

inline std::pair<InstructionDNA*, InstructionDNA*> crossoverParents(InstructionDNA* parent1, InstructionDNA* parent2){
    InstructionDNA* child1 = crossover(parent1, parent2);
    InstructionDNA* child2 = crossover(parent2, parent1);
    return {child1, child2};
}

inline void mutate(InstructionDNA* individual, float mutationRate){
    std::uniform_real_distribution<float> mutationDistribution(0.0f, 1.0f);
    // uniform_int_distribution does not allow 8-bit types: draw an int and cast it
    std::uniform_int_distribution<int> geneDistribution(0, 255);

    for(size_t i = 0; i < individual->getGenes().size(); ++i){
        if(mutationDistribution(gen) < mutationRate){
            individual->setGenes(i, static_cast<uint8_t>(geneDistribution(gen)));
        }
    }
}

// Changes the length to a random value in [1, MAX_INSTRUCTION_LENGTH]; new bytes are random
inline void mutateLength(InstructionDNA* individual){
    std::uniform_int_distribution<size_t> lengthDistribution(1, MAX_INSTRUCTION_LENGTH);
    std::uniform_int_distribution<int> geneDistribution(0, 255);

    size_t oldLength = individual->getGenes().size();
    size_t newLength = lengthDistribution(gen);

    individual->getGenes().resize(newLength);
    for(size_t i = oldLength; i < newLength; ++i){
        individual->setGenes(i, static_cast<uint8_t>(geneDistribution(gen)));
    }
}

inline std::pair<InstructionDNA*, InstructionDNA*> crossoverParentsWithMutation(InstructionDNA* parent1, InstructionDNA* parent2, float mutationRate){
    auto [child1, child2] = crossoverParents(parent1, parent2);

    mutate(child1, mutationRate);
    mutate(child2, mutationRate);

    return {child1, child2};
}

inline std::pair<InstructionDNA*, InstructionDNA*> crossoverParentsWithMutationAndLength(InstructionDNA* parent1, InstructionDNA* parent2, float mutationRate){
    auto [child1, child2] = crossoverParentsWithMutation(parent1, parent2, mutationRate);

    mutateLength(child1);
    mutateLength(child2);

    return {child1, child2};
}

inline std::pair<InstructionDNA*, InstructionDNA*> elitism(const std::vector<InstructionDNA*>& population){
    if(population.size() < 2){
        return {nullptr, nullptr}; // Not enough individuals for elitism
    }

    size_t bestIndex = 0;
    size_t secondBestIndex = 1;

    if(population[1]->getFitness() > population[0]->getFitness()){
        bestIndex = 1;
        secondBestIndex = 0;
    }

    for(size_t i = 2; i < population.size(); ++i){
        if(population[i]->getFitness() > population[bestIndex]->getFitness()){
            secondBestIndex = bestIndex;
            bestIndex = i;
        } else if(population[i]->getFitness() > population[secondBestIndex]->getFitness()){
            secondBestIndex = i;
        }
    }

    return {population[bestIndex], population[secondBestIndex]};
}

inline void printIndividual(const InstructionDNA& individual) {
    for (uint8_t byte : individual.getGenes()) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(byte) << ' ';
    }
    std::cout << std::dec << "(fitness " << individual.getFitness() << ")\n";
}
