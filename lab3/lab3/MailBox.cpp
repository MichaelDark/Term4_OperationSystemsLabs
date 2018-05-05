#include "stdafx.h"
#include <stdio.h>
#include "MailBox.h"
#include "mailBox.h"

DWORD GetControlSumForFile(LPCTSTR fName) 
{
	HANDLE h = CreateFile(fName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (h == INVALID_HANDLE_VALUE) return MAILBOX_FILE;

	DWORD dwSize = GetFileSize(h, 0);
	PBYTE pMem = new BYTE[dwSize];
	DWORD dwCount, dwCS;
	BOOL b = ReadFile(h, pMem, dwSize, & dwCount, 0);
	if (b) {
		dwCS = GetControlSum(pMem, dwSize);
		b = WriteFile(h, & dwCS, sizeof(dwCS), & dwCount, 0);
	}
	delete[]pMem;
	CloseHandle(h);
	return b ? MAILBOX_OK : MAILBOX_FILE;
}

DWORD GetControlSum(PBYTE pMem, DWORD dwCount) 
{
	DWORD dwCS = 0;
	DWORD dwFull = dwCount / 4;
	PDWORD p32Mem = (PDWORD)pMem;
	for (DWORD i = 0; i < dwFull; ++i)
		dwCS += p32Mem[i];
	DWORD dwNotFull = dwCount % 4;
	if (dwNotFull) {
		pMem += dwFull * 4;
		DWORD dwNumber = 0;
		memcpy(& dwNumber, pMem, dwNotFull);
		dwCS += dwNumber;
	}
	return dwCS;
}

int MAILBOX::count = 0;

MAILBOX::MAILBOX(LPCTSTR fName, size_t MaxSize) 
{
	BOOL fileOperationSuccess;
	DWORD dwCount;
	_tcscpy_s(Title.FilePath, fName);
	HANDLE fileHandle = CreateFile(fName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (fileHandle == INVALID_HANDLE_VALUE) 
	{
		if (MaxSize) 
		{
			Title.MaxSize = MaxSize;
			Title.MessageCounts = 0;
			fileHandle = CreateFile(fName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_NEW, 0, 0);

			if (fileHandle == INVALID_HANDLE_VALUE) 
			{
				dwError = MAILBOX_FILE;
				return;
			}
			dwCS = GetControlSum((PBYTE)& Title, sizeof(Title));
			fileOperationSuccess = WriteFile(fileHandle, & Title, sizeof(Title), & dwCount, 0);
			if (fileOperationSuccess)
			{
				fileOperationSuccess = WriteFile(fileHandle, &dwCS, sizeof(dwCS), &dwCount, 0);
			}
			CloseHandle(fileHandle);
			dwCurSize = sizeof(Title) + sizeof(dwCS);
			if (!fileOperationSuccess) 
			{
				dwError = MAILBOX_FILE;
				return;
			}
			dwError = MAILBOX_OK;
		}
		else 
		{
			dwError = MAILBOX_CREATE;
			return;
		}
	}
	else 
	{
		dwCurSize = GetFileSize(fileHandle, 0);
		PBYTE pMem = new BYTE[dwCurSize];
		fileOperationSuccess = ReadFile(fileHandle, pMem, dwCurSize, & dwCount, 0);
		if (fileOperationSuccess) 
		{
			dwCS = *(DWORD*)(pMem + dwCurSize - 4);
			DWORD CalcCS = GetControlSum(pMem, dwCurSize - 4);
			dwError = CalcCS == dwCS ? MAILBOX_OK : MAILBOX_CS;
			PMAILBOX_TITLE pTitle = (PMAILBOX_TITLE)pMem;
			Title = *pTitle;
		}
		else 
		{
			dwError = MAILBOX_FILE;
		}
		delete[]pMem;
		CloseHandle(fileHandle);
	}
}
MAILBOX& MAILBOX::operator+=(LPCTSTR Msg) 
{
	DWORD dwLen = _tcslen(Msg) * sizeof(DWORD);
	dwError = MAILBOX_CREATE;
	if (Title.MaxSize >= dwCurSize + dwLen + sizeof(DWORD)) 
	{
		Title.MessageCounts += 1;
		dwError = MAILBOX_FILE;
		HANDLE h = CreateFile(Title.FilePath, GENERIC_READ |
			GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		BOOL b;
		DWORD dwCount;
		if (h != INVALID_HANDLE_VALUE) 
		{
			b = WriteFile(h, & Title, sizeof(Title), & dwCount, 0);
			if (b) 
			{
				LONG dwHigeOffs = -1;
				SetFilePointer(h, -4, & dwHigeOffs, FILE_END);
				b = WriteFile(h, & dwLen, sizeof(dwLen), & dwCount, 0);
				if (b) b = WriteFile(h, Msg, dwLen, & dwCount, 0);
				CloseHandle(h); h = INVALID_HANDLE_VALUE;
				if (b) 
				{
					dwError = GetControlSumForFile(Title.FilePath);
				}
			}
			if (h != INVALID_HANDLE_VALUE) CloseHandle(h);
		}
	}
	return *this;
}
DWORD MAILBOX::RdMsg(TCHAR *msg, DWORD i) 
{
	BOOL b = TRUE;
	DWORD dwLen, dwCount;
	dwError = MAILBOX_NUMBER;
	if (i < Title.MessageCounts) 
	{
		dwError = MAILBOX_FILE;
		HANDLE h = CreateFile(Title.FilePath, GENERIC_READ,

			FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		if (h != INVALID_HANDLE_VALUE) 
		{
			SetFilePointer(h, sizeof(Title), 0, FILE_BEGIN);
			for (DWORD j = 0; j < i; ++j) 
			{
				b = ReadFile(h, & dwLen, sizeof(dwLen), & dwCount, 0);
				if (!b) break;
				SetFilePointer(h, dwLen, 0, FILE_CURRENT);
			}
			if (b) b = ReadFile(h, & dwLen, sizeof(dwLen), & dwCount, 0);
			if (b && msg)
			{
				b = ReadFile(h, msg, dwLen, & dwCount, 0);
				msg[dwLen / sizeof(TCHAR)] = 0;
			}
			dwError = b ? MAILBOX_OK : MAILBOX_FILE;
			CloseHandle(h);
		}
	}
	return dwLen;
}
MAILBOX& MAILBOX::operator -=(DWORD i) 
{
	dwError = MAILBOX_NUMBER;
	if (i < Title.MessageCounts)
	{
		Title.MessageCounts -= 1;
		dwError = MAILBOX_FILE;
		HANDLE h = CreateFile(Title.FilePath, GENERIC_READ |
			GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		dwCurSize = GetFileSize(h, 0);
		BOOL b;
		DWORD dwCount, dwLen = 0;
		if (h != INVALID_HANDLE_VALUE)
		{
			b = WriteFile(h, & Title, sizeof(Title), & dwCount, 0);
			DWORD dwOld = sizeof(Title), dwNew, dwCnt;
			if (b)
			{
				for (DWORD j = 0; j < i; ++j)
				{
					b = ReadFile(h, & dwLen, sizeof(dwLen), & dwCount, 0);
					if (!b) break;
					dwOld = SetFilePointer(h, dwLen, 0, FILE_CURRENT);
				}
				b = ReadFile(h, & dwLen, sizeof(dwLen), & dwCount, 0);
				dwNew = SetFilePointer(h, dwLen, 0, FILE_CURRENT);
				dwCnt = dwCurSize - dwNew - 4;
				PBYTE pMem = new BYTE[dwCnt];
				SetFilePointer(h, dwNew, 0, FILE_BEGIN);
				b = ReadFile(h, pMem, dwCnt, & dwCount, 0);
				if (b)
				{
					SetFilePointer(h, dwOld, 0, FILE_BEGIN);
					b = WriteFile(h, pMem, dwCnt, & dwCount, 0);
					SetEndOfFile(h);
				}
				delete[]pMem;
				dwError = b ? MAILBOX_OK : MAILBOX_FILE;
			}
			if (h != INVALID_HANDLE_VALUE)
				CloseHandle(h);
			if (b) dwError = GetControlSumForFile(Title.FilePath);
		}
	}
	return *this;
}