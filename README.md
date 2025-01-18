
# ParallelComm Library

It is common to have projects that have pins available, but no CPU cycles. 
One of them is a DSP, or digital signal processor. I was developing one 
and wanted a display with some information, waveform, parameters values, etc..
but I could not call a TFT or OLED display with I2C, or SPI or even Serial 
using another microcontroller. The solution was to use the small amount of 
CPU time available to set digital pins and make the transfer with Non-blocking 
CPU code to another microcontroller connected to the display. 
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
// 6 stages sender machine state
// 1355 msg / seg ~= 21600 bps - 6 stages sender machine state
// 3 stages sender machine state
// 2970 msg / seg ~= 23760 bps - 
// 4160 msg / seg ~= 33280 bps - native port manipulation
// 3400 msg / seg ~= 27200 bps - receiver with digitalWriteFast library (probably due to caller functions)
// 2970 msg / seg ~= 23720 bps - sender and receiver with digitalWriteFast library (probably due to caller functions)
```

## Features

- **UP to 16-bits Parallel Communication**: Uses any bits data bus for transmitting data.
- **Pack and Unpack 2 numbers**: Pack/Unpack 2 numbers to transfer in one send.
- **Two Device Roles**: Supports **Sender** (Master) and **Receiver** (Slave) roles.


```ruby
               ___________________
Sender________/          |||||||| \_______________
              v                   v
                  ^                       ^
                  ________________________
Receiver_________/               |||||||| \____

```