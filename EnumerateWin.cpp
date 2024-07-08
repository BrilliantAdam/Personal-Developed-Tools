//brilliantadam
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <tchar.h>
#include <psapi.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "psapi.lib")

void GetSystemInfoDetails() {
    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo);

    std::cout << "Hardware information: " << std::endl;
    std::cout << "  OEM ID: " << siSysInfo.dwOemId << std::endl;
    std::cout << "  Number of processors: " << siSysInfo.dwNumberOfProcessors << std::endl;
    std::cout << "  Page size: " << siSysInfo.dwPageSize << std::endl;
    std::cout << "  Processor type: " << siSysInfo.dwProcessorType << std::endl;
    std::cout << "  Minimum application address: " << siSysInfo.lpMinimumApplicationAddress << std::endl;
    std::cout << "  Maximum application address: " << siSysInfo.lpMaximumApplicationAddress << std::endl;
    std::cout << "  Active processor mask: " << siSysInfo.dwActiveProcessorMask << std::endl;
}

void GetNetworkDetails() {
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    IP_ADAPTER_INFO *pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
    
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
    }

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
        IP_ADAPTER_INFO *pAdapter = pAdapterInfo;
        while (pAdapter) {
            std::cout << "Adapter Name: " << pAdapter->AdapterName << std::endl;
            std::cout << "Description: " << pAdapter->Description << std::endl;
            std::cout << "IP Address: " << pAdapter->IpAddressList.IpAddress.String << std::endl;
            std::cout << "IP Mask: " << pAdapter->IpAddressList.IpMask.String << std::endl;
            std::cout << "Gateway: " << pAdapter->GatewayList.IpAddress.String << std::endl;
            std::cout << "MAC Address: ";
            for (UINT i = 0; i < pAdapter->AddressLength; i++) {
                if (i == (pAdapter->AddressLength - 1))
                    printf("%.2X\n", (int)pAdapter->Address[i]);
                else
                    printf("%.2X-", (int)pAdapter->Address[i]);
            }
            std::cout << std::endl;
            pAdapter = pAdapter->Next;
        }
    }

    if (pAdapterInfo)
        free(pAdapterInfo);
}

void GetRunningProcesses() {
    DWORD aProcesses[1024], cbNeeded, cProcesses;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return;
    }

    cProcesses = cbNeeded / sizeof(DWORD);
    std::cout << "Running processes:" << std::endl;

    for (unsigned int i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

            if (NULL != hProcess) {
                HMODULE hMod;
                DWORD cbNeeded;

                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                    GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
                }
            }
            std::wcout << L"  " << szProcessName << std::endl;
            CloseHandle(hProcess);
        }
    }
}

void GetInstalledSoftware() {
    HKEY hUninstallKey = NULL;
    HKEY hAppKey = NULL;
    TCHAR sAppName[1024];
    DWORD cchAppName = 1024;
    LONG lResult = 0;
    DWORD dwIndex = 0;

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"), 0, KEY_READ, &hUninstallKey) == ERROR_SUCCESS) {
        while (RegEnumKeyEx(hUninstallKey, dwIndex, sAppName, &cchAppName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
            if (RegOpenKeyEx(hUninstallKey, sAppName, 0, KEY_READ, &hAppKey) == ERROR_SUCCESS) {
                TCHAR szDisplayName[1024];
                DWORD dwBufferSize = sizeof(szDisplayName);
                if (RegQueryValueEx(hAppKey, TEXT("DisplayName"), NULL, NULL, (LPBYTE)szDisplayName, &dwBufferSize) == ERROR_SUCCESS) {
                    std::wcout << L"Installed Software: " << szDisplayName << std::endl;
                }
                RegCloseKey(hAppKey);
            }
            cchAppName = 1024;
            dwIndex++;
        }
        RegCloseKey(hUninstallKey);
    }
}

int main() {
    GetSystemInfoDetails();
    std::cout << std::endl;

    GetNetworkDetails();
    std::cout << std::endl;

    GetRunningProcesses();
    std::cout << std::endl;

    GetInstalledSoftware();
    std::cout << std::endl;

    return 0;
}
