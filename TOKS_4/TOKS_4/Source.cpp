#include "csma.h"


using namespace std;

int main()
{
    CSMA_CD cd1("one", 124);
    CSMA_CD cd2("two", 533);
    CSMA_CD cd3("three", 532);
    CSMA_CD cd4("four", 464);
    CSMA_CD cd5("five", 234);
    cd1.beginTransmitting();
    cd2.beginTransmitting();
    cd3.beginTransmitting();
    cd4.beginTransmitting();
    cd5.beginTransmitting();
    cin.get();
    cd1.stop();
    cd2.stop();
    cd3.stop();
    cd4.stop();
    cd5.stop();
    return 0;
}