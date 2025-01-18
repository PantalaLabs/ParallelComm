#include <ParallelComm.h>

#define SENDERPIN 10   // deve ser o mesmo para os dois lados
#define RECEIVERPIN 11 // deve ser o mesmo para os dois lados
uint8_t dataBusPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

ParallelComm comm(dataBusPins, SENDERPIN, RECEIVERPIN, RECEIVER);

uint16_t msgValue = 0; // importante ser 16 bits
uint32_t count;
uint32_t period;

void setup()
{
    Serial.begin(9600);
    comm.begin();
    period = millis() + 1000;
}

void loop()
{
    if (comm.updateReceiver(msgValue))
    {
        count++;
        Serial.println(msgValue);
    }
    if (millis() >= period)
    {
        Serial.println(String(count) + " messages");
        count = 0;
        period = millis() + 1000;
    }
}