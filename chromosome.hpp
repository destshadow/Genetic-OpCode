#pragma once

#include <vector>
#include <cstdint>
#include <random>

class InstructionDNA{

    std::vector<uint8_t> genes;
    float fitness;

public:
    InstructionDNA( std::mt19937& dis, bool randomize = true){
        if(randomize){
            std::uniform_int_distribution<int> distribution(1, 15);
            size_t len = distribution(dis);
            genes.resize(len);

            std::uniform_int_distribution<int> bytes_distribution(0, 255);
            for(size_t i = 0; i < len; ++i){
                genes[i] = static_cast<uint8_t>(bytes_distribution(dis));
            }

        }
        fitness = 0.0f;
    }

    InstructionDNA(size_t len){
        genes.resize(len);
        fitness = 0.0f; //to check
    }

    const std::vector<uint8_t>& getGenes() const {
        return genes;
    }

    std::vector<uint8_t>& getGenes() { return genes; }

    void setGenes(size_t index, uint8_t value) {
        if(index < genes.size()){
            genes[index] = value;
        }
    }

    float getFitness() const{
        return fitness;
    }

    void setFitness(float fitness){
        this->fitness = fitness;
    }
};
