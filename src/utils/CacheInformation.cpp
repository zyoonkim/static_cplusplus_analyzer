#include "utils/CacheInformation.h"
#include <sys/sysctl.h>

std::unique_ptr<CacheProvider> createCacheProvider() {
    #ifdef __linux__
        return std::make_unique<LinuxCacheProvider>();
    #elif _WIN32
        return std::make_unique<WindowsCacheProvider>();
    #elif __APPLE__
        return std::make_unique<AppleCacheProvider>();
    #else
        return nullptr;
    #endif
}


CacheInfo LinuxCacheProvider::getCacheInfo() {
    CacheInfo info {};

    constexpr const char* basePath = "/sys/devices/system/cpu/cpu0/cache/";

    info.l1Size = parseCacheSize(readFile(std::string(basePath) + "index0/size"));
    info.l2Size = parseCacheSize(readFile(std::string(basePath) + "index2/size"));
    info.l3Size = parseCacheSize(readFile(std::string(basePath) + "index3/size"));
    info.cacheLineSize = parseNumber(readFile(std::string(basePath) + "index0/coherency_line_size"));

    if (info.cacheLineSize == 0) {
        info.cacheLineSize = 64;
    }
    return info;
}

// CacheInfo WindowsCacheProvider::getCacheInfo() {
    
// }

CacheInfo AppleCacheProvider::getCacheInfo() {
    CacheInfo info {};

    info.l1Size = readSysctlValue("hw.l1dcachesize");
    info.l2Size = readSysctlValue("hw.l2cachesize");
    info.l3Size = readSysctlValue("hw.l3cachesize");
    info.cacheLineSize = readSysctlValue("hw.cachelinesize");

    if (info.cacheLineSize == 0) {
        info.cacheLineSize = 64;
    }

    return info;
}



size_t parseCacheSize(const std::string & sizeStr){
    if (sizeStr.empty()) {
        return 0;
    }

    size_t number = 0;
    size_t i = 0;

    while (i < sizeStr.size() && std::isdigit(sizeStr[i])) {
        number = number * 10 + (sizeStr[i] - '0');
        ++i;
    }
    if (i >= sizeStr.size()) {
        return number;
    }
    char suffix = sizeStr[i];

    switch (suffix) {
        case 'K':
        case 'k':
            return number * 1024;
        case 'M':
        case 'm':
            return number * 1024 * 1024;
        case 'G':
        case 'g':
            return number * 1024 * 1024 * 1024;
        default:
            return number;
    }
}

size_t parseNumber(const std::string & sizeStr){
    if (sizeStr.empty()) {
        return 0;
    }

    return static_cast<size_t>(std::stoull(sizeStr));
}

std::string readFile(const std::string& path){
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }

    std::string value;
    std::getline(file, value);
    return value;
}



size_t readSysctlValue(const char* name) {

    size_t value = 0;
    size_t size = sizeof(value);
    if (sysctlbyname(name, &value, &size, nullptr, 0) != 0) {
        return 0;
    }
    return value;

}