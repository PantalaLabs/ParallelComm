
#include <Arduino.h>
#include <ParallelComm.h>

// Defina os pinos do barramento de dados, DIR e STROBE
#define SENDERPIN 10
#define RECEIVERPIN 11
#define MAXBUSPINS 8
uint8_t dataBusPins[MAXBUSPINS] = {2, 3, 4, 5, 6, 7, 8, 9};

ParallelComm comm(dataBusPins, SENDERPIN, RECEIVERPIN, SENDER, MAXBUSPINS);

void setup()
{
    Serial.begin(9600);
    comm.begin();
}

void loop()
{
    comm.updateSender(random(110, 120));
}