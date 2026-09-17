#pragma once

#include <algorithm>
#include <utility>
#include "chromosome.hpp"

extern std::mt19937 gen;

InstructionDNA* crossover(InstructionDNA* parent1, InstructionDNA* parent2){
    size_t len1 = parent1->getGenes().size();
    size_t len2 = parent2->getGenes().size();
    size_t minLen = std::min(len1, len2);

    std::uniform_int_distribution<size_t> distribution(0, minLen);
    size_t crossoverPoint = distribution(gen);

    InstructionDNA* child = new InstructionDNA(minLen);

    for(size_t i = 0; i < crossoverPoint; ++i){
        child->setGenes(i, parent1->getGenes()[i]);
    }

    for(size_t i = crossoverPoint; i < minLen; ++i){
        child->setGenes(i, parent2->getGenes()[i]);
    }

    return child;
}

int selectParent(const std::vector<InstructionDNA*>& population){
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

std::pair<InstructionDNA*, InstructionDNA*> selectParents(const std::vector<InstructionDNA*>& population){
    int parent1Index = selectParent(population);
    int parent2Index = selectParent(population);

    // Ensure that the two parents are not the same individual
    for(int attempt = 0; parent2Index == parent1Index && attempt < 20; ++attempt){
        parent2Index = selectParent(population);
    }
    if(parent2Index == parent1Index){
        parent2Index = (parent1Index + 1) % population.size();
    }

    return {population[parent1Index], population[parent2Index]};
}

std::pair<InstructionDNA*, InstructionDNA*> crossoverParents(InstructionDNA* parent1, InstructionDNA* parent2){
    InstructionDNA* child1 = crossover(parent1, parent2);
    InstructionDNA* child2 = crossover(parent2, parent1);
    return {child1, child2};
}

std::pair<InstructionDNA*, InstructionDNA*> crossoverParentsWithMutation(InstructionDNA* parent1, InstructionDNA* parent2, float mutationRate){
    auto [child1, child2] = crossoverParents(parent1, parent2);

    std::uniform_real_distribution<float> mutationDistribution(0.0f, 1.0f);
    std::uniform_int_distribution<uint8_t> geneDistribution(0, 255);

    for(size_t i = 0; i < child1->getGenes().size(); ++i){
        if(mutationDistribution(gen) < mutationRate){
            child1->setGenes(i, geneDistribution(gen));
        }
    }

    for(size_t i = 0; i < child2->getGenes().size(); ++i){
        if(mutationDistribution(gen) < mutationRate){
            child2->setGenes(i, geneDistribution(gen));
        }
    }

    return {child1, child2};
}

std::pair<InstructionDNA*, InstructionDNA*> crossoverParentsWithMutationAndLength(InstructionDNA* parent1, InstructionDNA* parent2, float mutationRate){
    auto [child1, child2] = crossoverParents(parent1, parent2);

    std::uniform_real_distribution<float> mutationDistribution(0.0f, 1.0f);
    std::uniform_int_distribution<uint8_t> geneDistribution(0, 255);
    std::uniform_int_distribution<size_t> lengthDistribution(1, 15); // Assuming max length is 15

    // Mutate child1
    for(size_t i = 0; i < child1->getGenes().size(); ++i){
        if(mutationDistribution(gen) < mutationRate){
            child1->setGenes(i, geneDistribution(gen));
        }
    }

    // Mutate child2
    for(size_t i = 0; i < child2->getGenes().size(); ++i){
        if(mutationDistribution(gen) < mutationRate){
            child2->setGenes(i, geneDistribution(gen));
        }
    }

    // Randomly change the length of the children
    size_t newLength1 = lengthDistribution(gen);
    size_t newLength2 = lengthDistribution(gen);

    if(newLength1 != child1->getGenes().size()){
        child1->getGenes().resize(newLength1);
        for(size_t i = 0; i < newLength1; ++i){
            if(i >= child1->getGenes().size()){
                child1->setGenes(i, geneDistribution(gen));
            }
        }
    }

    if(newLength2 != child2->getGenes().size()){
        child2->getGenes().resize(newLength2);
        for(size_t i = 0; i < newLength2; ++i){
            if(i >= child2->getGenes().size()){
                child2->setGenes(i, geneDistribution(gen));
            }
        }
    }

    return {child1, child2};
}

std::pair<InstructionDNA*, InstructionDNA*> elitism(const std::vector<InstructionDNA*>& population){
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

void mutate(InstructionDNA* individual, float mutationRate){
    std::uniform_real_distribution<float> mutationDistribution(0.0f, 1.0f);
    std::uniform_int_distribution<uint8_t> geneDistribution(0, 255);

    for(size_t i = 0; i < individual->getGenes().size(); ++i){
        if(mutationDistribution(gen) < mutationRate){
            individual->setGenes(i, geneDistribution(gen));
        }
    }
}

void printIndividual(const InstructionDNA& individual) {
    for (uint8_t byte : individual.getGenes()) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(byte) << ' ';
    }
    std::cout << std::dec << "(fitness " << individual.getFitness() << ")\n";
}