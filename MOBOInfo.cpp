#include "MOBOInfo.hpp"

#pragma comment(lib, "wbemuuid.lib")

MOBOInfo::MOBOInfo() {
    // Query WMI for motherboard information
    if (!queryWMIForMOBOInfo()) {
        moboInfo = "Failed to retrieve motherboard information";
    }
}

bool MOBOInfo::initializeCOM() {
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

bool MOBOInfo::queryWMIForMOBOInfo() {
    if (!initializeCOM()) return false;

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
    hr = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_BaseBoard"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);
    if (FAILED(hr)) {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;
    std::ostringstream infoStream;

    // Retrieve motherboard information
    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (0 == uReturn) {
            break;
        }

        // Lambda function to get property value
        auto getProp = [&](const wchar_t* propName) -> std::string {
            VARIANT vtProp;
            VariantInit(&vtProp);
            hr = pclsObj->Get(propName, 0, &vtProp, nullptr, nullptr);
            std::string propValue;
            if (SUCCEEDED(hr) && V_VT(&vtProp) == VT_BSTR && vtProp.bstrVal != nullptr) {
                propValue = _com_util::ConvertBSTRToString(vtProp.bstrVal);
            }
            VariantClear(&vtProp);
            return propValue.empty() ? "N/A" : propValue;
            };

        // Extracting and concatenating properties using the lambda function
        infoStream << "Manufacturer: " << getProp(L"Manufacturer") << "; ";
        infoStream << "Product: " << getProp(L"Product") << "; ";
        infoStream << "Serial Number: " << getProp(L"SerialNumber") << "; ";

        pclsObj->Release();
    }

    moboInfo = infoStream.str();
    pEnumerator->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
    return true;
}

void MOBOInfo::displayMOBOInfo() const {
    if (!moboInfo.empty()) {
        std::cout << "Motherboard Information:\n";

        std::istringstream infoStream(moboInfo);
        std::string line;

        // Print motherboard information
        while (std::getline(infoStream, line, ';')) {
            // Skip leading space if present
            if (!line.empty() && line.front() == ' ') {
                line.erase(0, 1); // Remove the first character if it's a space
            }

            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 2); // Skip ": " to get the value

                std::cout << std::left << std::setw(15) << key << ": " << value << "\n";
            }
        }
    }
    else {
        std::cout << "Motherboard information not available.\n";
    }

    std::cout << "\n";
}
