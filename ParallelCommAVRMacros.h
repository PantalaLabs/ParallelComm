#ifndef PARALLELCOMMMACROS_H
#define PARALLELCOMMMACROS_H

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

#endif