#pragma once
#define ulong unsigned long long
#include <iomanip>
#include <time.h>

ulong GetPrime();
bool IsPrime(ulong number);
ulong GCD(ulong a, ulong b);
ulong ModularPow(ulong base, ulong power, ulong modulus);
void GenerateKeys(ulong * e, ulong * d, ulong * n);
void CryptMessage(char * msg, int length, ulong e, ulong n, ulong * crypto);
void DecryptMessage(char * msg, int length, ulong d, ulong n, ulong * crypto);