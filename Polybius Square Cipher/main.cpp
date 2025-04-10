#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream> 

using namespace std;

int main() {
    unordered_map<char, string> PolybiusSquare;
    unordered_map<string, char> ReverseSquare; 
    char letter = 'A';
    for (int row = 1; row <= 5; ++row) {
        for (int col = 1; col <= 5; ++col) {
            string coordinates = to_string(row) + to_string(col);
            if (letter == 'I') {
                PolybiusSquare['I'] = coordinates;
                PolybiusSquare['J'] = coordinates; 
                ReverseSquare[coordinates] = 'I';
            }
            else if (letter > 'Z') {
                break;
            }
            PolybiusSquare[letter] = coordinates;
            ReverseSquare[coordinates] = letter;
            ++letter;
        }
    }
    string message = "";
    string encrypted_msg = "";
    string decrypted_msg = "";
    cout << "Enter your message: ";
    cin >> message;
    cout << "Encrypting your message...\n";
    for (auto symb : message) {
        symb = toupper(symb);
        encrypted_msg += PolybiusSquare[symb] + " ";
    }
    cout << "Encrypted Message: " << encrypted_msg << endl;
    cout << "Decrypting your message...\n";
    stringstream encryptedStream(encrypted_msg);
    string coordinates;
    while (encryptedStream >> coordinates) {
        decrypted_msg += ReverseSquare[coordinates];
    }
    cout << "Decrypted Message: " << decrypted_msg << endl;
    return 0;
}
