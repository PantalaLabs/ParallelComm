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
// obrigado a ArminJo pela biblioteca para AVR
// https://github.com/ArminJo/digitalWriteFast/blob/master/digitalWriteFast.h
#include "digitalWriteFast.h"
#define WRITE_PIN(pin, value) digitalWriteFast(pin, value)
#define READ_PIN(pin) digitalReadFast(pin)
#define SET_PIN_MODE(pin, mode) pinMode(pin, mode)
#define SET_PIN_OUTPUT(pin) pinMode(pin, OUTPUT)
#define SET_PIN_INPUT(pin) pinMode(pin, INPUT)

#elif defined(ARDUINO_GIGA) || defined(ARDUINO_PORTENTA_H7_M7)
// obrigado a KurtE pela biblioteca para GIGA
// https://github.com/KurtE/UNOR4-stuff/tree/main/libraries/GIGA_digitalWriteFast
#include "GIGA_digitalWriteFast.h"

#define WRITE_PIN(pin, value) digitalWriteFast(pin, value)
#define READ_PIN(pin) digitalReadFast(pin)
#define SET_PIN_MODE(pin, mode) pinMode(pin, mode)
#define SET_PIN_OUTPUT(pin) pinMode(pin, OUTPUT)
#define SET_PIN_INPUT(pin) pinMode(pin, INPUT)

#else
// Fallback para placas que não suportam manipulação direta de registradores
#define WRITE_PIN(pin, value) digitalWrite(pin, value)
#define READ_PIN(pin) digitalRead(pin)
#define SET_PIN_MODE(pin, mode) pinMode(pin, mode)
#define SET_PIN_OUTPUT(pin) pinMode(pin, OUTPUT)
#define SET_PIN_INPUT(pin) pinMode(pin, INPUT)

#endif // Fim da definição de macros para placas específicas

// por que usei bits_total ao invés de bits_num1 e bits_num2 ?
// por que é mais fácil estipular o total de bit do canal e trabalhar com ele
// pois permanece invariável durante todo o processo e pod-se usar um #defina prá ele

// empacota 2 numeros de bits_num1 bits para um canal de bits_total bits
#define PACK_2NUMSTO_XBITS(num1, num2, bits_num1, bits_total)          \
    (((num1 & ((1UL << bits_num1) - 1)) << (bits_total - bits_num1)) | \
     (num2 & ((1UL << (bits_total - bits_num1)) - 1)))

// desempacota 2 numeros de bits_num1 bits de um canal de bits_total bits
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
    uint8_t senderTX;    // Pino controlado pelo Master
    uint8_t receiverTX;  // Pino controlado pelo Receiver
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
        case SENDER_IDLE:              // se sender idle
            if (!READ_PIN(receiverTX)) // e receiver idle
            {
                WRITE_PIN(senderTX, HIGH);              // requer ack
                senderState = SENDER_WAITING_FIRST_ACK; // estado : espera ack do receiver
            }
            break;

        case SENDER_WAITING_FIRST_ACK: // se sender esperando ack
            if (READ_PIN(receiverTX))  // e ack recebido
            {
                for (int i = 0; i < dataBusSize; i++) // envia os bits
                {
                    WRITE_PIN(dataBus[i], (content >> i) & 0x01);
                }
                WRITE_PIN(senderTX, LOW);                // requer ack do reciver
                senderState = SENDER_WAITING_SECOND_ACK; // estado : espera segundo ack do receiver
            }
            break;

        case SENDER_WAITING_SECOND_ACK: // se esperando ack do receiver
            if (!READ_PIN(receiverTX))  // e recebeu ack do receiver
            {
                senderState = SENDER_IDLE; // mensgem enviada
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
        case RECEIVER_IDLE:                 // se receiver idle
            if (READ_PIN(senderTX) == HIGH) // e recebe requisição para envio
            {
                WRITE_PIN(receiverTX, HIGH);           // envia ack
                receiverState = RECEIVER_WAITING_BYTE; // estado : espera mensgem do sender
            }
            break;

        case RECEIVER_WAITING_BYTE:        // se receiver esperando mensagem do sender
            if (READ_PIN(senderTX) == LOW) // se já tem os bits na porta e recebe fim de mensagem
            {
                content = readData();          // lê os bits
                WRITE_PIN(receiverTX, LOW);    // envia ack de mensagem completa
                receiverState = RECEIVER_IDLE; // agaurda próxima mensagem
                return true;
            }
            break;
        }
        return false;
    }
    // lê os bits na porta e armazena em data
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

    // empacota varios numeros de n bits
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

    // desempacota varios numeros de n bits
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