#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

enum {
	MAILBOX_OK,
	MAILBOX_CREATE,
	MAILBOX_FILE,
	MAILBOX_NUMBER,
	MAILBOX_CS
};

typedef struct _MAILBOX_TITLE
{
	TCHAR FilePath[MAX_PATH];
	DWORD MaxSize, MessageCounts;
} MAILBOX_TITLE, *PMAILBOX_TITLE;

class MAILBOX {
	MAILBOX_TITLE Title;
	DWORD dwError;
	static int count;
	DWORD dwCurSize;
	DWORD dwCS; // ���������� ����
public:
	MAILBOX(LPCTSTR fName, size_t MaxSize = 0);
	MAILBOX& operator+=(LPCTSTR Msg); // ������ ����
	MAILBOX& operator-=(DWORD Number); // �������� ���� �� �������
	DWORD operator()() 
	{ // ʳ������ �����
		return Title.MessageCounts;
	}
	// ʳ������ �����
	DWORD ReadCounts() 
	{
		return Title.MessageCounts;
	}
	// ������� ����� � ������� �������
	DWORD RdMsg(TCHAR *res, DWORD i);
	// ������� �� �����
	void Clear();
	DWORD GetLastError() { return dwError; }
	~MAILBOX() { count--; }
};
// ���������� ����
DWORD GetControlSum(PBYTE pMem, DWORD dwCount);