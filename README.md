
# ParallelComm Library

It is common to have projects that have pins available, but no CPU cycles. 
One of them is a DSP, or digital signal processor. I was developing one 
and wanted a display with some information, waveform, parameters values, etc..
but I could not call a TFT or OLED display with I2C, or SPI or even Serial 
using another microcontroller. The solution was to use the small amount of 
CPU time available to set digital pins and make the transfer with Non-blocking 
CPU code to another microcontroller connected to the display. 
The library was born. Today it sends up to 2 bytes and you can send 2 
numbers at once: 
first number with the first 8 bits of a potentiometer, 
and the second number, 5 bits with "potentiometer identifier".
If you need more , create a byte control.

As soon as I have the DSP project ready, I will post photos.

Performance,

```
UNO->UNO 
10 bits , pins 2,3,4,5,6,7,8,9
12/13 sender / receiver
4043 msg / seg ~= 32344 bps 
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