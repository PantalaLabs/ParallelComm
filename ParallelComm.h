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
    uint8_t senderTX;   // Pino controlado pelo Master
    uint8_t receiverTX; // Pino controlado pelo Receiver
    DeviceRole role;     // Papel do dispositivo (Master ou Receiver)
    SenderState senderState;
    ReceiverState receiverState;

public:
    // Construtor com dedução automática do tamanho do array
    template <size_t N>
    ParallelComm(uint8_t (&dataBusPins)[N], uint8_t senderTX, uint8_t receiverTX, DeviceRole deviceRole)
    {
        dataBusSize = N;                    // Calcula o tamanho do array automaticamente
        dataBus = new uint8_t[dataBusSize]; // Aloca memória dinamicamente para o barramento
        // Internaliza os pinos do barramento de dados
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

    ~ParallelComm() // Destrutor para liberar memória
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
        case SENDER_IDLE:
            if (!READ_PIN(receiverTX))
            {
                WRITE_PIN(senderTX, HIGH);
                senderState = SENDER_WAITING_FIRST_ACK;
            }
            break;

        case SENDER_WAITING_FIRST_ACK:
            if (READ_PIN(receiverTX))
            {
                for (int i = 0; i < dataBusSize; i++)
                {
                    WRITE_PIN(dataBus[i], (content >> i) & 0x01); // Apenas os primeiros dataBusSize bits são enviados
                }
                WRITE_PIN(senderTX, LOW);
                senderState = SENDER_WAITING_SECOND_ACK;
            }
            break;

        case SENDER_WAITING_SECOND_ACK:
            if (!READ_PIN(receiverTX))
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
            if (READ_PIN(senderTX) == HIGH)
            {
                receiverState = RECEIVER_WAITING_BYTE;
                WRITE_PIN(receiverTX, HIGH);
            }
            break;

        case RECEIVER_WAITING_BYTE:
            if (READ_PIN(senderTX) == LOW)
            {
                content = readData();
                WRITE_PIN(receiverTX, LOW);
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
        // Aplica uma máscara para garantir que apenas os primeiros dataBusSize bits sejam considerados
        data &= (1 << dataBusSize) - 1;
        return data;
    }

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

        // Verifica se o total de bits excede 16 bits
        if (totalBits > 16)
        {
            Serial.println("Erro: Total de bits excede 16.");
            return 0;
        }

        // Empacota os números
        for (uint8_t i = 0; i < count; i++)
        {
            packed |= (numbers[i] & ((1UL << bits[i]) - 1)) << (totalBits - bits[i]);
            totalBits -= bits[i];
        }

        return packed;
    }

    void unpackNumbers(uint16_t packed, uint16_t numbers[], const uint8_t bits[], uint8_t count)
    {
        // Garante que o valor empacotado está limitado ao tamanho do barramento
        packed &= (1UL << dataBusSize) - 1;

        uint8_t totalBits = 0;

        // Serial.print("dataBusSize: ");
        // Serial.println(dataBusSize);

        // Calcula o total de bits necessários
        for (uint8_t i = 0; i < count; i++)
        {
            totalBits += bits[i];
        }

        if (totalBits > dataBusSize)
        {
            Serial.println("Erro: Total de bits excede o tamanho do barramento.");
            return;
        }

        // Desempacota os números
        for (uint8_t i = 0; i < count; i++)
        {
            numbers[i] = (packed >> (totalBits - bits[i])) & ((1UL << bits[i]) - 1);
            totalBits -= bits[i];
        }
    }
};

#endif // Fim de PARALLELCOMM_H