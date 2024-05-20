//brilliantadam
//21/5/2024
//####################################
//#########ProcGuardv1.0 //###########
//####################################
//Guards against suspicious processes,
//Auto upload hashes to "VirusTotal"
//####################################
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <json/json.h>
#include <chrono>
#include <thread>

// Function to upload file hash to VirusTotal
Json::Value uploadToVirusTotal(const std::string& fileHash, const std::string& apiKey) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        std::string url = "https://www.virustotal.com/vtapi/v2/file/report";
        std::string params = "resource=" + fileHash + "&apikey=" + apiKey;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    Json::Value jsonData;
    Json::Reader jsonReader;
    jsonReader.parse(readBuffer, jsonData);

    return jsonData;
}

// Function to check antivirus detections
bool checkDetections(const Json::Value& jsonData) {
    int detections = 0;
    for (auto it = jsonData["scans"].begin(); it != jsonData["scans"].end(); ++it) {
        if (it->second["detected"]) {
            detections++;
        }
    }
    return detections > 1;
}

// Function to report notification to the user
void reportNotification(const std::string& processName) {
    std::string message = "Suspicious process detected: " + processName;
    MessageBox(NULL, message.c_str(), "Suspicious Process Detected", MB_ICONWARNING | MB_OK);
}

int main() {
    // Load all running processes
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        std::cerr << "Error in Process32First: " << GetLastError() << std::endl;
        return 1;
    }

    // Get file hash of suspicious process
    std::string fileHash = getFileHash(pe32.szExeFile);

    // Upload file hash to VirusTotal and get response
    Json::Value jsonData = uploadToVirusTotal(fileHash, "your_virustotal_api_key");

    // Check for antivirus detections
    if (checkDetections(jsonData)) {
        reportNotification(pe32.szExeFile);
    }

    // Release resources
    CloseHandle(hSnapshot);

    return 0;
}
