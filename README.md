
# ParallelComm Library

Library to send/receive data through digital pins between boards or 
between processors on the same board

## Motivation

CPU cycle hungry projects where it is not possible at all to make any 
CPU blocking serial calls.

In my case, a DSP consumes almost all of the CPU, leaving only a few 
fractions of time available, enough to configure some digital pins and 
transfer data to another core or even another microprocessor on 
another board with Non-blocking protocol.

Like a DSP running in one core and a OLED display on another.

You can send up to 16 bytes per call or you can send 2 
values concatenated up to 16 bytes.

## Dependencies

thanks to ArminJo
https://github.com/ArminJo/digitalWriteFast/blob/master/digitalWriteFast.h

thanks to KurtE
https://github.com/KurtE/UNOR4-stuff/tree/main/libraries/GIGA_digitalWriteFast


## Performance
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