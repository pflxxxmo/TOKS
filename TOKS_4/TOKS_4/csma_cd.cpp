#include "csma.h"

using namespace std;

unsigned CSMA_CD::srpd = 0;

#define success 0x0A
#define error 0x0C
#define stopStation 0x0B
#define standart 0x0F


void CSMA_CD::changeColors(int color)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | color));
}

void CSMA_CD::generateMessage(unsigned& msg)
{
	srand(clock() + GetCurrentThreadId());
	unsigned hi = rand();
	unsigned lo = rand();
	msg = 0;
	msg = hi << 16;
	msg |= lo;
}

void CSMA_CD::transmitFunc(CSMA_CD* instance)
{
	while (instance->running)
	{
		Sleep(instance->delay_ms); // CSMA_CD specified transmit period
		unsigned msg;
		generateMessage(msg);
		int try_counter = 0;
		while (true)
		{
			if (!try_counter && CSMA_CD::srpd == JAM_SIGNAL)
				break;

			CSMA_CD::srpd |= msg;
			Sleep(COLISION_DELAY); // Collision window

			if (CSMA_CD::srpd == msg)
			{
				changeColors(success);
				cout << "Transmitting success from " << instance->name << ": 0x" << setfill('0') << setw(8) << std::hex << std::uppercase << srpd << endl;
				CSMA_CD::srpd = 0;
				break;
			}
			else
			{
				CSMA_CD::srpd = JAM_SIGNAL;
				changeColors(error);
				try_counter++;
				cout << "Collision detected from " << instance->name << ", try to resolve: " << try_counter << endl;
				cout << "Transmitting JAM SIGNAL from " << instance->name << endl;
				if (try_counter == MAX_TRIES)
				{
					cout << "Unable to resolve, skipping" << endl;
					break;
				}

				int time_slot = rand() % ((int)pow(2, try_counter) + 1);
				Sleep(time_slot * 100);                                                         // backoff delay
				CSMA_CD::srpd = 0;
			}
		}
	}
}

void CSMA_CD::beginTransmitting()
{
	transmit_thread = new thread(transmitFunc, this);
	running = true;
}

void CSMA_CD::stop()
{
	running = false;
	transmit_thread->join();
	changeColors(stopStation);
	cout << "Station " << name << " stopped" << endl;
	changeColors(standart);
}