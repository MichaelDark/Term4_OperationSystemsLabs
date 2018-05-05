#include <stdlib.h>
#include "RSAMath.h"

// Просте в інтервалі 0..99

unsigned Simple() {

	unsigned x = rand() % 100;
	x |= 1;
	while (1) {
		unsigned p;
		for (unsigned i = 3; ; i += 2) {
			p = i * i;
			if (p > x) break;
			if (x % i == 0) break;
		}
		if (p > x) return x;
		x += 2;
	}
}

// Найбільший загальний дільник
unsigned gcd(unsigned x, unsigned y) {
	while (x && y) {
		if (x > y)x %= y; else y %= x;
	}
	return x + y;
}

// Обчислення ступеня по модулю
unsigned PowMod(unsigned x, unsigned y, unsigned m) {
	unsigned r = 1;
	for (unsigned i = 0; i < y; i++) {
		r *= x;
		r %= m;
	}
		return r;
}