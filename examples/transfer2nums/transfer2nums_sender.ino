#define USEDIGITALWRITELIBRARY
#include <ParallelComm.h>

#define SENDERPIN 10   // deve ser o mesmo para os dois lados
#define RECEIVERPIN 11 // deve ser o mesmo para os dois lados
uint8_t dataBusPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};

ParallelComm comm(dataBusPins, SENDERPIN, RECEIVERPIN, SENDER);

void setup()
{
    Serial.begin(9600);
    comm.begin();
}

void loop()
{
    comm.updateSender(PACK_2NUMSTO_8BITS(random(32),random(8),5));
}