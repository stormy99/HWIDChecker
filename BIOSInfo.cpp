#include "BIOSInfo.hpp"

#pragma comment(lib, "wbemuuid.lib")

BIOSInfo::BIOSInfo() {
    if (!queryBIOSUUID()) {
        biosUUID = "Unable to retrieve BIOS UUID";
    }
}

bool BIOSInfo::queryBIOSUUID() {
    HRESULT hres;

    // Initialize COM
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) {
        std::cerr << "Failed to initialize COM library. Error code = " << hres << "\n";
        return false;
    }

    // Set general COM security levels
    hres = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
    if (FAILED(hres)) {
        CoUninitialize();
        return false;
    }

    // Obtain the initial locator to WMI
    IWbemLocator* pLoc = nullptr;
    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&pLoc));
    if (FAILED(hres)) {
        CoUninitialize();
        return false;
    }

    // Connect to WMI through the IWbemLocator::ConnectServer method
    IWbemServices* pSvc = nullptr;
    hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, nullptr, 0, nullptr, nullptr, &pSvc);
    if (FAILED(hres)) {
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    // Set security levels on the proxy
    hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    if (FAILED(hres)) {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    // Use the IWbemServices pointer to make requests of WMI
    IEnumWbemClassObject* pEnumerator = nullptr;
    hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT UUID FROM Win32_ComputerSystemProduct"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);
    if (FAILED(hres)) {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    // Get BIOS UUID from WMI
    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;
    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (0 == uReturn) {
            break;
        }

        VARIANT vtProp;
        VariantInit(&vtProp); // Initialize vtProp before use

        // Get the value of the UUID property
        hr = pclsObj->Get(L"UUID", 0, &vtProp, nullptr, nullptr);
        if (SUCCEEDED(hr)) {
            biosUUID = _bstr_t(vtProp.bstrVal);
            VariantClear(&vtProp);
        }

        pclsObj->Release();
    }

    // Cleanup
    pEnumerator->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

    return true;
}

void BIOSInfo::displayBIOSInfo() const {
    std::cout << "SYS-BIOS UUID: " << biosUUID << "\n\n";
}

