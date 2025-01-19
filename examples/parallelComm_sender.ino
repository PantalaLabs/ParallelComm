#include <ParallelComm.h>

#define SENDERPIN 12   // deve ser o mesmo para os dois lados
#define RECEIVERPIN 13 // deve ser o mesmo para os dois lados
uint8_t dataBusPins[] = {2, 3, 4, 5, 6, 7, 8, 9};
uint16_t data;
uint8_t num1;
uint8_t num2;
ParallelComm comm(dataBusPins, SENDERPIN, RECEIVERPIN, SENDER);

void setup()
{
  Serial.begin(9600);
  comm.begin();
}

void loop()
{

  if (comm.updateSender(data))
  {
    // example 1 , one 8 bit number
    // data = random(235, 250);
    // Serial.println(data);

    // example 2 , send one 5 bit number and another 3 bit number
    num1 = random(27, 30);
    num2 = random(5);
    data = PACK_2NUMSTO_XBITS(num1, num2, 5, 8);
    Serial.print("sending: ");
    Serial.print(data, BIN);
    Serial.print(" " + String(data));
    Serial.println(" " + String(num1) + " " + String(num2));
  }
}
