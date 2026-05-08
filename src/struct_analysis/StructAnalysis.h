#ifndef STRUCT_ANALYSIS_HPP
#define STRUCT_ANALYSIS_HPP 

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "../utils/Struct.h"


// ----------------------------------------------------
// StructAnalysis

class StructAnalysis {
    private:
        std::vector<StructElt> structElts;
        std::vector<StructElt> newStructElts;
        int oldSize;
        int newStructSize;
        void calcNewStruct();
        static void printLine();

    public:
        void readInput();
        void analyzeDriver();
        int calcStructSize(const std::vector<StructElt>& vec) const;
        int calcStructSize() const;
        void printOutput() const;
};








// map for type sizes
static const std::unordered_map<std::string, int> TYPE_SIZE_MAP = {
    {"char", 1},
    {"signed char", 1},
    {"unsigned char", 1},

    {"short", 2},
    {"short int", 2},
    {"unsigned short", 2},

    {"int", 4},
    {"signed int", 4},
    {"unsigned int", 4},

    {"long", 8},
    {"long int", 8},
    {"unsigned long", 8},

    {"long long", 8},
    {"long long int", 8},
    {"unsigned long long", 8},

    {"float", 4},
    {"double", 8},
    {"long double", 16},

    {"bool", 1}
};

#endif