#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define HEADER_SIZE sizeof(DWORD) * 3
#define WRITEMESSAGE_BEGIN 0
#define WRITEMESSAGE_AFTER 1
#define WRITEMESSAGE_END 2

#define SYMBOL_SIZE 2

class Mailbox {
	TCHAR FilePath[MAX_PATH];
	DWORD _messageCount;
	DWORD _totalSize;
	DWORD _maxSize;
public:
	Mailbox(LPCTSTR fName, size_t MaxSize = 1000);
	Mailbox& operator+=(LPCTSTR Msg); 
	Mailbox& operator-=(DWORD Number); 

	void Write(LPCTSTR Msg, DWORD Mode);
	void WriteBegin(LPCTSTR Msg);
	void WriteEnd(LPCTSTR Msg);
	void WriteAt(DWORD Position, LPCTSTR Msg);

	void ShiftToRight(DWORD from, DWORD to);
	DWORD ReadMessage(TCHAR *res, DWORD i);

	DWORD MessageCount() 
	{
		return _messageCount;
	}
	void Clear();
	~Mailbox() {}
};
// Контрольна сума
//DWORD GetControlSum(PBYTE pMem, DWORD dwCount);