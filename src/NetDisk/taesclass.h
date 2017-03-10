#ifndef TAESCLASS_H
#define TAESCLASS_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <QByteArray>

typedef unsigned long DWORD;
typedef unsigned char UCHAR,*PUCHAR;
typedef void *PVOID,*LPVOID;
typedef unsigned char byte;
typedef DWORD *PDWORD,*LPDWORD;

#ifndef VOID
#define VOID void
#endif

//#pragma once

//enum KeySize { Bits128, Bits192, Bits256 }; // key size, in bits, for construtor
#define Bits128    16
#define Bits192    24
#define Bits256    32

class Aes // Advanced Encryption Standard
{
public:
    ~Aes();
    Aes();
    Aes(int keySize, unsigned char* keyBytes);
    unsigned char State[4][4];
    void Cipher(unsigned char* input, unsigned char* output); // encipher 16-bit input
    void InvCipher(unsigned char* input, unsigned char* output); // decipher 16-bit input
private:
    int Nb; // block size in 32-bit words. Always 4 for AES. (128 bits).
    int Nk; // key size in 32-bit words. 4, 6, 8. (128, 192, 256 bits).
    int Nr; // number of rounds. 10, 12, 14.

    unsigned char key[32];
    unsigned char w[16*15];

    void SetNbNkNr(int keySize);
    void AddRoundKey(int round); //轮密钥加
    void SubBytes(); //S盒字节代换
    void InvSubBytes(); //逆S盒字节代换
    void ShiftRows(); //行移位
    void InvShiftRows();
    void MixColumns(); //列混淆
    void InvMixColumns();
    unsigned char gfmultby01(unsigned char b);
    unsigned char gfmultby02(unsigned char b);
    unsigned char gfmultby03(unsigned char b);
    unsigned char gfmultby09(unsigned char b);
    unsigned char gfmultby0b(unsigned char b);
    unsigned char gfmultby0d(unsigned char b);
    unsigned char gfmultby0e(unsigned char b);
    void KeyExpansion(); //密钥扩展
    unsigned char* SubWord(unsigned char* word); //密钥S盒字代换
    unsigned char* RotWord(unsigned char* word); //密钥移位

};

class TAesClass
{
public:
    TAesClass();
    ~TAesClass();
    void InitializePrivateKey(DWORD KeySize,UCHAR *KeyBytes); //AES 密钥初始化
    DWORD OnAesEncrypt(LPVOID InBuffer,DWORD InLength,LPVOID OutBuffer);            //AES 加密数据
    DWORD OnAesUncrypt(LPVOID InBuffer,DWORD InLength,LPVOID OutBuffer);            //AES 解密数据
    QByteArray AesEncrypt(QByteArray plainText);
    QByteArray AesUncrypt(QByteArray cipherText);
private:
    Aes * m_lpAes;
};

#endif // TAESCLASS_H
