#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
using namespace std;

vector<int> homophonicDecrypt(const vector<int>& input, const unordered_map<int, int>& reverseHomophonicTable) {
    vector<int> decryptedMessage;
    for (int code : input) {
        if (reverseHomophonicTable.find(code) != reverseHomophonicTable.end())
            decryptedMessage.push_back(reverseHomophonicTable.at(code));
        else
            decryptedMessage.push_back(code);
    }
    return decryptedMessage;
}

string lzwDecompress(const vector<int>& compressed) {
    unordered_map<int, string> dictionary;
    for (int i = 0; i < 256; i++)
        dictionary[i] = string(1, char(i));
    string current = dictionary[compressed[0]];
    string decompressed = current;
    string entry;
    int dictSize = 256;
    for (size_t i = 1; i < compressed.size(); ++i) {
        int key = compressed[i];
        if (dictionary.find(key) != dictionary.end())
            entry = dictionary[key];
        else if (key == dictSize)
            entry = current + current[0];
        else
            throw runtime_error("Error in data during decompression!");
        decompressed += entry;
        dictionary[dictSize++] = current + entry[0];
        current = entry;
    }
    return decompressed;
}

int main() {
    const wchar_t* sharedMemoryName = L"SharedMemory";
    const int bufferSize = 1024;

    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sharedMemoryName);
    if (!hMapFile) {
        cerr << "Can't open shared memory: " << GetLastError() << endl;
        return 1;
    }

    char* pBuffer = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
    if (!pBuffer) {
        cerr << "Can't map view of file: " << GetLastError() << endl;
        CloseHandle(hMapFile);
        return 1;
    }

    string key(pBuffer);
    cout << "Key received: " << key << endl;
    strcpy_s(pBuffer, bufferSize, "KeyReceived");

    cout << "Waiting for message..." << endl;
    while (strcmp(pBuffer, "KeyReceived") == 0)
        Sleep(100);

    string encryptedMessageStr(pBuffer);
    cout << "Encrypted message received: " << encryptedMessageStr << endl;

    vector<int> encryptedMessage;
    stringstream ss(encryptedMessageStr);
    int num;
    while (ss >> num)
        encryptedMessage.push_back(num);
    // The reverse mapping table must exactly match the server's encryption table.
    // If the server uses:
    //   space (32) -> {10, 11, 12}
    //   a (97)    -> {1000, 1001, 1002}
    //   b (98)    -> {1003, 1004, 1005}
    //   c (99)    -> {1006, 1007, 1008}, etc.
    // Then the client must have the corresponding reverse mapping for exactly these numbers.
    unordered_map<int, int> reverseHomophonicTable = {
        {10, 32}, {11, 32}, {12, 32},
        {1000, 97}, {1001, 97}, {1002, 97},
        {1003, 98}, {1004, 98}, {1005, 98},
        {1006, 99}, {1007, 99}, {1008, 99},
        {1009, 100}, {1010, 100}, {1011, 100},
        {1012, 101}, {1013, 101}, {1014, 101},
        {1015, 102}, {1016, 102}, {1017, 102},
        {1018, 103}, {1019, 103}, {1020, 103},
        {1021, 104}, {1022, 104}, {1023, 104},
        {1024, 105}, {1025, 105}, {1026, 105},
        {1027, 106}, {1028, 106}, {1029, 106},
        {1030, 107}, {1031, 107}, {1032, 107},
        {1033, 108}, {1034, 108}, {1035, 108},
        {1036, 109}, {1037, 109}, {1038, 109},
        {1039, 110}, {1040, 110}, {1041, 110},
        {1042, 111}, {1043, 111}, {1044, 111},
        {1045, 112}, {1046, 112}, {1047, 112},
        {1048, 113}, {1049, 113}, {1050, 113},
        {1051, 114}, {1052, 114}, {1053, 114},
        {1054, 115}, {1055, 115}, {1056, 115},
        {1057, 116}, {1058, 116}, {1059, 116},
        {1060, 117}, {1061, 117}, {1062, 117},
        {1063, 118}, {1064, 118}, {1065, 118},
        {1066, 119}, {1067, 119}, {1068, 119},
        {1069, 120}, {1070, 120}, {1071, 120},
        {1072, 121}, {1073, 121}, {1074, 121},
        {1075, 122}, {1076, 122}, {1077, 122}
    };

    vector<int> decryptedMessage = homophonicDecrypt(encryptedMessage, reverseHomophonicTable);

    cout << "Decrypted message (numeric): ";
    for (int code : decryptedMessage)
        cout << code << " ";
    cout << endl;

    string decompressedMessage = lzwDecompress(decryptedMessage);
    cout << "Decompressed message: " << decompressedMessage << endl;

    UnmapViewOfFile(pBuffer);
    CloseHandle(hMapFile);
    return 0;
}
