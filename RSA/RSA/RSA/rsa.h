#pragma once
#ifdef RSA_EXPORTS
#define RSAAPI extern "C" _declspec (dllexport)
#else
#define RSAAPI extern "C" _declspec (dllimport)
#endif
// ����������
RSAAPI bool Crypt(unsigned OpenMsg, unsigned E, unsigned n, unsigned *CryptMsg);
// �������������
RSAAPI bool DeCrypt(unsigned CryptMsg, unsigned D, unsigned n, unsigned	*OpenMsg);
// ��������� ������
RSAAPI unsigned EncryptAndDecryptKey(unsigned *E, unsigned *D);