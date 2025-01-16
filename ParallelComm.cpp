#include "ParallelComm.h"

ParallelComm::ParallelComm(uint8_t dataBusPins[8], uint8_t sender, uint8_t receiver, DeviceRole deviceRole)
{
    // Configura os pinos do barramento de dados
    for (int i = 0; i < 8; i++)
    {
        dataBus[i] = dataBusPins[i];
    }
    senderPin = sender;
    receiverPin = receiver;
    role = deviceRole;
    // Configura os pinos do barramento conforme o papel do dispositivo
    if (role == SENDER)
    {
        // Master: pinos do barramento são OUTPUT
        for (int i = 0; i < 8; i++)
        {
            SET_PIN_OUTPUT(dataBus[i]);
        }
    }
    else
    {
        // RECEIVER: pinos do barramento são INPUT
        for (int i = 0; i < 8; i++)
        {
            SET_PIN_INPUT(dataBus[i]);
        }
    }

    // Inicializa os estados
    senderState = SENDER_IDLE;
    receiverState = RECEIVER_IDLE;
}

void ParallelComm::begin()
{
    // Configura os pinos de controle (senderPin e receiverPin)
    if (role == SENDER)
    {
        // Master: senderPin é OUTPUT, receiverPin é INPUT
        Serial.println("SENDER:begin");
        SET_PIN_OUTPUT(senderPin);
        SET_PIN_INPUT(receiverPin);
    }
    else
    {
        // RECEIVER: senderPin é INPUT, receiverPin é OUTPUT
        Serial.println("RECEIVER:begin");
        SET_PIN_INPUT(senderPin);
        SET_PIN_OUTPUT(receiverPin);
    }
}

bool ParallelComm::updateSender(uint8_t content = 0)
{
    switch (senderState)
    {
    case SENDER_IDLE:
        if (!READ_PIN(receiverPin)) // RECEIVER recebeu
        {
            WRITE_PIN(senderPin, HIGH);
            senderState = SENDER_WAITING_FIRST_ACK;
            break;
        }

    case SENDER_WAITING_FIRST_ACK:
        if (READ_PIN(receiverPin)) // RECEIVER recebeu
        {
            for (int i = 0; i < 8; i++)
            {
                WRITE_PIN(dataBus[i], (content >> i) & 0x01);
            }
            WRITE_PIN(senderPin, LOW);               // Master desativa o senderPin
            senderState = SENDER_WAITING_SECOND_ACK; // Muda para o estado de espera
        }
        break;

    case SENDER_WAITING_SECOND_ACK:
        if (!READ_PIN(receiverPin)) // RECEIVER recebeu
        {
            senderState = SENDER_IDLE; // Retorna ao estado IDLE
            return true;
        }
        break;
    }
    return false;
}

bool ParallelComm::updateReceiver(uint8_t &content)
{
    switch (receiverState)
    {
    case RECEIVER_IDLE:
        if (READ_PIN(senderPin) == HIGH) // Se o Master iniciar a comunicação
        {
            receiverState = RECEIVER_WAITING_BYTE; // Muda para o estado de espera
            WRITE_PIN(receiverPin, HIGH);
        }
        break;
    case RECEIVER_WAITING_BYTE:
        if (READ_PIN(senderPin) == LOW) // Se o Master desativar o ACK
        {
            content = readByte();
            WRITE_PIN(receiverPin, LOW);
            receiverState = RECEIVER_IDLE; // Retorna ao estado IDLE
            return true;
        }
        break;
    }
    return false;
}

uint8_t ParallelComm::readByte()
{
    uint8_t data = 0;
    for (int i = 0; i < 8; i++)
    {
        data |= (READ_PIN(dataBus[i]) << i);
    }
    return data;
}