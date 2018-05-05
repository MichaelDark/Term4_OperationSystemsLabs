//#include "rsa.h"
//
//ulong GetPrime()
//{
//	ulong number = rand() % 300 + 200;
//
//	while (!IsPrime(number))
//	{
//		number++;
//	}
//
//	return number;
//}
//
//bool IsPrime(ulong number)
//{
//	for (ulong i = 2; i < sqrt(number); i++)
//	{
//		if (number % i == 0)
//		{
//			return false;
//		}
//	}
//
//	return true;
//}
//
///////////////////////////////////////////////////////////////////////
//// Алгоритм Евклида. Алгоритм для нахождения наибольшего
//// общего делителя двух целых чисел. Используется для проверки
//// чисел на взаимопростоту.
//ulong GCD(ulong a, ulong b)
//{
//	ulong c;
//
//	while (b)
//	{
//		c = a % b;
//		a = b;
//		b = c;
//	}
//
//	return a < 0 ? a * -1 : a;
//}
//
//ulong ModularPow(ulong base, ulong power, ulong modulus)
//{
//	ulong res = 1;
//	while (power) {
//		if (power & 1)
//			res = (res * base) % modulus;
//		base = (base * base) % modulus;
//		power >>= 1;
//	}
//	return res;
//}
//
//void GenerateKeys(ulong * e, ulong * d, ulong * n)
//{
//	srand(time(NULL));
//
//	ulong p = GetPrime();
//	ulong q = GetPrime();
//
//	while (p == q)
//	{
//		q = GetPrime();
//	}
//
//	*n = p * q;
//	ulong phi = (p - 1) * (q - 1);
//
//	ulong d_;
//	*d = 0;
//
//	while (*d != 1)
//	{
//		d_ = rand() % 100;
//		*d = GCD(d_, phi);
//	}
//
//	ulong e_ = 0;
//	*e = 0;
//
//	while (*e != 1)
//	{
//		e_ += 1;
//		*e = (e_ * d_) % phi;
//	}
//
//	*d = d_;
//	*e = e_;
//}
//
//void CryptMessage(char * msg, int length, ulong e, ulong n, ulong * crypto)
//{
//	for (int i = 0;  i < length;  i++)
//	{
//		long symbol = static_cast<ulong>(msg[i]);
//		crypto[i] = ModularPow(symbol, e, n);
//	}
//}
//
//void DecryptMessage(char * msg, int length, ulong d, ulong n, ulong * crypto)
//{
//	for (int i = 0; i < length; i++)
//	{
//		msg[i] = static_cast<char>(ModularPow(crypto[i], d, n));
//	}
//
//	msg[length] = '\0';
//}
