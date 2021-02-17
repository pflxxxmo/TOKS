#include "Header.h"
#include "Package.h"
#include "Station.h"

int main() {

	Station zero(0);
	Station one(1);
	Station two(2);
	Station three(3);

	one.setMaxDest(3);
	two.setMaxDest(3);
	three.setMaxDest(3);
	zero.setMaxDest(3);

	zero.setNext(&one);
	one.setNext(&two);
	two.setNext(&three);
	three.setNext(&zero);

	thread a(threadFun, &one);
	thread b(threadFun, &two);
	thread c(threadFun, &zero);
	thread d(threadFun, &three);
	
	cin.clear();
	cin.get();

	zero.stopStation();
	one.stopStation();
	two.stopStation();
	three.stopStation();

	a.join();
	b.join();
	c.join();
	d.join();

	return 0;
}