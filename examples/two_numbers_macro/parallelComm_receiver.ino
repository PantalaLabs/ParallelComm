#include <ParallelComm.h>

#define RX 12   // from SENDER TX PIN
#define TX 13   // to SENDER RX pin
uint8_t dataBusPins[] = {2, 3, 4, 5, 6, 7, 8, 9};

ParallelComm comm(dataBusPins, RX, TX, RECEIVER);

uint16_t msgValue = 0; // importante ser 16 bits
uint8_t num1;
uint8_t num2;
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
    // example 2 , receive one 5 bit number and another 3 bit number
    UNPACK_2NUMSFROM_XBITS(msgValue, num1, num2, 5, 8);

    Serial.print("received: ");
    Serial.print(msgValue, BIN);
    Serial.print(" " + String(msgValue));
    Serial.println(" " + String(num1) + " " + String(num2));

    delay(800);
  }
  if (millis() >= period)
  {
    Serial.println(String(count) + " messages");
    count = 0;
    period = millis() + 1000;
  }
}
