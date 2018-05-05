#ifndef RSA_H
#define RSA_H

#define ulong unsigned long long
#include <iomanip>
#include <time.h>

#ifdef RSA_EXPORTS  
#define RSA_API __declspec(dllexport)   
#else  
#define RSA_API __declspec(dllimport)   
#endif

#ifdef __cplusplus
extern "C"
{
#endif

ulong GetPrime();
bool IsPrime(ulong number);
ulong GCD(ulong a, ulong b);
ulong ModularPow(ulong base, ulong power, ulong modulus);
RSA_API void GenerateKeys(ulong * e, ulong * d, ulong * n);
RSA_API void CryptMessage(wchar_t * msg, int length, ulong e, ulong n, ulong * crypto);
RSA_API void DecryptMessage(wchar_t * msg, int length, ulong d, ulong n, ulong * crypto);

#ifdef __cplusplus
}
#endif


#endif