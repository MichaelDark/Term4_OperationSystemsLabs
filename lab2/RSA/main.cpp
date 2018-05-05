#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include "../RSA_dll/rsa.h"

using namespace std;

int main(int argc, wchar_t **argv)
{
	ulong e, d, n;

	GenerateKeys(&e, &d, &n);

	printf("Keys:\n");
	printf("E: %lu;\n", e);
	printf("D: %lu;\n", d);
	printf("N: %lu;\n", n);

	wchar_t msg[] = L"Hello, it's me...";
	ulong *res = new ulong[wcslen(msg)];
	wchar_t *dec = new wchar_t[wcslen(msg) + 1];

	CryptMessage(msg, wcslen(msg), e, n, res);
	DecryptMessage(dec, wcslen(msg), d, n, res);

	for (int i = 0; i < wcslen(msg); i++)
	{
		printf("%lu ", res[i]);
	}

	printf("\n%s", dec);

	delete[] res;
	delete[] dec;

	getchar();

	return 0;
}