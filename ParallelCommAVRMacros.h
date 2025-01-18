#ifndef PARALLELCOMMMACROS_H
#define PARALLELCOMMMACROS_H

// Macros para manipulação de pinos (compatíveis com Arduino Uno e Nano)
#if defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_UNO)
// Manipulação de pinos usando registradores para ATmega328P
#define WRITE_PIN(pin, value)            \
    if (pin >= 0 && pin <= 7)            \
    {                                    \
        if (value)                       \
            PORTD |= (1 << pin);         \
        else                             \
            PORTD &= ~(1 << pin);        \
    }                                    \
    else if (pin >= 8 && pin <= 13)      \
    {                                    \
        if (value)                       \
            PORTB |= (1 << (pin - 8));   \
        else                             \
            PORTB &= ~(1 << (pin - 8));  \
    }                                    \
    else if (pin >= A0 && pin <= A5)     \
    {                                    \
        if (value)                       \
            PORTC |= (1 << (pin - A0));  \
        else                             \
            PORTC &= ~(1 << (pin - A0)); \
    }

#define READ_PIN(pin)                                                                                                                  \
    ((pin >= 0 && pin <= 7) ? ((PIND & (1 << pin)) ? HIGH : LOW) : (pin >= 8 && pin <= 13) ? ((PINB & (1 << (pin - 8))) ? HIGH : LOW)  \
                                                               : (pin >= A0 && pin <= A5)  ? ((PINC & (1 << (pin - A0))) ? HIGH : LOW) \
                                                                                           : LOW)

#define SET_PIN_MODE(pin, mode)         \
    if (pin >= 0 && pin <= 7)           \
    {                                   \
        if (mode == OUTPUT)             \
            DDRD |= (1 << pin);         \
        else                            \
            DDRD &= ~(1 << pin);        \
    }                                   \
    else if (pin >= 8 && pin <= 13)     \
    {                                   \
        if (mode == OUTPUT)             \
            DDRB |= (1 << (pin - 8));   \
        else                            \
            DDRB &= ~(1 << (pin - 8));  \
    }                                   \
    else if (pin >= A0 && pin <= A5)    \
    {                                   \
        if (mode == OUTPUT)             \
            DDRC |= (1 << (pin - A0));  \
        else                            \
            DDRC &= ~(1 << (pin - A0)); \
    }

#define SET_PIN_OUTPUT(pin) SET_PIN_MODE(pin, OUTPUT)
#define SET_PIN_INPUT(pin) SET_PIN_MODE(pin, INPUT)

#else

// Fallback para placas que não suportam manipulação direta de registradores
#define WRITE_PIN(pin, value) digitalWrite(pin, value)
#define READ_PIN(pin) digitalRead(pin)
#define SET_PIN_MODE(pin, mode) pinMode(pin, mode)
#define SET_PIN_OUTPUT(pin) pinMode(pin, OUTPUT)
#define SET_PIN_INPUT(pin) pinMode(pin, INPUT)

#endif // Fim da definição de macros para placas específicas
#endif