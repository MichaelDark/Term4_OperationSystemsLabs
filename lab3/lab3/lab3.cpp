#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include "MailBox.h"
#include <tchar.h>
#include <iostream>

using namespace std;
//int MAILBOX::count = 0;
int _tmain()
{
	Mailbox MyBox(_T(" mb.txt"), 1000);

	MyBox.WriteEnd(_T("111"));
	MyBox.WriteEnd(_T("222"));
	MyBox.WriteEnd(_T("333"));
	MyBox.WriteBegin(_T("444"));
	MyBox.WriteBegin(_T("555"));
	MyBox.Write(_T("666"), 3);
	MyBox.Write(_T("777"), 10);
	MyBox.Write(_T("888"), -1);
	MyBox.Write(_T("999"), 1);

	MyBox.Delete(2);
	MyBox.Delete(2);
	
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