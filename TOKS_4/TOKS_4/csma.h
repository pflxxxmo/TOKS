#pragma once

#include <thread>
#include <windows.h>
#include <iomanip>
#include <iostream>
#include <conio.h>

#define JAM_SIGNAL 0xFFFFFFFF
#define MAX_TRIES 10
#define COLISION_DELAY 10

class CSMA_CD
{
private:
    static unsigned srpd;
    std::thread* transmit_thread;
    bool running;
    int delay_ms;
    std::string name;
    static void transmitFunc(CSMA_CD* instance);
    static void generateMessage(unsigned& msg);
    static void changeColors(int color);

public:
    CSMA_CD(std::string name, int delay_ms) : running(false)
    {
        this->name = name;
        this->delay_ms = delay_ms;
    }
    void beginTransmitting();
    void stop();
};