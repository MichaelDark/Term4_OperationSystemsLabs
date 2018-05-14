#include "stdafx.h"
#include <stdio.h>
#include "MailBox.h"

//PUBLIC

Mailbox::Mailbox(LPCTSTR filePath, size_t MaxSize) 
{
	FATAL = FALSE;
	_tcscpy_s(FilePath, filePath);
	HANDLE fileHandle = CreateFile(filePath, GENERIC_READ, 
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	if (fileHandle == INVALID_HANDLE_VALUE) 
	{
		CloseHandle(fileHandle);
		CreateMailbox(filePath, MaxSize);
	}
	else 
	{
		CloseHandle(fileHandle);
		ReadMailbox(filePath);
	}
}

BOOL Mailbox::WriteBegin(LPCTSTR Msg)
{
	HANDLE fileHandle = OpenFileRW();

	if (FATAL)
	{
		CloseHandle(fileHandle);
		return FALSE;
	}

	BOOL success = WriteAtPosition(fileHandle, 0, Msg);
	CloseHandle(fileHandle);
	return success;
}
BOOL Mailbox::Write(LPCTSTR Msg, DWORD Index)
{
	HANDLE fileHandle = OpenFileRW();

	if (FATAL)
	{
		CloseHandle(fileHandle);
		return FALSE;
	}

	BOOL success = WriteAtPosition(fileHandle, Index, Msg);
	CloseHandle(fileHandle);
	return success;
}
BOOL Mailbox::WriteEnd(LPCTSTR Msg)
{
	HANDLE fileHandle = OpenFileRW();

	if (FATAL)
	{
		CloseHandle(fileHandle);
		return FALSE;
	}

	BOOL success = WriteAtPosition(fileHandle, _messCount, Msg);
	CloseHandle(fileHandle);
	return success;
}
BOOL Mailbox::Delete(DWORD Index, TCHAR* res)
{
	if (FATAL ||
		!CheckIntegrity())
	{
		FATAL |= TRUE;
		return FALSE;
	}

	if (!CheckIndexExist(Index))
	{
		return FALSE;
	}

	if (res != nullptr)
		_tcscpy(res, this->operator[](Index));

	HANDLE fileHandle = OpenFileRW();

	Bounds deletingArea = GetMessageBounds(fileHandle, Index);
	ShiftToLeft(fileHandle, deletingArea);

	_messCount--;
	UpdateHeader(fileHandle);
	UpdateChecksum(fileHandle);

	CloseHandle(fileHandle);
	return TRUE;
}
TCHAR* Mailbox::operator[] (DWORD Index)
{
	TCHAR* message;
	DWORD byteLength;

	if (FATAL ||
		!CheckIntegrity() ||
		!CheckIndexExist(Index))
	{
		message = new TCHAR[1];
		message[0] = '\0';
		return message;
	}

	HANDLE fileHandle = OpenFileRW();
	DWORD messagePosition = GetMessageBounds(fileHandle, Index).firstByte;

	SetFilePointer(fileHandle, messagePosition, 0, FILE_BEGIN);
	FATAL |= !ReadFile(fileHandle, &byteLength, sizeof(DWORD), &countRead, 0);

	DWORD messageSize = (byteLength / SYMBOL_SIZE) + 1;
	message = new TCHAR[messageSize];

	FATAL |= !ReadFile(fileHandle, message, byteLength, &countRead, 0);
	message[messageSize - 1] = _T('\0');

	CloseHandle(fileHandle);

	return message;
}

DWORD Mailbox::MessageCount()
{
	return _messCount;
}
DWORD Mailbox::TotalSize()
{
	return _totalSize;
}
DWORD Mailbox::MaxSize()
{
	return _maxSize;
}
DWORD Mailbox::Checksum()
{
	return _checksum;
}

BOOL Mailbox::CheckIntegrity()
{
	HANDLE fileHandle = OpenFileRW();
	if (FATAL ||
		!CheckChecksum(fileHandle) ||
		!CheckHierarchy(fileHandle))
	{
		CloseHandle(fileHandle);
		return FALSE;
	}
	CloseHandle(fileHandle);
	return TRUE;
}
BOOL Mailbox::Clear()
{
	HANDLE fileHandle = OpenFileRW();

	if (FATAL ||
		!CheckChecksum(fileHandle) ||
		!CheckHierarchy(fileHandle))
	{
		CloseHandle(fileHandle);
		return FALSE;
	}

	ShiftToLeft(fileHandle, { HEADER_SIZE, _totalSize });

	_messCount = 0;
	_totalSize = HEADER_SIZE;

	UpdateHeader(fileHandle);
	UpdateChecksum(fileHandle);
	CloseHandle(fileHandle);

	return TRUE;
}

//PRIVATE
BOOL Mailbox::CreateMailbox(LPCTSTR fName, size_t MaxSize)
{
	_checksum = 0;
	_messCount = 0;
	_totalSize = HEADER_SIZE;
	_maxSize = MaxSize;

	if (_maxSize < HEADER_SIZE) { FATAL |= TRUE; return FALSE; }

	HANDLE fileHandle = CreateFile(FilePath, GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 0, CREATE_NEW, 0, 0);
	if (fileHandle == INVALID_HANDLE_VALUE) { FATAL |= TRUE; CloseHandle(fileHandle); return FALSE; }

	FATAL |= !WriteFile(fileHandle, &_checksum, sizeof(DWORD), &countWritten, 0);
	FATAL |= !WriteFile(fileHandle, &_messCount, sizeof(DWORD), &countWritten, 0);
	FATAL |= !WriteFile(fileHandle, &_totalSize, sizeof(DWORD), &countWritten, 0);
	FATAL |= !WriteFile(fileHandle, &_maxSize, sizeof(DWORD), &countWritten, 0);
	if (sizeof(DWORD) != countWritten) { FATAL |= TRUE; CloseHandle(fileHandle); return FALSE; }

	_checksum = CalculateChecksum(fileHandle);

	SetFilePointer(fileHandle, 0, 0, FILE_BEGIN);
	FATAL |= !WriteFile(fileHandle, &_checksum, sizeof(DWORD), &countWritten, 0);
	if (sizeof(DWORD) != countWritten) { FATAL |= TRUE; CloseHandle(fileHandle); return FALSE; }

	CloseHandle(fileHandle);

	return TRUE;
}
BOOL Mailbox::ReadMailbox(LPCTSTR fName)
{
	HANDLE fileHandle = OpenFileRW();

	if (FATAL)
	{
		CloseHandle(fileHandle);
		return FALSE;
	}

	FATAL |= !ReadFile(fileHandle, &_checksum, 4, &countRead, 0);
	FATAL |= !ReadFile(fileHandle, &_messCount, 4, &countRead, 0);
	FATAL |= !ReadFile(fileHandle, &_totalSize, 4, &countRead, 0);
	FATAL |= !ReadFile(fileHandle, &_maxSize, 4, &countRead, 0);
	if (sizeof(DWORD) != countRead) { FATAL |= TRUE; CloseHandle(fileHandle); return FALSE; }

	if (CalculateChecksum(fileHandle) != _checksum) { FATAL |= TRUE; CloseHandle(fileHandle); return FALSE; }

	CloseHandle(fileHandle);

	return TRUE;
}

HANDLE Mailbox::OpenFileRW()
{
	HANDLE fileHandle = CreateFile(FilePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (fileHandle == INVALID_HANDLE_VALUE) { FATAL |= TRUE; CloseHandle(fileHandle); }

	return fileHandle;
}
BOOL Mailbox::WriteAtPosition(HANDLE fileHandle, DWORD Index, LPCTSTR Msg)
{
	DWORD messLen = _tcslen(Msg);
	DWORD messLenBytes = messLen * SYMBOL_SIZE;

	if (FATAL ||
		!CheckChecksum(fileHandle) ||
		!CheckHierarchy(fileHandle) ||
		!CheckOverflow(messLenBytes + sizeof(DWORD)) ||
		!CheckIndexPossible(Index))
	{
		return FALSE;
	}

	DWORD position = GetMessageBounds(fileHandle, Index).firstByte;
	ShiftToRight(fileHandle, { position, position + messLenBytes + sizeof(DWORD) });

	_messCount++;
	_totalSize += messLenBytes + sizeof(DWORD);

	SetFilePointer(fileHandle, position, 0, FILE_BEGIN);
	FATAL |= !WriteFile(fileHandle, &messLenBytes, sizeof(DWORD), &countWritten, 0);

	for (int i = 0; i < messLen; i++)
	{
		FATAL |= !WriteFile(fileHandle, &Msg[i], SYMBOL_SIZE, &countWritten, 0);
	}

	UpdateHeader(fileHandle);
	UpdateChecksum(fileHandle);

	return TRUE;
}
Bounds Mailbox::GetMessageBounds(HANDLE fileHandle, DWORD Index)
{
	DWORD currPos = HEADER_SIZE;
	DWORD currSize = 0;

	for (int i = 0; i <= Index; i++)
	{
		SetFilePointer(fileHandle, currPos, 0, FILE_BEGIN);
		FATAL |= !ReadFile(fileHandle, &currSize, sizeof(DWORD), &countRead, 0);
		currPos += currSize + sizeof(DWORD);
	}

	return { currPos - currSize - sizeof(DWORD), currPos };
}
DWORD Mailbox::CalculateChecksum(HANDLE fileHandle)
{
	DWORD fileSize = GetFileSize(fileHandle, 0) - 4;
	DWORD wordCount = fileSize / 4;
	DWORD controlSum = 0;

	SetFilePointer(fileHandle, 4, 0, FILE_BEGIN);
	for (DWORD i = 0; i < wordCount; ++i)
	{
		DWORD currWord = 0;
		FATAL |= !ReadFile(fileHandle, &currWord, sizeof(DWORD), &countRead, 0);
		controlSum += currWord;
	}

	DWORD remBytesCount = fileSize % 4;
	if (remBytesCount) {
		DWORD currWord = 0;
		FATAL |= !ReadFile(fileHandle, &currWord, remBytesCount, &countRead, 0);
		controlSum += currWord;
	}

	return controlSum;
}


BOOL Mailbox::CheckHierarchy(HANDLE fileHandle)
{
	DWORD checksum = -1, messCount = -1, totalSize = -1, maxSize = -1;

	SetFilePointer(fileHandle, 0, 0, FILE_BEGIN);
	FATAL |= !ReadFile(fileHandle, &checksum, sizeof(DWORD), &countRead, 0);
	FATAL |= !ReadFile(fileHandle, &messCount, sizeof(DWORD), &countRead, 0);
	FATAL |= !ReadFile(fileHandle, &totalSize, sizeof(DWORD), &countRead, 0);
	FATAL |= !ReadFile(fileHandle, &maxSize, sizeof(DWORD), &countRead, 0);
	if (messCount < 0 || totalSize < 0 || maxSize < 0 || totalSize > maxSize) { FATAL |= TRUE; }

	for (int i = 0; i < messCount; i++)
	{
		DWORD currMessSize = -1;

		FATAL |= !ReadFile(fileHandle, &currMessSize, sizeof(DWORD), &countRead, 0);

		TCHAR* currMess;
		DWORD messageSize = (currMessSize / SYMBOL_SIZE);
		currMess = new TCHAR[messageSize];
		FATAL |= !ReadFile(fileHandle, currMess, currMessSize, &countRead, 0);
		delete[] currMess;
		if (currMessSize != countRead) { FATAL |= TRUE; }
	}

	return !FATAL;
}
BOOL Mailbox::CheckChecksum(HANDLE fileHandle)
{
	if (_checksum != CalculateChecksum(fileHandle)) { FATAL |= TRUE; }

	return !FATAL;
}
BOOL Mailbox::CheckIndexPossible(DWORD Index)
{
	if (Index >= 0 && Index <= _messCount)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL Mailbox::CheckIndexExist(DWORD Index)
{
	if (Index >= 0 && Index < _messCount)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL Mailbox::CheckOverflow(DWORD messSize)
{
	if (_maxSize >= _totalSize + messSize)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Mailbox::ShiftToRight(HANDLE fileHandle, Bounds bounds)
{
	DWORD leftBound = bounds.firstByte;
	DWORD rightBound = bounds.lastByte;
	DWORD difference = bounds.Difference();
	BYTE currByte;

	for (int i = GetFileSize(fileHandle, 0); i > leftBound; i--)
	{
		SetFilePointer(fileHandle, i - 1, 0, FILE_BEGIN);
		FATAL |= !ReadFile(fileHandle, &currByte, 1, &countRead, 0);

		SetFilePointer(fileHandle, i + difference - 1, 0, FILE_BEGIN);
		FATAL |= !WriteFile(fileHandle, &currByte, 1, &countWritten, 0);
	}

	return TRUE;
}
BOOL Mailbox::ShiftToLeft(HANDLE fileHandle, Bounds bounds)
{
	DWORD leftBound = bounds.firstByte;
	DWORD rightBound = bounds.lastByte;
	DWORD difference = bounds.Difference();
	BYTE currByte;

	for (int i = leftBound; i < GetFileSize(fileHandle, 0); i++)
	{
		SetFilePointer(fileHandle, i + difference, 0, FILE_BEGIN);
		FATAL |= !ReadFile(fileHandle, &currByte, 1, &countRead, 0);

		SetFilePointer(fileHandle, i, 0, FILE_BEGIN);
		FATAL |= !WriteFile(fileHandle, &currByte, 1, &countWritten, 0);
	}

	_totalSize -= difference;

	SetFilePointer(fileHandle, _totalSize, 0, FILE_BEGIN);
	FATAL |= !SetEndOfFile(fileHandle);

	return TRUE;
}

BOOL Mailbox::UpdateHeader(HANDLE fileHandle)
{
	SetFilePointer(fileHandle, 4, 0, FILE_BEGIN);
	FATAL |= !WriteFile(fileHandle, &_messCount, sizeof(DWORD), &countWritten, 0);
	FATAL |= !WriteFile(fileHandle, &_totalSize, sizeof(DWORD), &countWritten, 0);

	return !FATAL;
}
BOOL Mailbox::UpdateChecksum(HANDLE fileHandle)
{
	_checksum = CalculateChecksum(fileHandle);

	SetFilePointer(fileHandle, 0, 0, FILE_BEGIN);
	FATAL |= !WriteFile(fileHandle, &_checksum, sizeof(DWORD), &countWritten, 0);
	return !FATAL;
}