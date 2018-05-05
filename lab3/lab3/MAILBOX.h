#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define HEADER_SIZE sizeof(DWORD) * 3

#define SYMBOL_SIZE 2

struct Bounds {
	DWORD firstByte;
	DWORD lastByte;

	DWORD Difference()
	{
		return lastByte - firstByte;
	}
};

class Mailbox {
	TCHAR FilePath[MAX_PATH];
	DWORD _messageCount;
	DWORD _totalSize;
	DWORD _maxSize;
public:
	Mailbox(LPCTSTR fName, size_t MaxSize = 1000);

	void WriteBegin(LPCTSTR Msg);
	void Write(LPCTSTR Msg, DWORD Index);
	void WriteEnd(LPCTSTR Msg);
	void Delete(DWORD Index);
	TCHAR* operator[] (DWORD Index);

	BOOL ExistsIndex(DWORD Index);

	DWORD MessageCount()
	{
		return _messageCount;
	}
	void Clear();
	~Mailbox() {}
private:
	void ShiftToRight(Bounds bounds);
	void ShiftToLeft(Bounds bounds);
	Bounds GetMessageBounds(DWORD Index);
	void WriteAtPosition(DWORD Position, LPCTSTR Msg);
	void UpdateHeader();
};
// Контрольна сума
//DWORD GetControlSum(PBYTE pMem, DWORD dwCount);