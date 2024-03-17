#ifndef GPUINFO_HPP
#define GPUINFO_HPP

#include <comdef.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <Wbemidl.h>
#include <Windows.h>

class GPUInfo {
public:
    GPUInfo();
    void displayGPUInfo() const;

private:
    std::vector<std::string> gpuDetails;

    bool initializeCOM();
    bool queryWMIForGPUInfo();
};

#endif
