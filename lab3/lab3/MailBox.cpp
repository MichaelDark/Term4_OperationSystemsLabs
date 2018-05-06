#include "stdafx.h"
#include <stdio.h>
#include "MailBox.h"

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
	_lastState = MAILBOX_Success;
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
		if (fileHandle == INVALID_HANDLE_VALUE) { _lastState = MAILBOX_Fatal; return; }

		_lastState = WriteFile(fileHandle, &_messageCount, sizeof(DWORD), &countWritten, 0);
		_lastState = WriteFile(fileHandle, &_totalSize, sizeof(DWORD), &countWritten, 0);
		_lastState = WriteFile(fileHandle, &_maxSize, sizeof(DWORD), &countWritten, 0);
		if (!_lastState) { _lastState = MAILBOX_Fatal; }

		CloseHandle(fileHandle);
	}
	else 
	{
		_lastState = ReadFile(fileHandle, &_messageCount, 4, &countRead, 0);
		_lastState = ReadFile(fileHandle, &_totalSize, 4, &countRead, 0);
		_lastState = ReadFile(fileHandle, &_maxSize, 4, &countRead, 0);
		if (!_lastState) { _lastState = MAILBOX_Fatal; }

		CloseHandle(fileHandle);
	}
}

DWORD Mailbox::WriteBegin(LPCTSTR Msg)
{
	HANDLE fileHandle = OpenFileRW();
	WriteAtPosition(fileHandle, HEADER_SIZE, Msg);
	CloseHandle(fileHandle);
	_lastState = MAILBOX_Success;
	return _lastState;
}
DWORD Mailbox::Write(LPCTSTR Msg, DWORD Index)
{
	Index--;
	if (!ExistsIndex(Index))
	{
		_lastState = MAILBOX_Error;
		return _lastState;
	}

	HANDLE fileHandle = OpenFileRW();
	Bounds message = GetMessageBounds(fileHandle, Index);
	WriteAtPosition(fileHandle, message.firstByte, Msg);
	CloseHandle(fileHandle);
	_lastState = MAILBOX_Success;
	return _lastState;
}
DWORD Mailbox::WriteEnd(LPCTSTR Msg)
{
	HANDLE fileHandle = OpenFileRW();
	WriteAtPosition(fileHandle, _totalSize, Msg);
	CloseHandle(fileHandle);
	_lastState = MAILBOX_Success;
	return _lastState;
}
DWORD Mailbox::Delete(DWORD Index, TCHAR* res)
{
	Index--;
	if (!ExistsIndex(Index))
	{
		res = new TCHAR[MAX_PATH];
		res[0] = '\0';
		_lastState = MAILBOX_Error;
		return _lastState;
	}

	_tcscpy(res, this->operator[](Index + 1));

	HANDLE fileHandle = OpenFileRW();
	Bounds messagePosition = GetMessageBounds(fileHandle, Index);

	ShiftToLeft(fileHandle, messagePosition);
	_messageCount--;
	UpdateHeader(fileHandle);
	CloseHandle(fileHandle);
	_lastState = MAILBOX_Success;
	return _lastState;
}
TCHAR* Mailbox::operator[] (DWORD Index)
{
	TCHAR* message;
	Index--;
	if (!ExistsIndex(Index))
	{
		message = new TCHAR[MAX_PATH];
		message[0] = '\0';
		_lastState = MAILBOX_Error;
		return message;
	}

	HANDLE fileHandle = OpenFileRW();
	Bounds messagePosition = GetMessageBounds(fileHandle, Index);
	DWORD countWritten;
	DWORD byteLength;

	SetFilePointer(fileHandle, messagePosition.firstByte, 0, FILE_BEGIN);
	ReadFile(fileHandle, &byteLength, sizeof(DWORD), &countWritten, 0);
	DWORD messageSize = (byteLength / SYMBOL_SIZE) + 1;
	message = new TCHAR[messageSize];
	ReadFile(fileHandle, message, byteLength, &countWritten, 0);
	message[messageSize - 1] = _T('\0');
	CloseHandle(fileHandle);

	_lastState = MAILBOX_Success;
	return message;
}

BOOL Mailbox::ExistsIndex(DWORD Index)
{
	return Index >= 0 && Index < _messageCount;
}
DWORD Mailbox::MessageCount()
{
	return _messageCount;
}
DWORD Mailbox::TotalSize()
{
	return _totalSize;
}
DWORD Mailbox::MaxSize()
{
	return _maxSize;
}
DWORD Mailbox::Clear()
{
	HANDLE fileHandle = OpenFileRW();
	ShiftToLeft(fileHandle, { HEADER_SIZE, _totalSize } );
	_messageCount = 0;
	_totalSize = 0;
	UpdateHeader(fileHandle);
	CloseHandle(fileHandle);
	_lastState = MAILBOX_Success;
	return _lastState;
}

HANDLE Mailbox::OpenFileRW()
{
	HANDLE fileHandle = CreateFile(FilePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (fileHandle == INVALID_HANDLE_VALUE) { _lastState = MAILBOX_Fatal; }

	return fileHandle;
}
DWORD Mailbox::ShiftToRight(HANDLE fileHandle, Bounds bounds)
{
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

	_lastState = MAILBOX_Success;
	return _lastState;
}
DWORD Mailbox::ShiftToLeft(HANDLE fileHandle, Bounds bounds)
{
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

	_lastState = MAILBOX_Success;
	return _lastState;
}
Bounds Mailbox::GetMessageBounds(HANDLE fileHandle, DWORD Index)
{
	DWORD currPos = HEADER_SIZE;
	DWORD currSize = 0;
	DWORD countWritten;

	for (int i = 0; i <= Index; i++)
	{
		SetFilePointer(fileHandle, currPos, 0, FILE_BEGIN);
		ReadFile(fileHandle, &currSize, sizeof(DWORD), &countWritten, 0);
		currPos += currSize + sizeof(DWORD);
	}

	_lastState = MAILBOX_Success;
	return { currPos - currSize - sizeof(DWORD), currPos };
}
DWORD Mailbox::WriteAtPosition(HANDLE fileHandle, DWORD Position, LPCTSTR Msg)
{
	DWORD countWritten;
	DWORD messLen = _tcslen(Msg);
	DWORD messLenBytes = messLen * SYMBOL_SIZE;

	if (_maxSize >= _totalSize + messLenBytes + sizeof(DWORD))
	{
		if (Position != _totalSize)
		{
			ShiftToRight(fileHandle, { Position, Position + messLenBytes + sizeof(DWORD) });
		}

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

		UpdateHeader(fileHandle);

		_lastState = MAILBOX_Success;
	}
	else
	{
		_lastState = MAILBOX_Error;
	}
	return _lastState;
}
DWORD Mailbox::UpdateHeader(HANDLE fileHandle)
{
	SetFilePointer(fileHandle, 0, 0, FILE_BEGIN);
	WriteFile(fileHandle, &_messageCount, sizeof(DWORD), &countWritten, 0);
	WriteFile(fileHandle, &_totalSize, sizeof(DWORD), &countWritten, 0);
	_lastState = MAILBOX_Success;
	return _lastState;
}