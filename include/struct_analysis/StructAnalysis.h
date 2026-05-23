#ifndef STRUCT_ANALYSIS_HPP
#define STRUCT_ANALYSIS_HPP 

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
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










#endif