CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
TARGET := genetic_opcode
HEADERS := chromosome.hpp fitness.hpp geneticAlgo.hpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) main.cpp -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(TARGET)
