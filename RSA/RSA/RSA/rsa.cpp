#include <stdlib.h>
#include "RsaMath.h"
#include "rsa.h"

//Генерація ключів

RSAAPI unsigned EncryptAndDecryptKey(unsigned *E, unsigned *D) {
	int p, q;
	do {
		p = Simple(); q = Simple();
	} while (p == q);
	unsigned n = p * q;
	unsigned fi = (p - 1) * (q - 1);
	for (*E = 3; gcd(*E, fi) != 1; *E += 2);
	for (*D = 3; (*E) * (*D) % fi != 1; *D += 1);
	return n;
}

// Шифрування

RSAAPI bool Crypt(unsigned OpenMsg, unsigned E, unsigned n, unsigned *CryptMsg) {
	if (OpenMsg > n) return false;
	*CryptMsg = PowMod(OpenMsg, E, n);
	return true;
}

// Розшифрування

RSAAPI bool DeCrypt(unsigned CryptMsg, unsigned D, unsigned n, unsigned	*OpenMsg) {
	if (CryptMsg > n) return false;
	*OpenMsg = PowMod(CryptMsg, D, n);
	return true;

}
