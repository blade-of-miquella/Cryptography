#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

string encrypt(vector<vector<char>> msg, const string& key) {
    string ciphertext = "";
    vector<pair<char, int>> keyMap;
    for (int i = 0; i < key.size(); ++i)
        keyMap.push_back({ key[i], i });
    sort(keyMap.begin(), keyMap.end());
    for (auto& [ch, col] : keyMap)
        for (int i = 0; i < msg.size(); ++i)
            if (msg[i][col] != '_') ciphertext.push_back(msg[i][col]);
    return ciphertext;
}

string decrypt(const string& ciphertext, const string& key, int rows) {
    int columns = key.size();
    int n = ciphertext.size();
    vector<vector<char>> msg(rows, vector<char>(columns, '_'));

    vector<pair<char, int>> keyMap;
    for (int j = 0; j < columns; ++j)
        keyMap.push_back({ key[j], j });
    sort(keyMap.begin(), keyMap.end());

    vector<int> colCount(columns, 0);
    for (int j = 0; j < columns; ++j) {
        if (j * rows < n)
            colCount[j] = ((j + 1) * rows <= n) ? rows : (n - j * rows);
        else colCount[j] = 0;
    }

    int idx = 0;
    for (auto& [ch, origCol] : keyMap) {
        int count = colCount[origCol];
        for (int i = 0; i < count; ++i) msg[i][origCol] = ciphertext[idx++];
    }

    string plaintext = "";
    for (int j = 0; j < columns; ++j)
        for (int i = 0; i < colCount[j]; ++i) plaintext.push_back(msg[i][j]);
    return plaintext;
}

int main() {
    vector<vector<char>> msg;
    string message, key;

    cout << "Enter your message: ";
    cin >> message;
    cout << "Enter a key: ";
    cin >> key;

    int columns = key.length();
    int rows = (message.length() + columns - 1) / columns;
    msg.resize(rows, vector<char>(columns, '_'));

    int iter = 0;
    for (int j = 0; j < columns; ++j)
        for (int i = 0; i < rows; ++i)
            if (iter < message.length()) msg[i][j] = message[iter++];
            else msg[i][j] = '_';

    string encryptedMessage = encrypt(msg, key);
    cout << "Encrypted message: " << encryptedMessage << endl;

    string decryptedMessage = decrypt(encryptedMessage, key, rows);
    cout << "Decrypted message: " << decryptedMessage << endl;

    return 0;
}
