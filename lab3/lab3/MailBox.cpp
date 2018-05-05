#include "stdafx.h"
#include <stdio.h>
#include "MailBox.h"
#include "mailBox.h"

/*
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
*/
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

Mailbox::Mailbox(LPCTSTR filePath, size_t MaxSize) 
{
	DWORD countWritten;
	_tcscpy_s(FilePath, filePath);
	HANDLE fileHandle = CreateFile(filePath, GENERIC_READ, 
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	if (fileHandle == INVALID_HANDLE_VALUE) 
	{
		_messageCount = 0;
		_totalSize = HEADER_SIZE;
		_maxSize = MaxSize;
		fileHandle = CreateFile(filePath, GENERIC_WRITE, 
			FILE_SHARE_READ, 0, CREATE_NEW, 0, 0);

		WriteFile(fileHandle, &_messageCount, sizeof(DWORD), &countWritten, 0);
		WriteFile(fileHandle, &_totalSize, sizeof(DWORD), &countWritten, 0);
		WriteFile(fileHandle, &_maxSize, sizeof(DWORD), &countWritten, 0);

		CloseHandle(fileHandle);
	}
	else 
	{
		ReadFile(fileHandle, &_messageCount, 4, &countWritten, 0);
		ReadFile(fileHandle, &_totalSize, 4, &countWritten, 0);
		ReadFile(fileHandle, &_maxSize, 4, &countWritten, 0);

		CloseHandle(fileHandle);
	}
}

void Mailbox::WriteBegin(LPCTSTR Msg)
{
	WriteAtPosition(HEADER_SIZE, Msg);
}
void Mailbox::Write(LPCTSTR Msg, DWORD Index)
{
	Index--;
	if (!ExistsIndex(Index))
	{
		return;
	}

	Bounds message = GetMessageBounds(Index);

	WriteAtPosition(message.firstByte, Msg);
}
void Mailbox::WriteEnd(LPCTSTR Msg)
{
	WriteAtPosition(_totalSize, Msg);
}
void Mailbox::Delete(DWORD Index)
{
	Index--;
	if (!ExistsIndex(Index))
	{
		return;
	}

	Bounds message = GetMessageBounds(Index);

	ShiftToLeft(message);

	_messageCount--;

	UpdateHeader();
}

BOOL Mailbox::ExistsIndex(DWORD Index)
{
	return Index >= 0 && Index < _messageCount;
}

void Mailbox::WriteAtPosition(DWORD Position, LPCTSTR Msg)
{
	DWORD countWritten;
	DWORD messLen = _tcslen(Msg);
	DWORD messLenBytes = messLen * SYMBOL_SIZE;

	if (_maxSize >= _totalSize + messLenBytes + sizeof(DWORD))
	{
		if (Position != _totalSize)
		{
			ShiftToRight({ Position, Position + messLenBytes + sizeof(DWORD) });
		}

		HANDLE fileHandle = CreateFile(FilePath, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		DWORD currPos = Position;
		_messageCount++;
		_totalSize += messLenBytes + sizeof(DWORD);

		SetFilePointer(fileHandle, currPos, 0, FILE_BEGIN);
		WriteFile(fileHandle, &messLenBytes, sizeof(DWORD), &countWritten, 0);
		currPos += sizeof(DWORD);

		for (int i = 0; i < messLen; i++)
		{
			SetFilePointer(fileHandle, currPos, 0, FILE_BEGIN);
			WriteFile(fileHandle, &Msg[i], SYMBOL_SIZE, &countWritten, 0);
			currPos += SYMBOL_SIZE;
		}

		SetFilePointer(fileHandle, 0, 0, FILE_BEGIN);
		WriteFile(fileHandle, &_messageCount, sizeof(DWORD), &countWritten, 0);
		WriteFile(fileHandle, &_totalSize, sizeof(DWORD), &countWritten, 0);

		CloseHandle(fileHandle);
	}
}
void Mailbox::ShiftToRight(Bounds bounds)
{
	HANDLE fileHandle = CreateFile(FilePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	DWORD leftBound = bounds.firstByte;
	DWORD rightBound = bounds.lastByte;
	DWORD difference = bounds.Difference();
	DWORD countWritten;

	for (int i = _totalSize; i > leftBound; i--)
	{
		BYTE currByte;
		SetFilePointer(fileHandle, i - 1, 0, FILE_BEGIN);
		ReadFile(fileHandle, &currByte, 1, &countWritten, 0);

		SetFilePointer(fileHandle, i + difference - 1, 0, FILE_BEGIN);
		WriteFile(fileHandle, &currByte, 1, &countWritten, 0);
	}

	CloseHandle(fileHandle);
}
void Mailbox::ShiftToLeft(Bounds bounds)
{
	HANDLE fileHandle = CreateFile(FilePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	DWORD leftBound = bounds.firstByte;
	DWORD rightBound = bounds.lastByte;
	DWORD difference = bounds.Difference();
	DWORD countWritten;

	for (int i = leftBound; i < _totalSize; i++)
	{
		BYTE currByte;
		SetFilePointer(fileHandle, i + difference, 0, FILE_BEGIN);
		ReadFile(fileHandle, &currByte, 1, &countWritten, 0);

		SetFilePointer(fileHandle, i, 0, FILE_BEGIN);
		WriteFile(fileHandle, &currByte, 1, &countWritten, 0);
	}

	_totalSize -= difference;
	SetFilePointer(fileHandle, _totalSize, 0, FILE_BEGIN);
	SetEndOfFile(fileHandle);

	CloseHandle(fileHandle);
}

Bounds Mailbox::GetMessageBounds(DWORD Index)
{
	HANDLE fileHandle = CreateFile(FilePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	DWORD currPos = HEADER_SIZE;
	DWORD currSize = 0;
	DWORD countWritten;

	for (int i = 0; i <= Index; i++)
	{
		SetFilePointer(fileHandle, currPos, 0, FILE_BEGIN);
		ReadFile(fileHandle, &currSize, sizeof(DWORD), &countWritten, 0);
		currPos += currSize + sizeof(DWORD);
	}

	CloseHandle(fileHandle);

	return { currPos - currSize - sizeof(DWORD), currPos };
}

void Mailbox::UpdateHeader()
{
	DWORD countWritten;
	HANDLE fileHandle = CreateFile(FilePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	WriteFile(fileHandle, &_messageCount, sizeof(DWORD), &countWritten, 0);
	WriteFile(fileHandle, &_totalSize, sizeof(DWORD), &countWritten, 0);

	CloseHandle(fileHandle);
}

DWORD Mailbox::ReadMessage(TCHAR *msg, DWORD i) 
{
	BOOL b = TRUE;
	DWORD dwLen = 0, dwCount;
	/*dwError = MAILBOX_NUMBER;
	if (i < _header.MessageCounts) 
	{
		dwError = MAILBOX_FILE;
		HANDLE h = CreateFile(_header.FilePath, GENERIC_READ,

			FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		if (h != INVALID_HANDLE_VALUE) 
		{
			SetFilePointer(h, sizeof(_header), 0, FILE_BEGIN);
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
	}*/
	return dwLen;
}

//DELETE
	//dwError = MAILBOX_NUMBER;
	//if (i < _header.MessageCounts)
	//{
	//	_header.MessageCounts -= 1;
	//	dwError = MAILBOX_FILE;
	//	HANDLE h = CreateFile(_header.FilePath, GENERIC_READ |
	//		GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	//	dwCurSize = GetFileSize(h, 0);
	//	BOOL b;
	//	DWORD dwCount, dwLen = 0;
	//	if (h != INVALID_HANDLE_VALUE)
	//	{
	//		b = WriteFile(h, & _header, sizeof(_header), & dwCount, 0);
	//		DWORD dwOld = sizeof(_header), dwNew, dwCnt;
	//		if (b)
	//		{
	//			for (DWORD j = 0; j < i; ++j)
	//			{
	//				b = ReadFile(h, & dwLen, sizeof(dwLen), & dwCount, 0);
	//				if (!b) break;
	//				dwOld = SetFilePointer(h, dwLen, 0, FILE_CURRENT);
	//			}
	//			b = ReadFile(h, & dwLen, sizeof(dwLen), & dwCount, 0);
	//			dwNew = SetFilePointer(h, dwLen, 0, FILE_CURRENT);
	//			dwCnt = dwCurSize - dwNew - 4;
	//			PBYTE pMem = new BYTE[dwCnt];
	//			SetFilePointer(h, dwNew, 0, FILE_BEGIN);
	//			b = ReadFile(h, pMem, dwCnt, & dwCount, 0);
	//			if (b)
	//			{
	//				SetFilePointer(h, dwOld, 0, FILE_BEGIN);
	//				b = WriteFile(h, pMem, dwCnt, & dwCount, 0);
	//				SetEndOfFile(h);
	//			}
	//			delete[]pMem;
	//			dwError = b ? MAILBOX_OK : MAILBOX_FILE;
	//		}
	//		if (h != INVALID_HANDLE_VALUE)
	//			CloseHandle(h);
	//		//if (b) dwError = GetControlSumForFile(_header.FilePath);
	//	}
	//}
	