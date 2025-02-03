#ifndef PARALLELCOMM_H
#define PARALLELCOMM_H

#include <Arduino.h>

#if (defined(ARDUINO_AVR_MEGA) ||     \
     defined(ARDUINO_AVR_MEGA1280) || \
     defined(ARDUINO_AVR_MEGA2560) || \
     defined(__AVR_ATmega1280__) ||   \
     defined(__AVR_ATmega1281__) ||   \
     defined(__AVR_ATmega2560__) ||   \
     defined(__AVR_ATmega2561__)) ||  \
    defined(ARDUINO_AVR_NANO) ||      \
    defined(ARDUINO_AVR_UNO)
// thanks to ArminJo
// https://github.com/ArminJo/digitalWriteFast/blob/master/digitalWriteFast.h
#include <digitalWriteFast.h>
#define WRITE_PIN(pin, value) digitalWriteFast(pin, value)
#define READ_PIN(pin) digitalReadFast(pin)
#define SET_PIN_MODE(pin, mode) pinMode(pin, mode)
#define SET_PIN_OUTPUT(pin) pinMode(pin, OUTPUT)
#define SET_PIN_INPUT(pin) pinMode(pin, INPUT)

#elif defined(ARDUINO_GIGA) || defined(ARDUINO_PORTENTA_H7_M7)
// thanks to KurtE
// https://github.com/KurtE/UNOR4-stuff/tree/main/libraries/GIGA_digitalWriteFast
#include "GIGA_digitalWriteFast.h"

#define WRITE_PIN(pin, value) digitalWriteFast(pin, value)
#define READ_PIN(pin) digitalReadFast(pin)
#define SET_PIN_MODE(pin, mode) pinMode(pin, mode)
#define SET_PIN_OUTPUT(pin) pinMode(pin, OUTPUT)
#define SET_PIN_INPUT(pin) pinMode(pin, INPUT)

#else
// Fallback for boards that do not support direct register manipulation
#define WRITE_PIN(pin, value) digitalWrite(pin, value)
#define READ_PIN(pin) digitalRead(pin)
#define SET_PIN_MODE(pin, mode) pinMode(pin, mode)
#define SET_PIN_OUTPUT(pin) pinMode(pin, OUTPUT)
#define SET_PIN_INPUT(pin) pinMode(pin, INPUT)

#endif // End of macro definition for specific boards

// why did I use bits_total instead of bits_num1 and bits_num2 ?
// because it is easier to specify the total bit of the channel and work with it
// since it remains invariant throughout the process and you can use a #define for it

// packs 2 numbers of bits_num1 bits for a channel of bits_total bits
#define PACK_2NUMSTO_XBITS(num1, num2, bits_num1, bits_total)          \
    (((num1 & ((1UL << bits_num1) - 1)) << (bits_total - bits_num1)) | \
     (num2 & ((1UL << (bits_total - bits_num1)) - 1)))

// unpacks 2 numbers of bits_num1 bits for a channel of bits_total bits
#define UNPACK_2NUMSFROM_XBITS(packed, num1, num2, bits_num1, bits_total)       \
    do                                                                          \
    {                                                                           \
        num1 = (packed >> (bits_total - bits_num1)) & ((1UL << bits_num1) - 1); \
        num2 = packed & ((1UL << (bits_total - bits_num1)) - 1);                \
    } while (0)

#define WRITE_PIN(pin, value) digitalWrite(pin, value)
#define READ_PIN(pin) digitalRead(pin)
#define SET_PIN_MODE(pin, mode) pinMode(pin, mode)
#define SET_PIN_OUTPUT(pin) pinMode(pin, OUTPUT)
#define SET_PIN_INPUT(pin) pinMode(pin, INPUT)

// Enumerators for communication roles and states
enum DeviceRole
{
    SENDER,
    RECEIVER
};

enum SenderState
{
    SENDER_IDLE,
    SENDER_WAITING_FIRST_ACK,
    SENDER_WAITING_SECOND_ACK
};

enum ReceiverState
{
    RECEIVER_IDLE,
    RECEIVER_WAITING_BYTE
};

// ParallelComm class definition
class ParallelComm
{
private:
    uint8_t *dataBus;    // Data bus pins (dynamically allocated)
    uint8_t dataBusSize; // Number of bits (data bus size)
    uint8_t senderTX;    // Pin controlled by the Sender
    uint8_t receiverTX;  // Pin controlled by the Receiver
    DeviceRole role;     // Device role (Sender/ Receiver)
    SenderState senderState;
    ReceiverState receiverState;

public:
    // Constructor with automatic deduction of the array size
    template <size_t N>
    ParallelComm(uint8_t (&dataBusPins)[N], uint8_t senderTX, uint8_t receiverTX, DeviceRole deviceRole)
    {
        dataBusSize = N;                    // Calculate array size automatically
        dataBus = new uint8_t[dataBusSize]; // Dynamically allocates memory to the bus
        for (int i = 0; i < dataBusSize; i++)
        {
            dataBus[i] = dataBusPins[i];
        }
        senderTX = senderTX;
        receiverTX = receiverTX;
        role = deviceRole;
        senderState = SENDER_IDLE;
        receiverState = RECEIVER_IDLE;
    }

    ~ParallelComm() // Destructor
    {
        delete[] dataBus;
    }

    void begin()
    {
        if (role == SENDER)
        {
            for (int i = 0; i < dataBusSize; i++)
            {
                SET_PIN_OUTPUT(dataBus[i]);
            }
            SET_PIN_OUTPUT(senderTX);
            SET_PIN_INPUT(receiverTX);
        }
        else
        {
            for (int i = 0; i < dataBusSize; i++)
            {
                SET_PIN_INPUT(dataBus[i]);
            }
            SET_PIN_INPUT(senderTX);
            SET_PIN_OUTPUT(receiverTX);
        }
    }

    bool updateSender(uint16_t content = 0)
    {
        switch (senderState)
        {
        case SENDER_IDLE:              // sender : idling
            if (!READ_PIN(receiverTX)) // and receiver idles
            {
                WRITE_PIN(senderTX, HIGH);              // request ack
                senderState = SENDER_WAITING_FIRST_ACK; // state : wait receiver ack
            }
            else
            {
                Serial.println("Error: Receiver must be Idling.");
            }
            break;

        case SENDER_WAITING_FIRST_ACK: // state : wait receiver ack
            if (READ_PIN(receiverTX))  // ack received
            {
                for (int i = 0; i < dataBusSize; i++) // send bits
                {
                    WRITE_PIN(dataBus[i], (content >> i) & 0x01);
                }
                WRITE_PIN(senderTX, LOW);                // request ack
                senderState = SENDER_WAITING_SECOND_ACK; // state : wait second receiver ack
            }
            break;

        case SENDER_WAITING_SECOND_ACK: // state : wait receiver ack
            if (!READ_PIN(receiverTX))  // received an ack from the receiver
            {
                senderState = SENDER_IDLE; // mensge sent
                return true;
            }
            break;
        }
        return false;
    }

    bool updateReceiver(uint16_t &content)
    {
        switch (receiverState)
        {
        case RECEIVER_IDLE:                 // receiver : idling
            if (READ_PIN(senderTX) == HIGH) // receives request for send
            {
                WRITE_PIN(receiverTX, HIGH);           // sends ack
                receiverState = RECEIVER_WAITING_BYTE; // state : wait for message
            }
            break;

        case RECEIVER_WAITING_BYTE:        // state : waiting for message
            if (READ_PIN(senderTX) == LOW) // the bits are on the port , end of message
            {
                content = readData();          // read bits
                WRITE_PIN(receiverTX, LOW);    // sends complete message ack
                receiverState = RECEIVER_IDLE; // wait for next message
                return true;
            }
            break;
        }
        return false;
    }
    // reads the bits on the port and stores them in data
    uint16_t readData()
    {
        uint16_t data = 0;
        for (int i = 0; i < dataBusSize; i++)
        {
            data |= (READ_PIN(dataBus[i]) << i);
        }
        // Applies a mask to ensure that only the first dataBusSize bits are considered
        data &= (1 << dataBusSize) - 1;
        return data;
    }

    // packs several n-bit numbers
    uint16_t packNumbers(const uint16_t numbers[], const uint8_t bits[], uint8_t count)
    {
        uint16_t packed = 0;
        uint8_t totalBits = 0;

        // Serial.print("dataBusSize: ");
        // Serial.println(dataBusSize);

        // Calcula o total de bits necessários
        for (uint8_t i = 0; i < count; i++)
        {
            totalBits += bits[i];
        }

        if (totalBits > 16)
        {
            Serial.println("Error: Total bits exceeds 16.");
            return 0;
        }

        for (uint8_t i = 0; i < count; i++)
        {
            packed |= (numbers[i] & ((1UL << bits[i]) - 1)) << (totalBits - bits[i]);
            totalBits -= bits[i];
        }

        return packed;
    }

    // unpacks several n-bit numbers
    void unpackNumbers(uint16_t packed, uint16_t numbers[], const uint8_t bits[], uint8_t count)
    {
        // Ensures that the packed value is limited to the bus size
        packed &= (1UL << dataBusSize) - 1;

        uint8_t totalBits = 0;

        // Serial.print("dataBusSize: ");
        // Serial.println(dataBusSize);

        // Calculates the total bits required
        for (uint8_t i = 0; i < count; i++)
        {
            totalBits += bits[i];
        }

        if (totalBits > dataBusSize)
        {
            Serial.println("Error: Total bits exceeds bus size.");
            return;
        }

        // unpack numbers
        for (uint8_t i = 0; i < count; i++)
        {
            numbers[i] = (packed >> (totalBits - bits[i])) & ((1UL << bits[i]) - 1);
            totalBits -= bits[i];
        }
    }
};

#endif