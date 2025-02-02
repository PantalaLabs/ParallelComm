#include <ParallelComm.h>

#define RX 12 // connect to SENDER TX PIN
#define TX 13 // connect to SENDER RX pin
uint8_t dataBusPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
ParallelComm comm(dataBusPins, RX, TX, RECEIVER);

uint16_t packed;             // parallel message
uint16_t unpackedNumbers[3]; // unpacked numbers
uint8_t bits[] = {4, 3, 3};  // bits for each number
uint8_t messages = 3;        // number quantity

uint32_t period;

void setup()
{
  Serial.begin(9600);
  comm.begin();
  period = millis() + 1000;
}

void loop()
{
  if (comm.updateReceiver(packed))
  {
    comm.unpackNumbers(packed, unpackedNumbers, bits, messages);
    Serial.println("unpacked:");
    for (uint8_t i = 0; i < messages; i++)
    {
      Serial.print("# ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(unpackedNumbers[i]);
    }
    delay(3000);
  }
}
