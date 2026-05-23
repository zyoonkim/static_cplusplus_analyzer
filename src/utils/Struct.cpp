#include "utils/Struct.h"

bool StructEltCompare::operator()(const StructElt & structElt1, const StructElt & structElt2) const {
    return structElt1.size > structElt2.size;
}