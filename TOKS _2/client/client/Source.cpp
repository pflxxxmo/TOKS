#include "Header.h"

void PORT2(HANDLE readEnd, HANDLE writeEnd, HANDLE hFile, HANDLE hExit, HANDLE hWork)
{
    string buffer;
    int size;
    while (WaitForSingleObject(hExit, 1) == WAIT_TIMEOUT)
    {
        if (WaitForSingleObject(hWork, 1) != WAIT_TIMEOUT)
        {
            WaitForSingleObject(writeEnd, INFINITE);
            if (WaitForSingleObject(hExit, 1) != WAIT_TIMEOUT)
                return;
            buffer.clear();
            buffer = read(hFile);
            cout << "Message from COM1: " << buffer << endl;
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

int Client()
{
    HANDLE hFile = CreateFile("COM2", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == NULL)
    {
        printf("Error %d\n", GetLastError());
        exit(GetLastError());
    }
    SetCommMask(hFile, EV_RXCHAR); // ������������� ����� �� ������� �����.
    SetupComm(hFile, 1500, 1500);  // �������������� ���������������� ��������� ��� ��������� ���������� (����������, ����� �����-������)

    COMMTIMEOUTS CommTimeOuts;                     // ���������, ��������������� ��������� ��������� ����������������� �����.
    CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF; // M����������� ����� ��� ��������� ����� ������������ ���� �������� �� ����� �����.
    CommTimeOuts.ReadTotalTimeoutMultiplier = 0;   // ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.
    CommTimeOuts.ReadTotalTimeoutConstant = 1200;  // ���������, ������������, ����� ��������� ������ (������������) ������ ������� ������� ��� �������� ������.
    CommTimeOuts.WriteTotalTimeoutMultiplier = 0;  // ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.
    CommTimeOuts.WriteTotalTimeoutConstant = 1200; // ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.

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

    HANDLE readEnd = OpenEvent(EVENT_ALL_ACCESS, FALSE, "readEnd");
    HANDLE writeEnd = OpenEvent(EVENT_ALL_ACCESS, FALSE, "writeEnd");
    HANDLE hExit = OpenEvent(EVENT_ALL_ACCESS, FALSE, "exit");
    HANDLE hWork = OpenEvent(EVENT_ALL_ACCESS, FALSE, "work");

    PORT2(readEnd, writeEnd, hFile, hExit, hWork);

    CloseHandle(hFile);
    return 0;
}

int main()
{
    system("cls");
    puts("COM2 - port");
    int response = Client();
    if (!response)
        puts("Work ended");
    else
        puts("COM2 can not be opened");
    return 0;
}