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

using namespace std;

int _tmain()
{
	_wsetlocale(LC_ALL, _T("Russian"));
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	TCHAR* buff1 = new TCHAR[MAX_PATH];
	TCHAR* buff2 = new TCHAR[MAX_PATH];
	TCHAR* buff3 = new TCHAR[MAX_PATH];
	Mailbox MyBox(_T(" mb.txt"), 1000);

	MyBox.WriteEnd(_T("111"));
	MyBox.WriteEnd(_T("222"));
	MyBox.WriteEnd(_T("333"));
	MyBox.WriteBegin(_T("444"));
	MyBox.WriteBegin(_T("555"));
	MyBox.Write(_T("666"), 5);
	MyBox.Write(_T("777"), 10);
	MyBox.Write(_T("888"), -1);
	MyBox.Write(_T("999"), 1);

	MyBox.Delete(2, buff1);
	_tprintf(_T("Deleted #2: %s\r\n"), buff1);
	MyBox.Delete(2, buff2);
	_tprintf(_T("Deleted #2: %s\r\n"), buff2);

	_tprintf(_T("Mailbox[5]: %s\t\n"), MyBox[5]);
	
	/*
	TCHAR Msg[80];
	for (int i = 0; i < MailBox.ReadCounts(); ++i)
	{
		MailBox.RdMsg(Msg, i);
		_tprintf(_T(" %s\n"), Msg);
	}
	_tprintf(_T(" \n"));
	_tprintf(_T(" \n"));
	MailBox -= 2;
	for (int i = 0; i < MailBox.ReadCounts(); ++i)
	{
		MailBox.RdMsg(Msg, i);
		_tprintf(_T(" %s\n"), Msg);
	}
	*/
	system("pause");
}