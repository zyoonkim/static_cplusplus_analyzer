# C/C++ Code Analysis Suite
A collection of static analysis tools written in C++ using LLVM and Clang libraries.
Currently included:
- loop-analyzer
  - Uses the Clang AST to analyze loops in C/C++ source code.
  - Intended for detecting performance-related patterns and collecting loop information.
- struct-analyzer
  - Analyzes struct layouts, alignment, and padding behavior.
  - Useful for understanding memory layout and cache efficiency.
## Project Structure
```text
.
├── build
├── include
│   ├── loop_analysis_ast
│   ├── struct_analysis
│   └── utils
├── src
│   ├── loop_analysis_ast
│   ├── struct_analysis
│   └── utils
└── tests
```
## Requirements

* LLVM/Clang
* C++17 compiler
* GNU Make

Example installation using Homebrew on macOS:
```bash
brew install llvm
```
### Building

Build all tools:
```bash
cd build
make
```
Generated binaries will appear in:
```text
build/bin
```
## Binaries

### loop-analyzer

Analyzes loops using the Clang AST frontend.

Example:
```bash
./bin/loop-analyzer
```
### struct-analyzer

Analyzes struct memory layout and alignment.

Example:
```bash
./bin/struct-analyzer
```
### Cleaning

### Remove build artifacts:
```bash
make clean
```
## Notes

* The tests/ directory is used locally for development and is not included in the repository.
* LLVM and Clang headers/libraries are required for AST-based analysis.
* The project currently targets macOS with Homebrew LLVM installed at:
```text
/opt/homebrew/opt/llvm
```
