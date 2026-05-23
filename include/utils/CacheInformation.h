#ifndef CACHE_HPP
#define CACHE_HPP
#include <cstddef>
#include <memory>
#include <fstream>
#include <iostream>
#include <cctype>

struct CacheInfo {
    size_t l1Size;
    size_t l2Size;
    size_t l3Size;
    size_t cacheLineSize;
};

class CacheProvider {
    public:
        virtual CacheInfo getCacheInfo() = 0;
        virtual ~CacheProvider() = default;
};

class LinuxCacheProvider : public CacheProvider {
    public:
        CacheInfo getCacheInfo() override;

};

class WindowsCacheProvider : public CacheProvider {
    public:
        CacheInfo getCacheInfo() override;
};

class AppleCacheProvider : public CacheProvider {

    public:
        CacheInfo getCacheInfo() override;

};

std::unique_ptr<CacheProvider> createCacheProvider();


size_t parseCacheSize(const std::string & sizeStr);
size_t parseNumber(const std::string & sizeStr);

std::string readFile(const std::string& path);
size_t readSysctlValue(const char* name);
#endif