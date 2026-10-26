#pragma once

#include <cassert>
#include <vector>
#include <cstdint>
#include <random>
#include <utility>

// Maximum length of an x86 instruction
constexpr size_t MAX_INSTRUCTION_LENGTH = 15;

// Invariant: a chromosome always holds between 1 and MAX_INSTRUCTION_LENGTH bytes
inline bool isValidLength(size_t len){
    return len >= 1 && len <= MAX_INSTRUCTION_LENGTH;
}

class InstructionDNA{

    std::vector<uint8_t> genes;
    float fitness;

public:
    // Random chromosome
    explicit InstructionDNA(std::mt19937& dis){
        std::uniform_int_distribution<int> distribution(1, MAX_INSTRUCTION_LENGTH);
        size_t len = distribution(dis);
        genes.resize(len);

        std::uniform_int_distribution<int> bytes_distribution(0, 255);
        for(size_t i = 0; i < len; ++i){
            genes[i] = static_cast<uint8_t>(bytes_distribution(dis));
        }

        fitness = 0.0f;
    }

    // Chromosome from given bytes
    explicit InstructionDNA(std::vector<uint8_t> genes) : genes(std::move(genes)){
        assert(isValidLength(this->genes.size()));
        fitness = 0.0f;
    }

    const std::vector<uint8_t>& getGenes() const {
        return genes;
    }

    void setGenes(size_t index, uint8_t value) {
        if(index < genes.size()){
            genes[index] = value;
        }
    }

    // New bytes are set to 0: the caller decides their value
    void resizeGenes(size_t newLength){
        assert(isValidLength(newLength));
        genes.resize(newLength);
    }

    float getFitness() const{
        return fitness;
    }

    void setFitness(float fitness){
        this->fitness = fitness;
    }
};
