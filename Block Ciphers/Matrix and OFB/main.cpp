#include <iostream>
#include <chrono>
#include "matrixAndOFB.h"

using namespace std;

int main() {
    while (true) {
        cout << "Choose work mode (1 - encrypt, 2 - decrypt, 0 - exit): ";
        string choice;
        cin >> choice;

        if (choice == "0") {
            cout << "Exiting..." << endl;
            break;
        }
        string inputFile, outputFile, password;
        cout << "Input file: ";
        cin >> inputFile;
        cout << "Output file: ";
        cin >> outputFile;
        cout << "Password: ";
        cin >> password;
        CipherMode mode = OFB;
        auto start = chrono::high_resolution_clock::now();
        bool result = false;
        if (choice == "1") {
            result = encryptFile(inputFile, outputFile, password, mode);
            if (result) cout << "Success encryption." << endl;
            else  cout << "Encryption has failed." << endl;
        }
        else if (choice == "2") {
            result = decryptFile(inputFile, outputFile, password, mode);
            if (result) cout << "Success decryption." << endl;
            else cout << "Decryption has failed." << endl;
        }
        else {
            cerr << "Incorrect mode choice." << endl;
            continue;
        }
        auto end = chrono::high_resolution_clock::now();
        auto elapsedMs = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        cout << "Time: " << elapsedMs << " ms" << endl << endl;
    }

    return 0;
}
