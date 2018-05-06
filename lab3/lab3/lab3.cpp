#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include "MailBox.h"
#include <tchar.h>
#include <iostream>

#ifdef UNICODE
#define _tcout wcout
#define _tcin wcin
#else
#define _tcout cout
#define _tcin cin
#endif

#define TRUE_STR _T("TRUE")
#define FALSE_STR _T("FALSE")
#define SUCCESS_STR _T("SUCCESS")
#define ERROR_STR _T("ERROR")
#define FATAL_STR _T("FATAL")

using namespace std;

void WriteState(DWORD state);

int _tmain()
{
	_wsetlocale(LC_ALL, _T("Russian"));
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	TCHAR* buffer = new TCHAR[MAX_PATH];
	Mailbox MyBox(_T("mb.txt"), 1000);
	_tprintf(_T("Integrity: %s\r\n"), MyBox.CheckIntegrity() ? TRUE_STR : FALSE_STR);
	Mailbox MyBox1(_T("mb1.txt"), 1000);
	_tprintf(_T("Integrity: %s\r\n"), MyBox1.CheckIntegrity() ? TRUE_STR : FALSE_STR);
	Mailbox MyBox2(_T("mb2.txt"), 1000);
	_tprintf(_T("Integrity: %s\r\n"), MyBox2.CheckIntegrity() ? TRUE_STR : FALSE_STR);
	Mailbox MyBox3(_T("mb3.txt"), 1000);
	_tprintf(_T("Integrity: %s\r\n"), MyBox3.CheckIntegrity() ? TRUE_STR : FALSE_STR);

	_tprintf(_T("\r\n\r\n===WRITING\r\n\r\n"));
	WriteState( MyBox.WriteEnd(_T("111")) );
	WriteState( MyBox.WriteEnd(_T("222")) );
	WriteState( MyBox.WriteEnd(_T("333")) );
	WriteState( MyBox.WriteBegin(_T("444")) );
	WriteState( MyBox.WriteBegin(_T("555")) );
	WriteState( MyBox.Write(_T("666"), 5) );
	WriteState( MyBox.Write(_T("777"), 10) );
	WriteState( MyBox.Write(_T("888"), -1) );
	WriteState( MyBox.Write(_T("999"), 1) );

	_tprintf(_T("\r\n\r\n===DELITING\r\n\r\n"));
	WriteState( MyBox.Delete(2, buffer) );
	_tprintf(_T("Deleted #2: %s\r\n"), buffer);
	WriteState( MyBox.Delete(2, buffer) );
	_tprintf(_T("Deleted #2: %s\r\n"), buffer);

	for (int i = 1; i <= MyBox.MessageCount(); i++)
	{
		_tprintf(_T("Mailbox[%d]: %s\r\n"), i, MyBox[i]);
	}

	system("pause");
}

void WriteState(DWORD state)
{
	switch (state)
	{
	case MAILBOX_Success:
	{
		_tprintf(_T("State: %s\t\n"), SUCCESS_STR);
		return;
	}
	case MAILBOX_Error:
	{
		_tprintf(_T("State: %s\t\n"), ERROR_STR);
		return;
	}
	case MAILBOX_Fatal:
	{
		_tprintf(_T("State: %s\t\n"), FATAL_STR);
		return;
	}
	}
	return;
}