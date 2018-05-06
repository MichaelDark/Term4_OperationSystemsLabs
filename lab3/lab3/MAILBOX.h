#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define HEADER_SIZE sizeof(DWORD) * 3
#define SYMBOL_SIZE 2

enum {
	MAILBOX_Success = 1,
	MAILBOX_Error = 0,
	MAILBOX_Fatal = -1
};

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
	DWORD _lastState;
	DWORD countWritten;
	DWORD countRead;
public:
	Mailbox(LPCTSTR fName, size_t MaxSize = 1000);

	DWORD WriteBegin(LPCTSTR Msg);
	DWORD Write(LPCTSTR Msg, DWORD Index);
	DWORD WriteEnd(LPCTSTR Msg);
	DWORD Delete(DWORD Index, TCHAR* res);
	TCHAR* operator[] (DWORD Index);

	BOOL CheckIntegrity();
	BOOL ExistsIndex(DWORD Index);
	DWORD MessageCount();
	DWORD TotalSize();
	DWORD MaxSize();
	DWORD Clear();
	~Mailbox() {}
private:
	HANDLE OpenFileRW();
	DWORD ShiftToRight(HANDLE fileRW, Bounds bounds);
	DWORD ShiftToLeft(HANDLE fileRW, Bounds bounds);
	Bounds GetMessageBounds(HANDLE fileRW, DWORD Index);
	DWORD WriteAtPosition(HANDLE fileRW, DWORD Position, LPCTSTR Msg);
	DWORD UpdateHeader(HANDLE fileRW);
};