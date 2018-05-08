#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define HEADER_SIZE sizeof(DWORD) * 4
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
	DWORD _messCount;
	DWORD _totalSize;
	DWORD _maxSize;
	DWORD _checksum;

	DWORD countWritten;
	DWORD countRead;
	BOOL FATAL;
public:
	Mailbox(LPCTSTR fName, size_t MaxSize = 1000);

	//Write at the beginning with shift to right
	//Return: wheter has a fatal error
	BOOL WriteBegin(LPCTSTR Msg);

	//Write at the certain index with shift to right
	//Return: wheter has a fatal error
	BOOL Write(LPCTSTR Msg, DWORD Index);

	//Write at the end after the last element
	//Return: wheter has a fatal error
	BOOL WriteEnd(LPCTSTR Msg);

	//Delete at the certain index
	//Return: wheter has a fatal error
	BOOL Delete(DWORD Index, TCHAR* res);

	//Retrieve message from the certain index
	//Return: message
	TCHAR* operator[] (DWORD Index);


	DWORD MessageCount();
	DWORD TotalSize();
	DWORD MaxSize();
	DWORD Checksum();

	BOOL CheckIntegrity();
	BOOL Clear();

	~Mailbox() {}
private:
	BOOL ReadMailbox(LPCTSTR fName);
	BOOL CreateMailbox(LPCTSTR fName, size_t MaxSize = 1000);

	HANDLE OpenFileRW();
	BOOL WriteAtPosition(HANDLE fileHandle, DWORD Index, LPCTSTR Msg);
	Bounds GetMessageBounds(HANDLE fileRW, DWORD Index);
	DWORD CalculateChecksum(HANDLE fileHandle);

	BOOL CheckHierarchy(HANDLE fileHandle);
	BOOL CheckChecksum(HANDLE fileHandle);
	BOOL CheckIndexPossible(DWORD Index);
	BOOL CheckIndexExist(DWORD Index);
	BOOL CheckOverflow(DWORD messSize);

	BOOL ShiftToRight(HANDLE fileRW, Bounds bounds);
	BOOL ShiftToLeft(HANDLE fileRW, Bounds bounds);

	BOOL UpdateHeader(HANDLE fileRW);
	BOOL UpdateChecksum(HANDLE fileRW);
};