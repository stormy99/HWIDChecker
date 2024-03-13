# HWIDChecker

HWIDChecker is a C++ application designed to retrieve and display various hardware information from a Windows system. It utilizes the Windows Management Instrumentation (WMI) API to query system data related to the BIOS, CPU, disk drives, Ethernet adapters, GPU, motherboard, and RAM. The user can select specific hardware components to view detailed information or export all information to a text file.

Given the scarcity of HWIDChecker programs developed in C++, I took it upon myself to address this void by creating one.

## Features

- Display detailed hardware information including:
  - BIOS
  - CPU
  - Disk Drives
  - Ethernet Adapters
  - GPU
  - Motherboard (MOBO)
  - RAM
- Export hardware information to a text file (TXT)
- Simple and intuitive user interface
- Cross-platform compatibility (Windows)

## Prerequisites

- Windows operating system
- C++ compiler with C++11 support
- Windows SDK (for building on Windows)
- Visual Studio or MinGW (for building on Windows)
- CMake (optional, for building with CMake)

## Building from Source

1. Clone the repository:

   ```bash
   git clone https://github.com/your-username/HWIDChecker.git
   ```

2. Navigate to the project directory:

   ```bash
   cd HWIDChecker
   ```

3. Build the application:

   - Using Visual Studio:

     Open the solution file `HWIDChecker.sln` in Visual Studio, build the solution, and run the executable.

   - Using MinGW:

     ```bash
     g++ main.cpp BIOSInfo.cpp CPUInfo.cpp DiskDriveInfo.cpp EthernetAdapterInfo.cpp GPUInfo.cpp MOBOInfo.cpp RAMInfo.cpp -o HWIDChecker.exe
     ```

   - Using CMake:

     ```bash
     mkdir build
     cd build
     cmake ..
     cmake --build .
     ```

## Usage

1. Build the application as per the instructions provided above.

2. Run the executable:

   ```bash
   ./HWIDChecker.exe
   ```

3. Follow the on-screen menu to select the hardware information you want to display or export.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by [Open Hardware Monitor](https://openhardwaremonitor.org/)
