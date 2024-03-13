#ifndef DISKDRIVEINFO_HPP
#define DISKDRIVEINFO_HPP

#include <comdef.h>
#include <iomanip> // For std::setw and std::left
#include <iostream>
#include <sstream>
#include <string>
#include <Wbemidl.h>
#include <Windows.h>

class DiskDriveInfo {
public:
    DiskDriveInfo();
    void displayDiskDriveInfo() const;

private:
    std::string diskDriveDetails;

    bool initializeCOM();
    bool queryWMIForDiskDriveInfo();
};

#endif
