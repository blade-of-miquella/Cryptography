#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

string encrypt(const string& message, const string& key, const string& alphabet) {
    unordered_map<char, int> char_to_index;
    unordered_map<int, char> index_to_char;
    for (int i = 0; i < alphabet.size(); ++i) {
        char_to_index[alphabet[i]] = i;
        index_to_char[i] = alphabet[i];
    }
    string encrypt_message = "";
    for (size_t i = 0; i < message.size(); ++i) {
        int letter_index = char_to_index[message[i]];
        int key_index = char_to_index[key[i % key.size()]];
        int encrypted_index = (letter_index + key_index) % alphabet.size();
        encrypt_message += index_to_char[encrypted_index];
    }
    return encrypt_message;
}

string decrypt(const string& encrypt_message, const string& key, const string& alphabet) {
    unordered_map<char, int> char_to_index;
    unordered_map<int, char> index_to_char;
    for (int i = 0; i < alphabet.size(); ++i) {
        char_to_index[alphabet[i]] = i;
        index_to_char[i] = alphabet[i];
    }
    string decrypt_message = "";
    for (size_t i = 0; i < encrypt_message.size(); ++i) {
        int letter_index = char_to_index[encrypt_message[i]];
        int key_index = char_to_index[key[i % key.size()]];
        int decrypted_index = (letter_index - key_index + alphabet.size()) % alphabet.size();
        decrypt_message += index_to_char[decrypted_index];
    }
    return decrypt_message;
}

string generateRandomKey(int length, const string& alphabet) {
    string key = "";
    for (int i = 0; i < length; ++i) {
        key += alphabet[rand() % alphabet.size()];
    }
    return key;
}

int main() {
    srand(time(0));
    string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string message, key;
    cout << "Enter message: ";
    cin >> message;
    cout << "Enter Secret(Tssss...) key(Or press '-' to random gen.): ";
    cin >> key;
    if (key == "-") {
        int length = rand() % 10;
        key = generateRandomKey(length, alphabet);
    }
    if (key.size() > message.size()) {
        key = key.substr(0, message.size());
    }
    cout << "Encrypting message..." << endl;
    string encrypt_message = encrypt(message, key, alphabet);
    cout << "Encrypted message - " << encrypt_message << endl;
    cout << "Decrypting message..." << endl;
    string decrypt_message = decrypt(encrypt_message, key, alphabet);
    cout << "Decrypted message - " << decrypt_message << endl;
    return 0;
}
