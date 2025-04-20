#include "matrixAndOFB.h"
#include <fstream>
#include <vector>
#include <array>
#include <cstdint>
#include <random>
#include <iostream>
#include <cstring>

using namespace std;

const size_t BLOCK_SIZE = 11;
const array<char, 4> MAGIC = { 'K','A','T','E' };
const array<unsigned char, BLOCK_SIZE> PASS_CHECK = { 'P','A','S','S','_','C','H','E','C','K','!' };

vector<vector<unsigned char>> generateKeyMatrix(const string& password) {
    vector<vector<unsigned char>> matrix(BLOCK_SIZE, vector<unsigned char>(BLOCK_SIZE, 0));
    size_t pwdLen = password.size();
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        for (size_t j = 0; j < BLOCK_SIZE; j++) {
            if (i == j) {
                matrix[i][j] = 1;
            }
            else if (i < j) {
                matrix[i][j] = static_cast<unsigned char>(
                    ((i + j) + static_cast<unsigned char>(password[(i + j) % pwdLen])) % 256);
            }
            else { // i > j
                matrix[i][j] = 0;
            }
        }
    }
    return matrix;
}

array<unsigned char, BLOCK_SIZE> matrixMultiply(
    const array<unsigned char, BLOCK_SIZE>& block,
    const vector<vector<unsigned char>>& matrix)
{
    array<unsigned char, BLOCK_SIZE> result = { 0 };
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        unsigned int sum = 0;
        for (size_t j = 0; j < BLOCK_SIZE; j++) {
            sum += matrix[i][j] * block[j];
        }
        result[i] = static_cast<unsigned char>(sum % 256);
    }
    return result;
}

array<unsigned char, BLOCK_SIZE> encryptBlockOFB(
    const array<unsigned char, BLOCK_SIZE>& block,
    const vector<vector<unsigned char>>& keyMatrix)
{
    return matrixMultiply(block, keyMatrix);
}

uint32_t calculateChecksum(const vector<unsigned char>& data) {
    uint32_t sum = 0;
    for (unsigned char byte : data) {
        sum = (sum + byte) & 0xFFFFFFFF;
    }
    return sum;
}

void writeUint32(ofstream& ofs, uint32_t value) {
    unsigned char bytes[4];
    bytes[0] = value & 0xFF;
    bytes[1] = (value >> 8) & 0xFF;
    bytes[2] = (value >> 16) & 0xFF;
    bytes[3] = (value >> 24) & 0xFF;
    ofs.write(reinterpret_cast<char*>(bytes), 4);
}

uint32_t readUint32(ifstream& ifs) {
    unsigned char bytes[4];
    ifs.read(reinterpret_cast<char*>(bytes), 4);
    uint32_t value = bytes[0] | (bytes[1] << 8) |
        (bytes[2] << 16) | (bytes[3] << 24);
    return value;
}

array<unsigned char, BLOCK_SIZE> generateRandomIV() {
    array<unsigned char, BLOCK_SIZE> iv;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        iv[i] = static_cast<unsigned char>(dis(gen));
    }
    return iv;
}

bool encryptFile(const string& inputFile,
    const string& outputFile,
    const string& password,
    CipherMode mode)
{
    ifstream ifs(inputFile, ios::binary);
    if (!ifs) {
        cerr << "Cant open input file: " << inputFile << endl;
        return false;
    }
    vector<unsigned char> plainData((istreambuf_iterator<char>(ifs)),
        istreambuf_iterator<char>());
    ifs.close();

    uint32_t originalSize = static_cast<uint32_t>(plainData.size());
    uint32_t checksum = calculateChecksum(plainData);
    auto keyMatrix = generateKeyMatrix(password);
    array<unsigned char, BLOCK_SIZE> passCheckBlock = encryptBlockOFB(PASS_CHECK, keyMatrix);
    array<unsigned char, BLOCK_SIZE> iv = generateRandomIV();
    vector<unsigned char> cipherData;
    array<unsigned char, BLOCK_SIZE> keystream = iv;
    for (size_t pos = 0; pos < plainData.size(); pos += BLOCK_SIZE) {
        keystream = encryptBlockOFB(keystream, keyMatrix);
        size_t remaining = plainData.size() - pos;
        size_t blockLen = (remaining < BLOCK_SIZE) ? remaining : BLOCK_SIZE;
        for (size_t i = 0; i < blockLen; i++) {
            unsigned char cipherByte = plainData[pos + i] ^ keystream[i];
            cipherData.push_back(cipherByte);
        }
    }

    ofstream ofs(outputFile, ios::binary);
    if (!ofs) {
        cerr << "Cant open output file: " << outputFile << endl;
        return false;
    }
    ofs.write(MAGIC.data(), MAGIC.size());
    unsigned char modeByte = static_cast<unsigned char>(OFB);
    ofs.write(reinterpret_cast<const char*>(&modeByte), 1);
    ofs.write(reinterpret_cast<const char*>(iv.data()), iv.size());
    ofs.write(reinterpret_cast<const char*>(passCheckBlock.data()), passCheckBlock.size());
    writeUint32(ofs, originalSize);
    writeUint32(ofs, checksum);
    ofs.write(reinterpret_cast<const char*>(cipherData.data()), cipherData.size());
    ofs.close();

    return true;
}

bool decryptFile(const string& inputFile,
    const string& outputFile,
    const string& password,
    CipherMode /*mode*/)
{
    ifstream ifs(inputFile, ios::binary);
    if (!ifs) {
        cerr << "Cant open input file: " << inputFile << endl;
        return false;
    }
    array<char, 4> fileMagic;
    ifs.read(fileMagic.data(), fileMagic.size());
    if (memcmp(fileMagic.data(), MAGIC.data(), MAGIC.size()) != 0) {
        cerr << "Incorrect file format." << endl;
        return false;
    }
    unsigned char fileMode;
    ifs.read(reinterpret_cast<char*>(&fileMode), 1);
    if (fileMode != static_cast<unsigned char>(OFB)) {
        cerr << "Unsupported encryption mode. Only OFB mode is supported." << endl;
        return false;
    }

    array<unsigned char, BLOCK_SIZE> iv;
    ifs.read(reinterpret_cast<char*>(iv.data()), BLOCK_SIZE);
    array<unsigned char, BLOCK_SIZE> storedPassCheck;
    ifs.read(reinterpret_cast<char*>(storedPassCheck.data()), BLOCK_SIZE);
    uint32_t originalSize = readUint32(ifs);
    uint32_t storedChecksum = readUint32(ifs);
    vector<unsigned char> cipherData((istreambuf_iterator<char>(ifs)),
        istreambuf_iterator<char>());
    ifs.close();

    auto keyMatrix = generateKeyMatrix(password);
    array<unsigned char, BLOCK_SIZE> expectedPassCheck = encryptBlockOFB(PASS_CHECK, keyMatrix);
    if (memcmp(expectedPassCheck.data(), storedPassCheck.data(), BLOCK_SIZE) != 0) {
        cerr << "Wrong password." << endl;
        return false;
    }

    vector<unsigned char> plainData;
    array<unsigned char, BLOCK_SIZE> keystream = iv;
    size_t pos = 0;
    while (pos < cipherData.size()) {
        keystream = encryptBlockOFB(keystream, keyMatrix);
        size_t blockLen = ((cipherData.size() - pos) < BLOCK_SIZE) ? (cipherData.size() - pos) : BLOCK_SIZE;
        for (size_t i = 0; i < blockLen; i++) {
            unsigned char plainByte = cipherData[pos + i] ^ keystream[i];
            plainData.push_back(plainByte);
        }
        pos += blockLen;
    }
    if (plainData.size() < originalSize) {
        cerr << "Error: decrypted text less than expected." << endl;
        return false;
    }
    plainData.resize(originalSize);

    uint32_t computedChecksum = calculateChecksum(plainData);
    if (computedChecksum != storedChecksum) {
        cerr << "Control sum error. File corrupted or have been changed." << endl;
        return false;
    }

    ofstream ofs(outputFile, ios::binary);
    if (!ofs) {
        cerr << "Cant open output file: " << outputFile << endl;
        return false;
    }
    ofs.write(reinterpret_cast<const char*>(plainData.data()), plainData.size());
    ofs.close();
    return true;
}
