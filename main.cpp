#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include "chromosome.hpp"
#include "fitness.hpp"
#include "geneticAlgo.hpp"

std::random_device rd;
std::mt19937 gen(rd());

const int POP_SIZE = 150;
const float MUTATION_RATE = 0.05f;
const int MAX_GENERATIONS = 5;

int main() {
    std::vector<InstructionDNA*> population;

    for (int i = 0; i < POP_SIZE; ++i) {
        population.push_back(new InstructionDNA(gen));
    }

    for (int generation = 0; generation < MAX_GENERATIONS; ++generation) {
        evaluatePopulationFitness(population);
        auto [best, secondBest] = elitism(population);
        std::cout << "Generazione " << generation << ", migliore: ";
        printIndividual(*best);

        if (generation == MAX_GENERATIONS - 1) break;

        std::vector<InstructionDNA*> nextPopulation;
        nextPopulation.push_back(new InstructionDNA(*best));
        nextPopulation.push_back(new InstructionDNA(*secondBest));

        while (nextPopulation.size() < POP_SIZE) {
            auto [parent1, parent2] = selectParents(population);
            auto [child1, child2] = crossoverParentsWithMutation(
                parent1, parent2, MUTATION_RATE);
            nextPopulation.push_back(child1);
            
            if (nextPopulation.size() < POP_SIZE) {
                nextPopulation.push_back(child2);
            } else {
                delete child2;
            }
        }

        for (InstructionDNA* individual : population) delete individual;
        population = std::move(nextPopulation);
    }

    for (InstructionDNA* individual : population) delete individual;
}
