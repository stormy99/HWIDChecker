#ifndef MOBOINFO_HPP
#define MOBOINFO_HPP

#include <comdef.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <Wbemidl.h>
#include <Windows.h>

class MOBOInfo {
public:
    MOBOInfo();
    void displayMOBOInfo() const; // Display motherboard information

private:
    std::string moboInfo; // Stores motherboard information

    bool initializeCOM();
    bool queryWMIForMOBOInfo();
};

#endif
