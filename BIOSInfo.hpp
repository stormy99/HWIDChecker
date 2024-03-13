#ifndef BIOSINFO_HPP
#define BIOSINFO_HPP

#include <comdef.h>
#include <iostream>
#include <string>
#include <Wbemidl.h>
#include <Windows.h>

class BIOSInfo {
public:
    BIOSInfo();
    void displayBIOSInfo() const;

private:
    std::string biosUUID;
    bool queryBIOSUUID();
};

#endif

