#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <random>

using namespace std;

vector<vector<string>> encrypt(vector<vector<string>> message, vector<int> colKey, vector<int> rowKey) {
    vector<vector<string>> columnSwapped = message;
    for (int j = 0; j < message.size(); j++) {
        vector<string> tempCol(colKey.size());
        for (int i = 0; i < colKey.size(); i++) {
            tempCol[colKey[i] - 1] = columnSwapped[j][i]; 
        }
        columnSwapped[j] = tempCol;
    }

    vector<vector<string>> rowSwapped(columnSwapped.size(), vector<string>(colKey.size()));
    for (int i = 0; i < rowKey.size(); i++) {
        rowSwapped[rowKey[i] - 1] = columnSwapped[i]; 
    }

    return rowSwapped;
}

vector<vector<string>> decrypt(vector<vector<string>> encrypted, vector<int> colKey, vector<int> rowKey) {
    vector<vector<string>> rowRestored(encrypted.size(), vector<string>(colKey.size()));
    for (int i = 0; i < rowKey.size(); i++) {
        rowRestored[i] = encrypted[rowKey[i] - 1];
    }

    vector<vector<string>> columnRestored(rowRestored.size(), vector<string>(colKey.size()));
    for (int j = 0; j < rowRestored.size(); j++) {
        for (int i = 0; i < colKey.size(); i++) {
            columnRestored[j][i] = rowRestored[j][colKey[i] - 1];
        }
    }
    return columnRestored;
}

int main() {
    random_device rd;
    mt19937 generator(rd());
    vector<vector<string>> message;
    string msg;
    cout << "Enter your message: ";
    getline(cin, msg);

    int colNums = 0, rowNums = 0;

    for (int i = sqrt(msg.length()); i > 0; i--) {
        if (msg.length() % i == 0) {
            colNums = msg.length() / i;
            rowNums = i;
            break;
        }
    }

    message.resize(rowNums, vector<string>(colNums, "_"));

    vector<int> shuffleRows(rowNums);
    vector<int> shuffleCols(colNums);
    iota(shuffleRows.begin(), shuffleRows.end(), 1);
    iota(shuffleCols.begin(), shuffleCols.end(), 1);

    shuffle(shuffleRows.begin(), shuffleRows.end(), generator);
    shuffle(shuffleCols.begin(), shuffleCols.end(), generator);

    int count = 0;
    for (int i = 0; i < rowNums; i++) {
        for (int j = 0; j < colNums; j++) {
            if (count < msg.length()) {
                message[i][j] = string(1, msg[count]);
                count++;
            }
            else {
                message[i][j] = "_";
            }
        }
    }
    vector<vector<string>> encryptedMsg = encrypt(message, shuffleCols, shuffleRows);
    cout << "\nEncrypted Message:" << endl;
    for (const auto& row : encryptedMsg) {
        for (const auto& cell : row) {
            cout << cell;
        }
    }

    vector<vector<string>> decryptedMsg = decrypt(encryptedMsg, shuffleCols, shuffleRows);

    cout << "\nDecrypted Message:" << endl;
    for (const auto& row : decryptedMsg) {
        for (const auto& cell : row) {
            cout << cell;
        }
    }

    return 0;
}
