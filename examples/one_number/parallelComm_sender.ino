#include <ParallelComm.h>

#define TX 12 // connect to RECEIVER RX pin
#define RX 13 // connect to RECEIVER TX pin
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
    // example 1 , one 8 bit number
    data = random(235, 250);
    Serial.println(data);
  }
}
