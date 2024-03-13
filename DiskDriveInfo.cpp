#include "DiskDriveInfo.hpp"

#pragma comment(lib, "wbemuuid.lib")

DiskDriveInfo::DiskDriveInfo() {
    // Initialize COM and query WMI for disk drive information
    if (!initializeCOM() || !queryWMIForDiskDriveInfo()) {
        diskDriveDetails = "Failed to retrieve disk drive information";
    }
}

bool DiskDriveInfo::initializeCOM() {
    // Initialize COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM library. Error code: " << hr << "\n";
        return false;
    }

    // Initialize COM security
    hr = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
    if (FAILED(hr)) {
        CoUninitialize();
        std::cerr << "Failed to initialize COM security. Error code: " << hr << "\n";
        return false;
    }
    return true;
}

bool DiskDriveInfo::queryWMIForDiskDriveInfo() {
    IWbemLocator* pLoc = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&pLoc));
    if (FAILED(hr)) {
        CoUninitialize();
        return false;
    }

    IWbemServices* pSvc = nullptr;
    hr = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, nullptr, 0, nullptr, nullptr, &pSvc);
    if (FAILED(hr)) {
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    hr = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    if (FAILED(hr)) {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    IEnumWbemClassObject* pEnumerator = nullptr;
    hr = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT Model, SerialNumber FROM Win32_DiskDrive"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);
    if (FAILED(hr)) {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;
    std::ostringstream detailsStream;

    // Retrieve disk drive information
    while (pEnumerator && pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn) == WBEM_S_NO_ERROR) {
        VARIANT varModel, varSerialNumber;
        VariantInit(&varModel);
        VariantInit(&varSerialNumber);

        pclsObj->Get(L"Model", 0, &varModel, nullptr, nullptr);
        pclsObj->Get(L"SerialNumber", 0, &varSerialNumber, nullptr, nullptr);

        // Append disk drive details to the stringstream
        detailsStream << "Model: " << _bstr_t(varModel.bstrVal) << ", Serial Number: " << _bstr_t(varSerialNumber.bstrVal) << "\n";

        VariantClear(&varModel);
        VariantClear(&varSerialNumber);
        pclsObj->Release();
    }

    pEnumerator->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

    diskDriveDetails = detailsStream.str();
    return true;
}

void DiskDriveInfo::displayDiskDriveInfo() const {
    if (!diskDriveDetails.empty()) {
        std::cout << "DRIVES: \n";

        // Print table header
        std::cout << std::left << std::setw(30) << "Model" << "Serial Number\n";
        std::cout << std::string(50, '-') << "\n"; // Separator line

        // Print disk drive details
        std::istringstream stream(diskDriveDetails);
        std::string line;
        while (std::getline(stream, line)) {
            std::istringstream lineStream(line);
            std::string model, serialNumber, temp;

            // Extract model and serial number from the line
            std::getline(lineStream, model, ',');
            std::getline(lineStream, temp, ' '); // Skip the space
            std::getline(lineStream, serialNumber);

            model = model.substr(model.find(":") + 2); // Adjust to remove "Model: "
            serialNumber = serialNumber.substr(serialNumber.find(":") + 2); // Adjust for "Serial Number: "

            // Print model and serial number
            std::cout << std::left << std::setw(30) << model << serialNumber << "\n";
        }
    }
    else {
        std::cout << "Disk drive information not available.\n";
    }

    std::cout << "\n";
}
