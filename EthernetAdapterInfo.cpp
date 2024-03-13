#include "EthernetAdapterInfo.hpp"

#pragma comment(lib, "wbemuuid.lib")

EthernetAdapterInfo::EthernetAdapterInfo() {
    // Initialize COM and query WMI for Ethernet adapter information
    if (!initializeCOM() || !queryWMIForEthernetAdapterInfo()) {
        adapterDetails = "Failed to retrieve Ethernet adapter information";
    }
}

bool EthernetAdapterInfo::initializeCOM() {
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

bool EthernetAdapterInfo::queryWMIForEthernetAdapterInfo() {
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
    hr = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT Name, MACAddress, NetEnabled FROM Win32_NetworkAdapter WHERE NetConnectionID IS NOT NULL"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);
    if (FAILED(hr)) {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;
    std::ostringstream detailsStream;

    // Retrieve Ethernet adapter information
    while (pEnumerator && pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn) == WBEM_S_NO_ERROR) {
        VARIANT varName, varMACAddress, varNetEnabled;
        VariantInit(&varName);
        VariantInit(&varMACAddress);
        VariantInit(&varNetEnabled);

        pclsObj->Get(L"Name", 0, &varName, nullptr, nullptr);
        pclsObj->Get(L"MACAddress", 0, &varMACAddress, nullptr, nullptr);
        pclsObj->Get(L"NetEnabled", 0, &varNetEnabled, nullptr, nullptr);

        // Append Ethernet adapter details to the stringstream
        detailsStream << "Name: " << _bstr_t(varName.bstrVal)
            << ", MAC Address: " << _bstr_t(varMACAddress.bstrVal)
            << ", Enabled: " << (varNetEnabled.boolVal ? "Yes" : "No") << "\n";

        VariantClear(&varName);
        VariantClear(&varMACAddress);
        VariantClear(&varNetEnabled);
        pclsObj->Release();
    }

    pEnumerator->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

    adapterDetails = detailsStream.str();
    return true;
}

void EthernetAdapterInfo::displayEthernetAdapterInfo() const {
    if (!adapterDetails.empty()) {
        std::cout << "Ethernet Adapter: \n";
        // Header
        std::cout << std::left
            << std::setw(50) << "Name"
            << std::setw(20) << "MAC Address"
            << "Enabled\n";
        std::cout << std::string(80, '-') << "\n"; // Separator line

        // Print Ethernet adapter details
        std::istringstream stream(adapterDetails);
        std::string line;
        while (std::getline(stream, line)) {
            // Extract each detail by parsing the line
            std::istringstream detailStream(line);
            std::string name, macAddress, enabled;
            std::getline(detailStream, name, ','); // Extract up to the first comma
            name = name.substr(name.find(":") + 2); // Remove "Name: " prefix

            std::getline(detailStream, macAddress, ','); // Extract up to the second comma
            macAddress = macAddress.substr(macAddress.find(":") + 2); // Remove "MAC Address: " prefix

            std::getline(detailStream, enabled); // The rest is the enabled state
            enabled = enabled.substr(enabled.find(":") + 2); // Remove "Enabled: " prefix

            // Output each row with proper formatting
            std::cout << std::left
                << std::setw(50) << name
                << std::setw(20) << macAddress
                << enabled << "\n";
        }
    }
    else {
        std::cout << "Ethernet adapter information not available.\n";
    }

    std::cout << "\n";
}
