#include "CPUInfo.hpp"

CPUInfo::CPUInfo() {
    gatherCPUInfo();
}

void CPUInfo::gatherCPUInfo() {
    std::array<int, 4> cpui;
    std::array<char, 0x40> brand{}; // Ensure zero-initialization
    brand.fill(0); // Explicitly zero-fill the array

    __cpuid(cpui.data(), 0x80000000);
    const unsigned int nExIds = static_cast<unsigned int>(cpui[0]);

    if (nExIds >= 0x80000004) { // Ensure we have support for the extended CPUID information
        for (unsigned int i = 0x80000002; i <= 0x80000004; ++i) {
            __cpuidex(cpui.data(), i, 0);
            // Calculate the correct offset within the 'brand' array
            size_t offset = (i - 0x80000002) * 16;
            // Ensure we do not exceed the 'brand' array bounds
            if (offset < brand.size()) {
                std::memcpy(brand.data() + offset, cpui.data(), sizeof(cpui));
            }
        }
        CPUBrand.assign(brand.data(), brand.size());
    }
    else {
        CPUBrand = "Unsupported CPUID";
    }
}

void CPUInfo::displayCPUInfo() const {
    std::cout << "CPU: " << CPUBrand << "\n\n";
}
