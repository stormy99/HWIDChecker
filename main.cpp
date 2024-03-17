#include "main.hpp"

// Global instances of hardware information classes
BIOSInfo biosInfo;
CPUInfo cpuInfo;
DiskDriveInfo diskDriveInfo;
EthernetAdapterInfo ethernetAdapterInfo;
GPUInfo gpuInfo;
MOBOInfo moboInfo;
RAMInfo ramInfo;

// Displays ASCII art
void displayTitle() {
    std::system("cls"); // Clear the screen for a clean menu display
    std::cout << R"(
   __ ___      _________    _______ _____________ _________ 
  / // / | /| / /  _/ _ \  / ___/ // / __/ ___/ //_/ __/ _ \
 / _  /| |/ |/ // // // / / /__/ _  / _// /__/ ,< / _// , _/
/_//_/ |__/|__/___/____/  \___/_//_/___/\___/_/|_/___/_/|_|                                                  
)";
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

// Displays the main menu and handles user input
void displayMenu() {
    std::system("cls"); 
    std::cout << "Select hardware information to display:\n"
        << "1. CPU Information\n"
        << "2. Disk Drive Information\n"
        << "3. Ethernet Adapter Information\n"
        << "4. GPU Information\n"
        << "5. Motherboard Information\n"
        << "6. RAM Information\n"
        << "7. BIOS Information\n"
        << "8. Display All Information\n"
        << "9. Export Information to TXT\n"
        << "0. Exit\n"
        << "Enter choice: ";
}

// Function to export hardware information to a text file
void exportToText() {
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
    std::ostringstream strCout;
    std::cout.rdbuf(strCout.rdbuf());

    // Calling display methods to capture their output
    biosInfo.displayBIOSInfo();
    cpuInfo.displayCPUInfo();
    diskDriveInfo.displayDiskDriveInfo();
    ethernetAdapterInfo.displayEthernetAdapterInfo();
    gpuInfo.displayGPUInfo();
    moboInfo.displayMOBOInfo();
    ramInfo.displayRAMInfo();

    // Restore std::cout's original stream buffer
    std::cout.rdbuf(oldCoutStreamBuf);

    // Open the file for output with UTF-8 encoding
    std::ofstream file("hardware_info.txt", std::ios::out | std::ios::binary); // Use binary mode to prevent transformations
    // Ensure the stream uses UTF-8 encoding
    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<char>));

    if (file.is_open()) {
        file << strCout.str();
        file.close();
        std::cout << "Information exported to hardware_info.txt\n";
    }
    else {
        std::cout << "Failed to create export file.\n";
    }
}

int main() {
    SetConsoleTitle(L"HWIDChecker");
    displayTitle();

    int choice = -1;
    while (choice != 0) {
        displayMenu();
        std::cin >> choice;

        if (std::cin.fail()) { // Input validation
            std::cin.clear(); // Clear error flags
            std::cin.ignore(10000, '\n'); // Clear the input buffer
            continue; // Skip to the next iteration
        }

        std::system("cls");

        // Switch-case to handle user choice
        switch (choice) {
        case 1: cpuInfo.displayCPUInfo(); break;
        case 2: diskDriveInfo.displayDiskDriveInfo(); break;
        case 3: ethernetAdapterInfo.displayEthernetAdapterInfo(); break;
        case 4: gpuInfo.displayGPUInfo(); break;
        case 5: moboInfo.displayMOBOInfo(); break;
        case 6: ramInfo.displayRAMInfo(); break;
        case 7: biosInfo.displayBIOSInfo(); break;
        case 8:
            // Display all information sequentially
            biosInfo.displayBIOSInfo();
            cpuInfo.displayCPUInfo();
            diskDriveInfo.displayDiskDriveInfo();
            ethernetAdapterInfo.displayEthernetAdapterInfo();
            gpuInfo.displayGPUInfo();
            moboInfo.displayMOBOInfo();
            ramInfo.displayRAMInfo();
            break;
        case 9: exportToText(); break;
        case 0: std::cout << "Exiting...\n"; break;
        default: std::cout << "Invalid choice, please try again.\n";
        }

        if (choice != 0) {
            std::cout << "\nPress Enter to return to menu...";
            while (std::cin.get() != '\n') {} // Clear the input buffer
            std::cin.get(); // Wait for Enter press
        }
    }

    return 0;
}
