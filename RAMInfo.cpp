#include "RAMInfo.hpp"

#pragma comment(lib, "wbemuuid.lib")

RAMInfo::RAMInfo() {
    if (!initializeCOM() || !queryWMIForRAMInfo()) {
        ramDetails = "Failed to retrieve RAM information";
    }
}

bool RAMInfo::initializeCOM() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM library. Error code: " << hr << std::endl;
        return false;
    }

    hr = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
    if (FAILED(hr) && hr != RPC_E_TOO_LATE) {
        CoUninitialize();
        std::cerr << "Failed to initialize COM security. Error code: " << hr << std::endl;
        return false;
    }
    return true;
}

bool RAMInfo::queryWMIForRAMInfo() {
    IWbemLocator* pLoc = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&pLoc));
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
    hr = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT DeviceLocator, BankLabel, Capacity, Speed, Manufacturer FROM Win32_PhysicalMemory"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);
    if (FAILED(hr)) {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    std::ostringstream detailsStream;
    int slotNumber = 1;

    // Retrieve RAM information
    while (pEnumerator) {
        IWbemClassObject* pclsObj = nullptr;
        ULONG uReturn = 0;
        hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (uReturn == 0 || FAILED(hr)) {
            break;
        }

        VARIANT varDeviceLocator, varBankLabel, varManufacturer;
        VariantInit(&varDeviceLocator);
        VariantInit(&varBankLabel);
        VariantInit(&varManufacturer);

        hr = pclsObj->Get(L"DeviceLocator", 0, &varDeviceLocator, nullptr, nullptr);
        hr |= pclsObj->Get(L"BankLabel", 0, &varBankLabel, nullptr, nullptr);
        hr |= pclsObj->Get(L"Manufacturer", 0, &varManufacturer, nullptr, nullptr);

        if (SUCCEEDED(hr)) {
            detailsStream << "Slot " << slotNumber++ << ": ";
            detailsStream << "Manufacturer: " << _bstr_t(varManufacturer.bstrVal) << ", ";
            detailsStream << "DeviceLocator: " << _bstr_t(varDeviceLocator.bstrVal) << ", ";
            detailsStream << "BankLabel: " << _bstr_t(varBankLabel.bstrVal) << std::endl;
        }

        VariantClear(&varDeviceLocator);
        VariantClear(&varBankLabel);
        VariantClear(&varManufacturer);

        pclsObj->Release();
    }

    ramDetails = detailsStream.str();

    pEnumerator->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

    return true;
}

void RAMInfo::displayRAMInfo() const {
    if (!ramDetails.empty()) {
        std::cout << "RAM:\n";

        // Print table header
        std::cout << std::left
            << std::setw(10) << "Slot"
            << std::setw(20) << "Manufacturer"
            << std::setw(20) << "Locator"
            << "BankLabel" << std::endl;
        std::cout << std::string(60, '-') << std::endl; // Separator

        // Print RAM details
        std::istringstream infoStream(ramDetails);
        std::string detailLine;
        while (std::getline(infoStream, detailLine)) {
            // Parsing each line to extract details correctly
            std::string slot, manufacturer, locator, bankLabel;
            std::istringstream detailStream(detailLine);

            std::getline(detailStream, slot, ':'); // Slot number until colon
            detailStream >> manufacturer; // Manufacturer is next word
            std::getline(detailStream, manufacturer, ','); // Continue manufacturer until comma
            std::getline(detailStream, locator, ','); // DeviceLocator until comma
            std::getline(detailStream, bankLabel); // Rest is BankLabel

            // Clean up parsed strings (remove extra words, commas, and spaces)
            manufacturer.erase(manufacturer.find_last_not_of(", ") + 1);
            locator.erase(0, locator.find("DIMM")); // Start from "DIMM"
            bankLabel.erase(0, bankLabel.find("BANK")); // Start from "BANK"

            // Print the details in table format
            std::cout << std::left
                << std::setw(10) << slot
                << std::setw(20) << manufacturer
                << std::setw(20) << locator
                << bankLabel << std::endl;
        }
    }
    else {
        std::cout << "RAM information not available." << std::endl;
    }
}
