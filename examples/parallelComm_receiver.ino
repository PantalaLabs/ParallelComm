
#include <Arduino.h>
#include <ParallelComm.h>

#define SENDERPIN 10   // deve ser o mesmo para os dois lados
#define RECEIVERPIN 11 // deve ser o mesmo para os dois lados
uint8_t dataBusPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};

ParallelComm comm(dataBusPins, SENDERPIN, RECEIVERPIN, RECEIVER);

uint8_t msgValue = 0;
uint32_t count;
uint32_t period;

void setup()
{
    Serial.begin(9600);
    comm.begin();
    period = millis() + 1000;
}
// 1355 msg / seg ~= 21600 bps - 6 stages sender machine state
// 2970 msg / seg ~= 23760 bps - 3 stages sender machine state
// 4160 msg / seg ~= 33280 bps - 3 stages sender machine state + port manipulation
void loop()
{
    if (comm.updateReceiver(msgValue))
    {
        count++;
        // Serial.println(msgValue); //caso queira ver o valor das mensagens
    }
    if (millis() >= period)
    {
        Serial.println(String(count) + " messages");
        count = 0;
        period = millis() + 1000;
    }
}