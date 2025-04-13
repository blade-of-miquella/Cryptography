#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;
const int n = 5;
vector<vector<int>> matrix = {
    {0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0},
    {0, 1, 0, 0, 0},
    {1, 0, 1, 0, 0}
};
vector<vector<string>> encrypted_message(n, vector<string>(n, ""));

string encrypt(const string& message) {
    int length = 0;
    int message_index = 0;

    while (message_index < message.length()) {
        for (int rotation = 0; rotation < 4; ++rotation) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (matrix[i][j] == 1 && message_index < message.length() && encrypted_message[i][j] == "") {
                        encrypted_message[i][j] = string(1, message[message_index]);
                        message_index++;
                    }
                }
            }

            vector<vector<int>> rotated_matrix(n, vector<int>(n, 0));
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    rotated_matrix[j][n - i - 1] = matrix[i][j];
                }
            }
            matrix = rotated_matrix;
        }
        if (message_index < message.length()) {
            reverse(matrix.begin(), matrix.end());
        }
    }

    string encrypted_msg = "";
    for (const auto& row : encrypted_message) {
        for (const auto& cell : row) {
            if (!cell.empty()) {
                encrypted_msg += cell;
            }
        }
    }
    return encrypted_msg;
}

string decrypt(const string& encrypted_msg) {
    string decrypted_message = "";
    int message_index = 0;

    while (message_index < encrypted_msg.length()) {
        for (int rotation = 0; rotation < 4; ++rotation) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (matrix[i][j] == 1 && message_index < encrypted_msg.length() && !encrypted_message[i][j].empty()) {
                        decrypted_message += encrypted_message[i][j];
                        encrypted_message[i][j] = "";
                        message_index++;
                    }
                }
            }

            vector<vector<int>> rotated_matrix(n, vector<int>(n, 0));
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    rotated_matrix[j][n - i - 1] = matrix[i][j];
                }
            }
            matrix = rotated_matrix;
        }

        if (message_index < encrypted_msg.length()) {
            reverse(matrix.begin(), matrix.end());
        }
    }

    return decrypted_message;
}

vector<string> split_into_blocks(const string& message, int block_size) {
    vector<string> blocks;
    int index = 0;
    while (index < message.length()) {
        string block = message.substr(index, block_size);
        blocks.push_back(block);
        index += block_size;
    }
    return blocks;
}

int main() {
    string message;
    cout << "Enter your message: ";
    getline(cin, message);

    if (message.length() <= 25) {
        cout << "Encrypting...\n";
        string encrypted_msg = encrypt(message);
        cout << "Encrypted Message:\n" << encrypted_msg << endl;
        string decrypted_msg = decrypt(encrypted_msg);
        cout << "Decrypted Message:\n" << decrypted_msg << endl;
    } else {
        cout << "Encrypting blocks...\n";
        vector<string> blocks = split_into_blocks(message, 25);
        string final_encrypted_msg = "";
        string final_decrypted_msg = "";
        for (string& block : blocks) {
            string encrypted_block = encrypt(block);
            final_encrypted_msg += encrypted_block;
            string decrypted_block = decrypt(encrypted_block);
            final_decrypted_msg += decrypted_block;
        }
        cout << "Encrypted Message:\n" << final_encrypted_msg << endl;
        cout << "Decrypted Message:\n" << final_decrypted_msg << endl;
    }

    return 0;
}
