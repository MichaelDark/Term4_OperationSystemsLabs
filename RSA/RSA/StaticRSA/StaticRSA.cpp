// StaticRSA.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "..\RSA\rsa.h"
#include <iostream>
#include <windows.h>
using namespace std;

int main() {

	setlocale(LC_ALL, "Russian");

	unsigned E, D;
	unsigned n = EncryptAndDecryptKey(&E, &D);
	unsigned OpenMsg, CryptMsg, AfterDecryptMsg;

	wchar_t array[200];
	wchar_t uncypedArray[200];
	int language;

	cout << "Выберете язык. Английский - 1, Русский - 2" << "\n";
	wcin >> language;

	if (language == 1) {

		setlocale(LC_ALL, "English");

		HMODULE library = LoadLibrary(_T("EnglishLanguage.dll"));

		if (library != 0) {
			TCHAR Buf[MAX_PATH];
			LoadString(library, 101, Buf, sizeof(Buf));
			cout << Buf << "\n";
			LoadString(library, 102, Buf, sizeof(Buf));
			cout << Buf << "\n";

			wcin >> array;

			LoadString(library, 103, Buf, sizeof(Buf));
			cout << Buf << "\n";

			for (int i = 0; array[i] != '\0'; i++) {

				//wcout << array[i] << " --> ";
				wchar_t symbol = array[i];
				OpenMsg = (int)array[i];

				Crypt(OpenMsg, E, n, &CryptMsg);
				DeCrypt(CryptMsg, D, n, &AfterDecryptMsg);

				if (OpenMsg == AfterDecryptMsg) {
					printf("%u ", CryptMsg + " ");

				uncypedArray[i] == (wchar_t)AfterDecryptMsg;
					/*printf("OpenMsg = %u CryptMsg = %u AfterDecryptMsg = %wc - OK\n",
						OpenMsg, CryptMsg, (wchar_t)AfterDecryptMsg);*/
				}
				else {
					cout << "\n";
				}
			}

			LoadString(library, 104, Buf, sizeof(Buf));
			cout << "\n" << Buf << "\n";
			for (int i = 0; uncypedArray[i] != '\0'; i++) {
				printf("%wc", uncypedArray[i]);
			}
			FreeLibrary(library);
			cout << "\n";
		}
	}

	if (language == 2) {
		
		HMODULE library = LoadLibrary(_T("RussianLanguage.dll"));
		if (library != 0) {
			SetConsoleCP(1251);
			SetConsoleOutputCP(1251);
			TCHAR Buf[MAX_PATH];
			LoadString(library, 101, Buf, sizeof(Buf));
			cout << Buf << "\n";
			LoadString(library, 102, Buf, sizeof(Buf));
			cout << Buf << "\n";

			wcin >> array;

			LoadString(library, 103, Buf, sizeof(Buf));
			cout << Buf << "\n";

			for (int i = 0; array[i] != '\0'; i++) {

				//wcout << array[i] << " --> ";
				wchar_t symbol = array[i];
				OpenMsg = (int)array[i];

				Crypt(OpenMsg, E, n, &CryptMsg);
				DeCrypt(CryptMsg, D, n, &AfterDecryptMsg);

				if (OpenMsg == AfterDecryptMsg) {
					printf("%u ", CryptMsg + " ");

					uncypedArray[i] == (wchar_t)AfterDecryptMsg;
					/*printf("OpenMsg = %u CryptMsg = %u AfterDecryptMsg = %wc - OK\n",
					OpenMsg, CryptMsg, (wchar_t)AfterDecryptMsg);*/
				}
				else {
					cout << "\n";
				}
			}

			LoadString(library, 104, Buf, sizeof(Buf));
			cout << "\n" << Buf << "\n";
			for (int i = 0; uncypedArray[i] != '\0'; i++) {
				printf("%wc", uncypedArray[i]);
			}
			FreeLibrary(library);
			cout << "\n";

		}
	}



	system("pause");
	return 0;
}

