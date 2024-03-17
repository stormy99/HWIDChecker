#ifndef RAMINFO_HPP
#define RAMINFO_HPP

#include <comdef.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <Wbemidl.h>
#include <Windows.h>

class RAMInfo {
public:
    RAMInfo();
    void displayRAMInfo() const;

private:
    std::string ramDetails;

    bool initializeCOM();
    bool queryWMIForRAMInfo();
};

#endif
