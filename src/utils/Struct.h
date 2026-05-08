#ifndef STRUCT_HPP
#define STRUCT_HPP


#include <string>



struct StructElt{
    std::string name;
    std::string type;
    int size;
};

struct StructEltCompare{
    bool operator()(const StructElt & structElt1, const StructElt & structElt2) const;
};

#endif