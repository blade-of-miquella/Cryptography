#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
using namespace std;

enum CipherMode { OFB = 1 };

bool encryptFile(const string& inputFile,
    const string& outputFile,
    const string& password,
    CipherMode mode = OFB);

bool decryptFile(const string& inputFile,
    const string& outputFile,
    const string& password,
    CipherMode mode = OFB);

#endif // CRYPTO_H
