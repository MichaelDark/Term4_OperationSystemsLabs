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
	DWORD dwCS; // Контрольна сума
public:
	MAILBOX(LPCTSTR fName, size_t MaxSize = 0);
	MAILBOX& operator+=(LPCTSTR Msg); // Додати лист
	MAILBOX& operator-=(DWORD Number); // Видалити лист за номером
	DWORD operator()() 
	{ // Кількість листів
		return Title.MessageCounts;
	}
	// Кількість листів
	DWORD ReadCounts() 
	{
		return Title.MessageCounts;
	}
	// Читання листа з заданим номером
	DWORD RdMsg(TCHAR *res, DWORD i);
	// Знищити усі листи
	void Clear();
	DWORD GetLastError() { return dwError; }
	~MAILBOX() { count--; }
};
// Контрольна сума
DWORD GetControlSum(PBYTE pMem, DWORD dwCount);