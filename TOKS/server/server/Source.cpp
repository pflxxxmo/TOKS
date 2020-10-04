#include "Header.h"

void PORT1(HANDLE readEnd, HANDLE writeEnd, HANDLE hFile, HANDLE hExit, HANDLE hWork)
{
    string buffer;
    while (WaitForSingleObject(hExit, 1) == WAIT_TIMEOUT)
    {
        if (WaitForSingleObject(hWork, 1) != WAIT_TIMEOUT)
        {
            WaitForSingleObject(writeEnd, INFINITE);
            if (WaitForSingleObject(hExit, 1) != WAIT_TIMEOUT)
                return;
            buffer.clear();
            buffer = read(hFile);
            cout << "Message from COM2: " << buffer << endl;
            SetEvent(readEnd);
        }
        SetEvent(hWork);
        printf("Message: ");
        getline(cin, buffer);
        buffer += '\n';
        if (!write(hFile, buffer))
        {
            SetEvent(hExit);
            SetEvent(writeEnd);
            return;
        }
        else
        {
            SetEvent(writeEnd);
            WaitForSingleObject(readEnd, INFINITE);
        }
    }
}

int Server()
{
    HANDLE readEnd = CreateEvent(NULL, FALSE, FALSE, "readEnd");
    HANDLE writeEnd = CreateEvent(NULL, FALSE, FALSE, "writeEnd");
    HANDLE hExit = CreateEvent(NULL, FALSE, FALSE, "exit");
    HANDLE hWork = CreateEvent(NULL, FALSE, FALSE, "work");

    HANDLE hFile = CreateFile("COM1", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == NULL)
    {
        printf("Error\n");
        exit(GetLastError());
    }
    SetCommMask(hFile, EV_RXCHAR);                      // ������������� ����� �� ������� �����.
    SetupComm(hFile, 1500, 1500);                       // �������������� ���������������� ��������� ��� ��������� ���������� (����������, ����� �����-������)

    COMMTIMEOUTS CommTimeOuts;                          // ���������, ��������������� ��������� ��������� ����������������� �����.
    CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;      // M����������� ����� ��� ��������� ����� ������������ ���� �������� �� ����� �����.
    CommTimeOuts.ReadTotalTimeoutMultiplier = 0;        // ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.
    CommTimeOuts.ReadTotalTimeoutConstant = 1200;       // ���������, ������������, ����� ��������� ������ (������������) ������ ������� ������� ��� �������� ������.
    CommTimeOuts.WriteTotalTimeoutMultiplier = 0;       // ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.
    CommTimeOuts.WriteTotalTimeoutConstant = 1200;      // ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.

    if (!SetCommTimeouts(hFile, &CommTimeOuts))
    {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
        return 1;
    }

    DCB ComDCM;                               // ���������, ��������������� �������� ��������� ����������������� �����.
    memset(&ComDCM, 0, sizeof(ComDCM));       // ��������� ������ ��� ���������.
    ComDCM.DCBlength = sizeof(DCB);           // ������ �����, � ������, ���������.
    GetCommState(hFile, &ComDCM);             // ��������� ������ � ������� ���������� ����������� �������� ��� ���������� ����������.
    ComDCM.BaudRate = setBaudrate();            // �������� �������� ������.
    ComDCM.ByteSize = 8;                      // ���������� ����� �������������� ��� � ������������ � ����������� ������.
    ComDCM.Parity = NOPARITY;                 // ���������� ����� ����� �������� �������� (��� ��������� �����������).
    ComDCM.StopBits = ONESTOPBIT;             // ������ ���������� �������� ��� (���� ���).
    ComDCM.fAbortOnError = TRUE;              // ������ ������������� ���� �������� ������/������ ��� ������������� ������.
    ComDCM.fDtrControl = DTR_CONTROL_DISABLE; // ������ ����� ���������� ������� ��� ������� DTR.
    ComDCM.fRtsControl = RTS_CONTROL_DISABLE; // ������ ����� ���������� ������� ��� ������� RTS.
    ComDCM.fBinary = TRUE;                    // �������� �������� ����� ������.
    ComDCM.fParity = FALSE;                   // �������� ����� �������� ��������.
    ComDCM.fInX = FALSE;                      // ������ ������������� XON/XOFF ���������� ������� ��� ������.
    ComDCM.fOutX = FALSE;                     // ������ ������������� XON/XOFF ���������� ������� ��� ��������.
    ComDCM.XonChar = 0;                       // ������ ������ XON ������������ ��� ��� ������, ��� � ��� ��������.
    ComDCM.XoffChar = (unsigned char)0xFF;    // ������ ������ XOFF ������������ ��� ��� ������, ��� � ��� ��������.
    ComDCM.fErrorChar = FALSE;                // ������ ������, �������������� ��� ������ �������� � ��������� ���������.
    ComDCM.fNull = FALSE;                     // ��������� �� ������������� ������ �������� � ������� �������� �� ������ ���������� ����� ErrorChar.
    ComDCM.fOutxCtsFlow = FALSE;              // �������� ����� �������� �� �������� CTS.
    ComDCM.fOutxDsrFlow = FALSE;              // �������� ����� �������� �� �������� DSR.
    ComDCM.XonLim = 128;                      // ������ ����������� ����� �������� � �������� ������ ����� �������� ������� XON.
    ComDCM.XoffLim = 128;                     // ���������� ������������ ���������� ���� � �������� ������ ����� �������� ������� XOFF.

    if (!SetCommState(hFile, &ComDCM))
    {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
        return 2;
    }

    PORT1(readEnd, writeEnd, hFile, hExit, hWork);

    CloseHandle(hFile);
    return 0;
}

int main()
{
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION procInfo;
    ZeroMemory(&procInfo, sizeof(PROCESS_INFORMATION));
    if (!CreateProcess(NULL, (char*)"D:\\Visual Studio\\5\\TOKS\\client\\Debug\\client.exe", NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &procInfo))
    {
        cout << "CreateProcess failed " << GetLastError() << endl;
        return 0;
    }

    system("cls");
    puts("COM1 - port | Print 'exit' to quit");
    int response = Server();
    if (!response)
        puts("Work ended");
    else
        puts("COM1 can not be opened");
    return 0;
}