#include "loop_analysis_ast/Loop.h"





LoopAnalysis::LoopAnalysis(const std::string& file)
    : filename(file) {}

void LoopAnalysis::readFile() {}

void LoopAnalysis::getCacheInfo() {
    auto provider = createCacheProvider();
    if (provider) {
        cacheInfo = provider->getCacheInfo();
    } else {
        std::cout << "error: unsupported OS\n";
        std::exit(1);
    }
}

class ASTVisitor : public clang::RecursiveASTVisitor<ASTVisitor> {
public:
    std::vector<LoopInfo>& loops;
    std::unordered_map<std::string, StructInfo>& structs;
    clang::ASTContext* context = nullptr;

    int loopDepth = 0;

    ASTVisitor(std::vector<LoopInfo>& l,
               std::unordered_map<std::string, StructInfo>& s)
        : loops(l), structs(s) {}

    void setContext(clang::ASTContext& ctx) {
        context = &ctx;
    }

    bool VisitRecordDecl(clang::RecordDecl* decl) {
        if (!decl->isStruct()) return true;

        std::string name = decl->getNameAsString();
        if (name.empty()) return true;

        StructInfo& s = structs[name];
        s.name = name;
        s.fields.clear();

        for (auto* field : decl->fields()) {
            StructElt elt;
            elt.name = field->getNameAsString();
            elt.type = field->getType().getAsString();
            elt.size = 8;
            s.fields.push_back(elt);
        }

        return true;
    }

    bool VisitForStmt(clang::ForStmt* stmt) {
        if (!context) return true;

        LoopInfo loop;
        loop.lineNumber =
            context->getSourceManager().getSpellingLineNumber(stmt->getBeginLoc());

        loop.nestingLevel = ++loopDepth;

        collect(stmt->getBody(), loop);

        loops.push_back(loop);
        --loopDepth;

        return true;
    }

    void collect(clang::Stmt* stmt, LoopInfo& loop) {
        if (!stmt) return;

        if (auto* me = clang::dyn_cast<clang::MemberExpr>(stmt)) {
            MemoryAccess acc;
            acc.isStruct = true;
            acc.field = me->getMemberDecl()->getNameAsString();

            clang::Expr* baseExpr = me->getBase()->IgnoreImpCasts();

            if (auto* dre = clang::dyn_cast<clang::DeclRefExpr>(baseExpr)) {
                acc.base = dre->getDecl()->getNameAsString();
            }

            loop.memoryAccesses.push_back(acc);
        }

        for (auto* child : stmt->children()) {
            collect(child, loop);
        }
    }
};

class Consumer : public clang::ASTConsumer {
public:
    ASTVisitor visitor;

    Consumer(std::vector<LoopInfo>& l,
             std::unordered_map<std::string, StructInfo>& s)
        : visitor(l, s) {}

    void HandleTranslationUnit(clang::ASTContext& ctx) override {
        visitor.setContext(ctx);
        visitor.TraverseDecl(ctx.getTranslationUnitDecl());
    }
};

class Action : public clang::ASTFrontendAction {
public:
    std::vector<LoopInfo>& loops;
    std::unordered_map<std::string, StructInfo>& structs;

    Action(std::vector<LoopInfo>& l,
           std::unordered_map<std::string, StructInfo>& s)
        : loops(l), structs(s) {}

    std::unique_ptr<clang::ASTConsumer>
    CreateASTConsumer(clang::CompilerInstance&, clang::StringRef) override {
        return std::make_unique<Consumer>(loops, structs);
    }
};




void LoopAnalysis::analysisDriver() {
    loops.clear();
    structs.clear();


    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "failed to open file\n";
        return;
    }

    std::string code((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());

    auto action = std::make_unique<Action>(loops, structs);

    clang::tooling::runToolOnCodeWithArgs(
        std::move(action),
        code,
        {"-std=c++17"}
    );

    analyzeLoops();
}

void LoopAnalysis::analyzeLoops() {
    suggestions.clear();

    for (const auto& loop : loops) {
        LoopReport r;
        r.accessCount = loop.memoryAccesses.size();

        for (const auto& acc : loop.memoryAccesses) {
            if (acc.isStruct) {
                auto it = structs.find(acc.base);

                if (it != structs.end()) {
                    const StructInfo& s = it->second;
                    r.workingSet += s.fields.size() * 8;
                    r.usefulBytes += 8;
                } else {
                    r.workingSet += 8;
                    r.usefulBytes += 8;
                }
            } else {
                r.workingSet += 8;
                r.usefulBytes += 8;
            }
        }

        r.fitsL1 = r.workingSet <= cacheInfo.l1Size;
        r.fitsL2 = r.workingSet <= cacheInfo.l2Size;
        r.fitsL3 = r.workingSet <= cacheInfo.l3Size;

        double util = (r.accessCount > 0)
            ? r.usefulBytes / (r.accessCount * cacheInfo.cacheLineSize)
            : 0.0;

        std::cout << "\n-------------\n";
        std::cout << "loop at line: " << loop.lineNumber << "\n";
        std::cout << "nesting: " << loop.nestingLevel << "\n";

        if (!r.fitsL1) std::cout << "issue: L1 cache overflow\n";
        if (!r.fitsL2) std::cout << "issue: L2 cache pressure\n";
        if (util < 0.5) std::cout << "issue: poor cache line utilization\n";
    }
}

void LoopAnalysis::generateRecommendations() {
    std::cout << "\n--- optimization suggestions ---\n\n";

    for (const auto& loop : loops) {
        double workingSet = 0.0;
        double usefulBytes = 0.0;
        int accessCount = loop.memoryAccesses.size();

        std::unordered_map<std::string, int> structHits;

        for (const auto& acc : loop.memoryAccesses) {
            if (acc.isStruct) {
                auto it = structs.find(acc.base);

                if (it != structs.end()) {
                    const StructInfo& s = it->second;
                    workingSet += s.fields.size() * 8;
                    usefulBytes += 8;
                    structHits[acc.base]++;
                } else {
                    workingSet += 8;
                    usefulBytes += 8;
                }
            } else {
                workingSet += 8;
                usefulBytes += 8;
            }
        }

        bool fitsL1 = workingSet <= cacheInfo.l1Size;
        bool fitsL2 = workingSet <= cacheInfo.l2Size;

        double util = (accessCount > 0)
            ? usefulBytes / (accessCount * cacheInfo.cacheLineSize)
            : 1.0;

        std::cout << "loop at line " << loop.lineNumber << "\n";

        if (!fitsL1)
            std::cout << "- working set exceeds L1 cache\n";

        if (loop.nestingLevel > 1 && !fitsL2)
            std::cout << "- nested loop exceeds L2 cache\n";

        if (util < 0.5)
            std::cout << "- low cache line utilization\n";

        for (auto& [name, count] : structHits) {
            std::cout << "  " << name << ": " << count << "\n";
        }

        std::cout << "\n";
    }
}

void LoopAnalysis::printOutput() {
    std::cout << "--- Loop Analysis Report ---\n\n";

    for (const auto& s : suggestions) {
        std::cout << "Line " << s.lineNumber << ":\n";
        std::cout << s.issue << "\n";
        std::cout << s.recommendation << "\n\n";
    }
}