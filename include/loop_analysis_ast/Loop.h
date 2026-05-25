#ifndef LOOP_HPP
#define LOOP_HPP

#include <vector>
#include <string>
#include <cstddef>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <iostream>

#include <clang/AST/AST.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Basic/SourceManager.h>

#include "../utils/Struct.h"
#include "../utils/CacheInformation.h"

struct Suggestion {
    int lineNumber;
    std::string issue;
    std::string recommendation;
};

struct MemoryAccess;

struct LoopInfo {
    const clang::ForStmt* forStmt = nullptr;
    const clang::WhileStmt* whileStmt = nullptr;

    unsigned lineNumber = 0;
    unsigned nestingLevel = 0;

    std::vector<MemoryAccess> memoryAccesses;
};

struct MemoryAccess {
    std::string base;
    std::string field;
    bool isStruct = false;
    bool isPointer = false;
};

struct LoopCost {
    double workingSet;
    double cacheLineUtil;
    bool fitsL1;
    bool fitsL2;
    bool fitsL3;
};

struct LoopReport {
    std::unordered_map<std::string, int> structAccessCount;
    double workingSet = 0.0;
    double usefulBytes = 0.0;
    int accessCount = 0;
    bool fitsL1 = false;
    bool fitsL2 = false;
    bool fitsL3 = false;
};

class LoopAnalysis {
private:
    std::string filename;

    std::vector<LoopInfo> loops;
    std::vector<Suggestion> suggestions;

    std::unordered_map<std::string, StructInfo> structs;

    CacheInfo cacheInfo;

    void readFile();
    void runClangAnalysis();
    void analyzeLoops();
    void getCacheInfo();
    void generateRecommendations();

public:
    explicit LoopAnalysis(const std::string& file);

    void analysisDriver();
    void printOutput();
};



class LoopAction : public clang::FrontendAction {
public:
    std::vector<LoopInfo>& loops;
    std::vector<StructInfo>& structs;

    LoopAction(std::vector<LoopInfo>& l,
               std::vector<StructInfo>& s)
        : loops(l), structs(s) {}

    std::unique_ptr<clang::ASTConsumer>
    CreateASTConsumer(clang::CompilerInstance&,
                      clang::StringRef) override {
        return std::make_unique<clang::ASTConsumer>();
    }
};





#endif