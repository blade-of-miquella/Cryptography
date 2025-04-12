#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
using namespace std;

vector<int> lzwCompress(const string& input) {
    unordered_map<string, int> dictionary;
    for (int i = 0; i < 256; ++i)
        dictionary[string(1, char(i))] = i;
    string current = "";
    vector<int> compressed;
    int dictSize = 256;
    for (char c : input) {
        current += c;
        if (dictionary.find(current) == dictionary.end()) {
            dictionary[current] = dictSize++;
            current.pop_back();
            compressed.push_back(dictionary[current]);
            current = c;
        }
    }
    if (!current.empty())
        compressed.push_back(dictionary[current]);
    return compressed;
}

vector<int> homophonicEncrypt(const vector<int>& input, const string& key) {
    unordered_map<int, vector<int>> homophonicTable = {
        {32, {10, 11, 12}},
        {97, {1000, 1001, 1002}},
        {98, {1003, 1004, 1005}},
        {99, {1006, 1007, 1008}},
        {100, {1009, 1010, 1011}},
        {101, {1012, 1013, 1014}},
        {102, {1015, 1016, 1017}},
        {103, {1018, 1019, 1020}},
        {104, {1021, 1022, 1023}},
        {105, {1024, 1025, 1026}},
        {106, {1027, 1028, 1029}},
        {107, {1030, 1031, 1032}},
        {108, {1033, 1034, 1035}},
        {109, {1036, 1037, 1038}},
        {110, {1039, 1040, 1041}},
        {111, {1042, 1043, 1044}},
        {112, {1045, 1046, 1047}},
        {113, {1048, 1049, 1050}},
        {114, {1051, 1052, 1053}},
        {115, {1054, 1055, 1056}},
        {116, {1057, 1058, 1059}},
        {117, {1060, 1061, 1062}},
        {118, {1063, 1064, 1065}},
        {119, {1066, 1067, 1068}},
        {120, {1069, 1070, 1071}},
        {121, {1072, 1073, 1074}},
        {122, {1075, 1076, 1077}}
    };

    srand((unsigned)time(0));
    vector<int> encrypted;
    size_t keyLength = key.length();
    for (size_t i = 0; i < input.size(); ++i) {
        int currentValue = input[i];
        if (homophonicTable.find(currentValue) != homophonicTable.end()) {
            const auto& options = homophonicTable[currentValue];
            int keyOffset = key[i % keyLength] % options.size();
            encrypted.push_back(options[keyOffset]);
        }
        else {
            encrypted.push_back(currentValue);
        }
    }
    return encrypted;
}

int main() {
    const wchar_t* sharedMemoryName = L"SharedMemory";
    const int bufferSize = 1024;
    string key = "secret";

    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, bufferSize, sharedMemoryName);
    if (!hMapFile) {
        cerr << "Can't create shared memory: " << GetLastError() << endl;
        return 1;
    }
    char* pBuffer = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
    if (!pBuffer) {
        cerr << "Can't map view of file: " << GetLastError() << endl;
        CloseHandle(hMapFile);
        return 1;
    }

    strcpy_s(pBuffer, bufferSize, key.c_str());
    cout << "Key sent: " << key << endl;
    cout << "Waiting for key received by client..." << endl;
    while (strcmp(pBuffer, key.c_str()) == 0)
        Sleep(100);

    string message;
    cout << "Enter message: ";
    getline(cin, message);

    vector<int> compressedMessage = lzwCompress(message);
    cout << "Compressed message: ";
    for (int code : compressedMessage)
        cout << code << " ";
    cout << endl;

    vector<int> encryptedMessage = homophonicEncrypt(compressedMessage, key);
    cout << "Encrypted message: ";
    for (int code : encryptedMessage)
        cout << code << " ";
    cout << endl;

    string encryptedMessageStr;
    for (int code : encryptedMessage)
        encryptedMessageStr += to_string(code) + " ";

    strcpy_s(pBuffer, bufferSize, encryptedMessageStr.c_str());
    cout << "\nEncrypted message sent: " << encryptedMessageStr << endl;

    UnmapViewOfFile(pBuffer);
    CloseHandle(hMapFile);
    return 0;
}
