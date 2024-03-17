#include "GPUInfo.hpp"

#pragma comment(lib, "wbemuuid.lib")

GPUInfo::GPUInfo() {
    // Initialize COM and query WMI for GPU information
    if (!initializeCOM() || !queryWMIForGPUInfo()) {
        gpuDetails.push_back("Failed to retrieve GPU information");
    }
}

bool GPUInfo::initializeCOM() {
    // Initialize COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM library. Error code: " << hr << "\n";
        return false;
    }

    // Initialize COM security
    hr = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
    if (FAILED(hr) && hr != RPC_E_TOO_LATE) {
        CoUninitialize();
        std::cerr << "Failed to initialize COM security. Error code: " << hr << "\n";
        return false;
    }
    return true;
}

bool GPUInfo::queryWMIForGPUInfo() {
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
    hr = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT Name, Description, AdapterRAM, DriverVersion FROM Win32_VideoController"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);
    if (FAILED(hr)) {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    while (pEnumerator && pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn) == WBEM_S_NO_ERROR) {
        VARIANT varName, varDriverVersion;
        VariantInit(&varName);
        VariantInit(&varDriverVersion);

        pclsObj->Get(L"Name", 0, &varName, nullptr, nullptr);
        pclsObj->Get(L"DriverVersion", 0, &varDriverVersion, nullptr, nullptr);

        // Construct GPU detail string
        std::stringstream ss;
        ss << "Name: " << _bstr_t(varName.bstrVal) << "; ";
        ss << "Driver Version: " << _bstr_t(varDriverVersion.bstrVal);
        gpuDetails.push_back(ss.str());

        VariantClear(&varName);
        VariantClear(&varDriverVersion);
        pclsObj->Release();
    }

    pEnumerator->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

    return true;
}

void GPUInfo::displayGPUInfo() const {
    if (!gpuDetails.empty()) {
        std::cout << "GPU: \n";

        // Iterate over GPU details and display them
        for (const auto& detail : gpuDetails) {
            // Split the detail by semicolons into property lines
            std::istringstream propStream(detail);
            std::string propertyLine;
            while (std::getline(propStream, propertyLine, ';')) {
                // Trim leading whitespace
                propertyLine.erase(0, propertyLine.find_first_not_of(" \t"));
                if (!propertyLine.empty()) {
                    size_t colonPos = propertyLine.find(':');
                    if (colonPos != std::string::npos) {
                        std::string key = propertyLine.substr(0, colonPos);
                        std::string value = propertyLine.substr(colonPos + 2); // Skip ": " to get the value

                        // Output in a key-value pair format
                        std::cout << std::left << std::setw(20) << key << ": " << value << "\n";
                    }
                }
            }
            std::cout << "\n"; // Extra line for spacing between multiple GPUs, if present
        }
    }
    else {
        std::cout << "GPU information not available.\n";
    }
}
