
# ParallelComm Library

It is common to have projects that have pins available, but no CPU cycles. 
One of them is a DSP, or digital signal processor. I was developing one 
and wanted a display with some information, waveform, parameters values, etc..
but I could not call a TFT or OLED display with I2C, or SPI or even Serial 
using another microcontroller. The solution was to use the small amount of 
CPU time available to set digital pins and make the transfer with NON BLOCKING CPU 
to another microcontroller connected to the display. 
The library was born. Today it sends one byte, but for example, 
it can send 2 messages : 
one containing the first 8 bits of a potentiometer, 
and the second message, the last two bits of the potentiometer 
and 5 more more bits of "potentiometer identifier", 
or "value type identifier" and 
there is still one bit missing (we already use 8 + 2 + 5) 
to control the end of the message block (2 messages). 
You must create your control.

As soon as I have the DSP project ready, I will post photos.

Performance,

```
all UNO->UNO
1355 msg / seg ~= 21600 bps - 6 stages sender machine state (1st version)
2970 msg / seg ~= 23760 bps - 3 stages sender machine state (2nd version)
4160 msg / seg ~= 33280 bps - 3 stages sender machine state + UNO port manipulation (actual version)
```

A `ParallelComm` library is designed to facilitate parallel communication between two devices using an 8-bit 
data bus and 2 control pins. 
It supports two roles: **Sender** (Master) and **Receiver** (Slave). 
The library handles the communication protocol, ensuring data is transmitted and received correctly.

A `ParallelComm` library is designed to facilitate parallel communication between two devices using an 8-bit 
data bus and 2 control pins. 
It supports two roles: **Sender** (Master) and **Receiver** (Slave). 
The library handles the communication protocol, ensuring data is transmitted and received correctly.

## Features

- **8-bit Parallel Communication**: Uses an 8-bit data bus for transmitting data.
- **Two Device Roles**: Supports **Sender** (Master) and **Receiver** (Slave) roles.
- **State Management**: Implements state machines for both Sender and Receiver to manage communication states.
- **Easy to Use**: Simple API for sending and receiving data.

## Usage

### Installation

```ruby
               ___________________
Sender________/          |||||||| \_______________
              v                   v
                  ^                       ^
                  ________________________
Receiver_________/               |||||||| \____

```