#include "Header.h"


string read(HANDLE hFile)
{
	char buf = '\0';
	string msg;

	DWORD bytesReaded;
	do
	{
		if (!ReadFile(hFile, &buf, 1, &bytesReaded, NULL))
			cout << GetLastError() << endl;
		if (buf == '\n') break;
		msg += buf;
	} while (1);
	return msg;
}


bool write(HANDLE hFile, string buffer)
{
	int i = 0;
	cout << endl;
	while (i < buffer.size())
	{
		WriteFile(hFile, &(buffer[i++]), 1, NULL, NULL);
	}

	if (!strcmp(buffer.c_str(), "exit\n"))
		return false;
	else
		return true;
}

DWORD setBaudrate()
{
	DWORD baudRate;

	cout << "Select baudrate:" << endl << "0. 110" << endl << "1. 9600" << endl << "2. 14400" << endl << "3. 19200" << endl << "4. 38400" << endl << "5. 56000" << endl << "6. 256000" << endl;
	switch (_getch()) {

	case('0'):
		system("cls");
		baudRate = CBR_110;
		break;

	case('1'):
		system("cls");
		baudRate = CBR_9600;
		break;

	case('2'):
		system("cls");
		baudRate = CBR_14400;
		break;

	case('3'):
		system("cls");
		baudRate = CBR_19200;
		break;

	case('4'):
		system("cls");
		baudRate = CBR_38400;
		break;

	case('5'):
		system("cls");
		baudRate = CBR_56000;
		break;

	case('6'):
		system("cls");
		baudRate = CBR_256000;
		break;

	default:
		system("cls");
		baudRate = CBR_19200;
		cout << "Wrong input. Default 19200 baudrate is set" << endl;
		break;

	}
	return baudRate;
}