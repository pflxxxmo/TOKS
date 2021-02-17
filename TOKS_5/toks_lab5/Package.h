#pragma once
#include "Header.h"

class Package {
private:

	byte token = 0;
	byte destinationADR = 0;
	byte sourceADR = 0;
	unsigned data = 0;
	byte status = 0;

public:
	Package();
	Package(byte, byte, unsigned);
	friend ostream& operator<< (ostream& out, const Package& package);
	byte getToken();
	void setControl(byte);
	byte getDestination();
	void setDestination(byte);
	byte getSource();
	void setSource(byte);
	unsigned getData();
	void setData(unsigned);
	byte getStatus();
	void setStatus(byte);
};

Package::Package() {

	this->token = 1;
}

Package::Package(byte destination, byte source, unsigned data) {

	this->destinationADR = destination;
	this->sourceADR = source;
	this->data = data;
}

ostream& operator<< (ostream& out, const Package& package) {

	out << "\n\tToken:" << (int)package.token << "\n\tSource:" << (int)package.sourceADR << "\n\tDestination:" << (int)package.destinationADR << "\n\tData:" << " 0x" << std::hex << std::uppercase << package.data;
	return out;
}

byte Package::getToken() {
	return this->token;
}

void Package::setControl(byte byte) {
	this->token = byte;
}

byte Package::getDestination() {
	return this->destinationADR;
}

void Package::setDestination(byte byte) {
	this->destinationADR = byte;
}

byte Package::getSource() {
	return this->sourceADR;
}

void Package::setSource(byte byte) {
	this->sourceADR = byte;
}

unsigned Package::getData() {
	return this->data;
}

void Package::setData(unsigned byte) {
	this->data = byte;
}

byte Package::getStatus() {
	return this->status;
}

void Package::setStatus(byte byte) {
	this->status = byte;
}