#pragma once
#include "Header.h"
#include "Package.h"

class Station {
private:
	byte id;
	byte maxDest;
	Station* nextStation;
	Package package;
	bool flag;
	bool active = true;

public:
	Station(byte);
	void setMaxDest(byte);
	void setNext(Station*);
	byte generateDestination();
	unsigned generateData();
	void sendPackage();
	void receivePackage(Package);
	void routine();
	void stopStation();
};


Station::Station(byte id) 
{
	this->id = id;
}

void Station::setMaxDest(byte max) 
{
	this->maxDest = max + 1;
}

void Station::setNext(Station* next)
{
	this->nextStation = next;
}

byte Station::generateDestination() 
{
	byte dst = byte(rand() % maxDest);
	if (dst == id)
		return (id + 1) % maxDest;
	else 
		return dst;
}

unsigned Station::generateData() 
{
	srand(clock() + GetCurrentThreadId());
	unsigned hi = rand();
	unsigned lo = rand();
	unsigned msg = 0;
	msg = hi << 16;
	msg |= lo;
	return msg;
}

void Station::sendPackage()
{
	cout << "Station " << (int)this->id << " sent " << package << endl;
	nextStation->receivePackage(package);
}

void Station::receivePackage(Package package)
{
	this->package = package;
	Sleep(100);
	cout << "Station " << (int)this->id << " received " << package << endl;
	flag = true;
}

void Station::routine() 
{
	while (active) {
		if (flag) {
			if (package.getToken() == 1) 
			{
				cout << "Station " << (int)this->id << " received token" << endl;
				if (rand() % 2) 
				{
					cout << "Station " << (int)this->id << " passing token" << endl;
					sendPackage();
					flag = false;
					continue;
				}
				cout << "Station " << (int)this->id << " sending message" << endl;
				package = Package(generateDestination(), id, generateData());
				sendPackage();
				flag = false;
				while (!flag);
				if (package.getSource() == id)
				{
					if (package.getStatus() == 1)
					{
						cout << "Station " << (int)this->id << " successful delivery package" << endl;
						flag = false;
						package = Package();
						cout << "Station " << (int)this->id << " passing token" << endl;
						sendPackage();
						continue;
					}
					else
						cout << "Station " << (int)this->id << " destination didn't change status" << endl;
				}
				else
				{
					cout << "Station " << (int)this->id << " get wrong package" << endl;
				}
				package = Package();
				cout << "Station " << (int)this->id << " passing token" << endl;
				sendPackage();
			}
			else {
				if (package.getDestination() == id) {
					cout << "\nStation " << (int)this->id << " got message" << endl;
					package.setStatus(1);
				}
				cout << "Station " << (int)this->id << " passing message" << endl;
				sendPackage();
			}
			flag = false;
		}
		Sleep(1000);
	}
}

void Station::stopStation() 
{
	this->active = false;
	this->flag = false;
}

void threadFun(Station* st)
{
	st->routine();
}
