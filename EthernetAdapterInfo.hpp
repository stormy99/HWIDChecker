#ifndef ETHERNETADAPTERINFO_HPP
#define ETHERNETADAPTERINFO_HPP

#include <comdef.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <Wbemidl.h>
#include <Windows.h>

class EthernetAdapterInfo {
public:
    EthernetAdapterInfo();
    void displayEthernetAdapterInfo() const;

private:
    std::string adapterDetails;

    bool initializeCOM();
    bool queryWMIForEthernetAdapterInfo();
};

#endif

