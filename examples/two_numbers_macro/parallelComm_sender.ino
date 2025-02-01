#include <ParallelComm.h>

#define TX 12 // to RECEIVER RX pin
#define RX 13 // from RECEIVER TX pin
uint8_t dataBusPins[] = {2, 3, 4, 5, 6, 7, 8, 9};
ParallelComm comm(dataBusPins, TX, RX, SENDER);

uint16_t data;
uint8_t num1;
uint8_t num2;

void setup()
{
  Serial.begin(9600);
  comm.begin();
}

void loop()
{

  if (comm.updateSender(data))
  {
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
