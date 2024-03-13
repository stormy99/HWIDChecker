#ifndef CPUINFO_HPP
#define CPUINFO_HPP

#include <array>
#include <cstring>
#include <intrin.h>
#include <iostream>
#include <string>

class CPUInfo {
public:
    CPUInfo();
    void displayCPUInfo() const;

private:
    std::string CPUBrand;
    void gatherCPUInfo();
};

#endif


