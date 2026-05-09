#ifndef STRUCT_HPP
#define STRUCT_HPP


#include <string>
#include <vector>


struct StructElt{
    std::string name;
    std::string type;
    int size;
};

struct StructEltCompare{
    bool operator()(const StructElt & structElt1, const StructElt & structElt2) const;
};

struct StructInfo {
    std::string name;
    size_t size;
    size_t alignment;
    std::vector<StructElt> fields;
};





#endif