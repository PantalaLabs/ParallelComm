#ifndef PARALLELCOMM_H
#define PARALLELCOMM_H

#include <Arduino.h>
#include "ParallelCommAVRMacros.h"

#define PACK_2NUMSTO_XBITS(num1, num2, bits_num1, bits_total)          \
    (((num1 & ((1UL << bits_num1) - 1)) << (bits_total - bits_num1)) | \
     (num2 & ((1UL << (bits_total - bits_num1)) - 1)))

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

// Enumeradores para papéis e estados da comunicação
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

// Definição da classe ParallelComm
class ParallelComm
{
private:
    uint8_t *dataBus;    // Pinos do barramento de dados (alocado dinamicamente)
    uint8_t dataBusSize; // Número de bits (tamanho do barramento de dados)
    uint8_t senderPin;   // Pino controlado pelo Master
    uint8_t receiverPin; // Pino controlado pelo Receiver
    DeviceRole role;     // Papel do dispositivo (Master ou Receiver)
    SenderState senderState;
    ReceiverState receiverState;

public:
    // Construtor com dedução automática do tamanho do array
    template <size_t N>
    ParallelComm(uint8_t (&dataBusPins)[N], uint8_t sender, uint8_t receiver, DeviceRole deviceRole)
    {
        dataBusSize = N;                    // Calcula o tamanho do array automaticamente
        dataBus = new uint8_t[dataBusSize]; // Aloca memória dinamicamente para o barramento

        // Configura os pinos do barramento de dados
        for (int i = 0; i < dataBusSize; i++)
        {
            dataBus[i] = dataBusPins[i];
        }
        senderPin = sender;
        receiverPin = receiver;
        role = deviceRole;

        // Configura os pinos conforme o papel
        if (role == SENDER)
        {
            for (int i = 0; i < dataBusSize; i++)
            {
                SET_PIN_OUTPUT(dataBus[i]);
            }
        }
        else
        {
            for (int i = 0; i < dataBusSize; i++)
            {
                SET_PIN_INPUT(dataBus[i]);
            }
        }

        senderState = SENDER_IDLE;
        receiverState = RECEIVER_IDLE;
    }

    ~ParallelComm() // Destrutor para liberar memória
    {
        delete[] dataBus;
    }

    void begin()
    {
        if (role == SENDER)
        {
            SET_PIN_OUTPUT(senderPin);
            SET_PIN_INPUT(receiverPin);
        }
        else
        {
            SET_PIN_INPUT(senderPin);
            SET_PIN_OUTPUT(receiverPin);
        }
    }

    bool updateSender(uint16_t content = 0)
    {
        switch (senderState)
        {
        case SENDER_IDLE:
            if (!READ_PIN(receiverPin))
            {
                WRITE_PIN(senderPin, HIGH);
                senderState = SENDER_WAITING_FIRST_ACK;
            }
            break;

        case SENDER_WAITING_FIRST_ACK:
            if (READ_PIN(receiverPin))
            {
                for (int i = 0; i < dataBusSize; i++)
                {
                    WRITE_PIN(dataBus[i], (content >> i) & 0x01);
                }
                WRITE_PIN(senderPin, LOW);
                senderState = SENDER_WAITING_SECOND_ACK;
            }
            break;

        case SENDER_WAITING_SECOND_ACK:
            if (!READ_PIN(receiverPin))
            {
                senderState = SENDER_IDLE;
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
        case RECEIVER_IDLE:
            if (READ_PIN(senderPin) == HIGH)
            {
                receiverState = RECEIVER_WAITING_BYTE;
                WRITE_PIN(receiverPin, HIGH);
            }
            break;

        case RECEIVER_WAITING_BYTE:
            if (READ_PIN(senderPin) == LOW)
            {
                content = readData();
                WRITE_PIN(receiverPin, LOW);
                receiverState = RECEIVER_IDLE;
                return true;
            }
            break;
        }
        return false;
    }

    uint16_t readData()
    {
        uint16_t data = 0;
        for (int i = 0; i < dataBusSize; i++)
        {
            data |= (READ_PIN(dataBus[i]) << i);
        }
        return data;
    }

#include <Arduino.h>

    // Empacota números em um uint32_t com base nos tamanhos de bits fornecidos
    uint32_t packNumbers(const uint16_t *numbers, const uint8_t *bitSizes, size_t count)
    {
        uint32_t packed = 0;
        uint8_t bitOffset = 0;

        for (size_t i = 0; i < count; i++)
        {
            packed |= ((uint32_t)(numbers[i] & ((1UL << bitSizes[i]) - 1))) << bitOffset;
            bitOffset += bitSizes[i];
        }

        return packed;
    }

    // Desempacota números de um uint32_t com base nos tamanhos de bits fornecidos
    void unpackNumbers(uint32_t packed, uint16_t *numbers, const uint8_t *bitSizes, size_t count)
    {
        uint8_t bitOffset = 0;

        for (size_t i = 0; i < count; i++)
        {
            numbers[i] = (packed >> bitOffset) & ((1UL << bitSizes[i]) - 1);
            bitOffset += bitSizes[i];
        }
    }
};

#endif // Fim de PARALLELCOMM_H
