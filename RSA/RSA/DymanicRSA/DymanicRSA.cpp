// DymanicRSA.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
using namespace std;

// Визначення типів для адрес функцій
typedef bool(*TCRYPT)(unsigned, unsigned, unsigned, unsigned *);
typedef bool(*TDECRYPT)(unsigned, unsigned, unsigned, unsigned *);
typedef unsigned(*TENCRYPTANDDECRYPTKEY)(unsigned *, unsigned *);

int main(int argc, _TCHAR* argv[]) {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	

	// Завантаження бібліотеки
	HMODULE h = LoadLibrary(_T("RSA.dll"));
	if (h != 0) {
		// Визначення адрес для функцій, що експортуються
		TCRYPT Crypt = (TCRYPT)GetProcAddress(h, "Crypt");
		TDECRYPT DeCrypt = (TDECRYPT)GetProcAddress(h, "DeCrypt");
		TENCRYPTANDDECRYPTKEY EncryptAndDecryptKey = (TENCRYPTANDDECRYPTKEY)GetProcAddress(h, "EncryptAndDecryptKey");
		if (Crypt && DeCrypt && EncryptAndDecryptKey) {
			
			//_tsetlocale(LC_ALL, _T("Russian"));
			// Використання функцій

			unsigned E, D; //код для шифрования и разшифрования
			unsigned n = EncryptAndDecryptKey(&E, &D);
			unsigned OpenMsg, CryptMsg, AfterDecryptMsg;

			wchar_t array[200];
			cout << "Enter string" << "\n";
			wcin >> array;

			for (int j = 0; array[j] != '\0'; j++) { 
			
				wcout << array[j] << " --> ";
				wchar_t symbol = array[j];
				OpenMsg = (int)array[j];

				Crypt(OpenMsg, E, n, &CryptMsg);
				DeCrypt(CryptMsg, D, n, &AfterDecryptMsg);

				if (OpenMsg == AfterDecryptMsg) {
					printf(" OpenMsg=%u CryptMsg=%u DecryptMsg=%u DecryptSymbol=%wc - OK\n",
						 OpenMsg, CryptMsg, AfterDecryptMsg,(wchar_t)AfterDecryptMsg);
				}
				else {
					printf(" OpenMsg = %u CryptMsg = %u AfterDecryptMsg = %u - Error\n",
						OpenMsg, CryptMsg, (wchar_t)AfterDecryptMsg);
				}
			}
		}
	}
	system("pause");
	return 0;
}
