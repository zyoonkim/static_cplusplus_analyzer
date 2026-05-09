#include "StructAnalysis.h"
#include "../utils/TypeMap.h"


// ------------------------------------

// read current struct
void StructAnalysis::readInput() {
    structElts.clear();

    std::string line;

    while (std::getline(std::cin, line)) {

        if (line.empty()) continue;

        if (line.find("struct") != std::string::npos) continue;
        if (line.find("{") != std::string::npos) continue;
        if (line.find("}") != std::string::npos) continue;
        if (line.find("//") != std::string::npos) continue;

        std::string type, name;
        std::stringstream ss(line);

        ss >> type >> name;

        if (type.empty() || name.empty()) continue;
        if (!name.empty() && name.back() == ';') {
            name.pop_back();
        }

        int size = 0;
        auto it = TYPE_SIZE_MAP.find(type);

        if (it != TYPE_SIZE_MAP.end()) {
            size = it->second;
        } else if (type.find("*") != std::string::npos) {
            size = 8;
        } else {
            size = 8;
        }
        StructElt elt;
        elt.name = name;
        elt.size = size;
        elt.type = type;
        structElts.push_back(elt);
    }
}



// driver for analysis
void StructAnalysis::analyzeDriver() {
    oldSize = calcStructSize(structElts);
    calcNewStruct();
    printOutput();
}


// calculate the size of the current struct
int StructAnalysis::calcStructSize(const std::vector<StructElt> & vec) const {

    int offset = 0;
    int structAlignment = 1;

    for (const auto &elt : vec) {
        int alignment = elt.size;
        if (alignment > structAlignment) {
            structAlignment = alignment;
        }
        if (offset % alignment != 0) {
            offset += alignment - (offset % alignment);
        }
        offset += elt.size;
    }

    if (offset % structAlignment != 0) {
        offset += structAlignment - (offset % structAlignment);
    }
    return offset;

}

int StructAnalysis::calcStructSize() const {
    int offset = 0;
    int structAlignment = 1;

    for (const auto &elt : structElts) {
        int alignment = elt.size;
        if (alignment > structAlignment) {
            structAlignment = alignment;
        }
        if (offset % alignment != 0) {
            offset += alignment - (offset % alignment);
        }
        offset += elt.size;
    }

    if (offset % structAlignment != 0) {
        offset += structAlignment - (offset % structAlignment);
    }
    return offset; 
}


// determine the best size struct and modify class variables
void StructAnalysis::calcNewStruct() {
    newStructElts = structElts;
    std::sort(newStructElts.begin(), newStructElts.end(), StructEltCompare());
    newStructSize = calcStructSize(newStructElts);
}


void StructAnalysis::printLine() {
    std::cout << "------------------------" << '\n';
}


void StructAnalysis::printOutput() const {
    std::cout << "Original Struct:" << '\n';

    for (const auto &elt : structElts) {
        std::cout << elt.type << " " << elt.name << " " << elt.size << '\n';
    }
    std::cout << "Total size: " << oldSize << '\n';
    std::cout << '\n';
    std::cout << "Optimized Struct:" << '\n';

    for (const auto &elt : newStructElts) {
        std::cout << elt.type << " " <<  elt.name << " " << elt.size << '\n';
    }
    std::cout << "Total size: " << newStructSize << "\n";
    printLine();
    std::cout << "\n";
}