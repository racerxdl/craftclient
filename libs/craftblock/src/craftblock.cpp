#include <craftblock.h>

using namespace CraftBlock;

#ifndef CRAFTBLOCK_VERSION
#   define CRAFTBLOCK_VERSION "nightly"
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

std::string CraftBlock::CraftBlockVersion() {
    return std::string(TOSTRING(CRAFTBLOCK_VERSION));
}

void CraftBlock::initialize() {
    CraftBlock::initializeGeometries();
}