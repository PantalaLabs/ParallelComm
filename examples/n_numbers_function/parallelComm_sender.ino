#include <ParallelComm.h>

#define SENDERPIN 12   // same to Sender and Rceiver
#define RECEIVERPIN 13 // same to Sender and Rceiver
uint8_t dataBusPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
ParallelComm comm(dataBusPins, SENDERPIN, RECEIVERPIN, SENDER);
uint16_t packed;

uint16_t numbers[] = {10, 5, 7}; // numbers to pack
uint8_t bits[] = {4, 3, 3};      // bits for each number
uint8_t count = 3;               // number quantity

void setup()
{
  Serial.begin(9600);
  comm.begin();
  packed = comm.packNumbers(numbers, bits, count);
}

void loop()
{

  if (comm.updateSender(packed))
  {
    Serial.println("Sent: " + String(numbers[0]) + " " + String(numbers[1]) + " " + String(numbers[2]));
  }
}
