
# ParallelComm Library

Non-blocking library to send/receive data through digital pins between boards or 
between processors on the same board.
Not tested between two cores of the same uC.

## Motivation

CPU cycle hungry projects where it is not possible at all to make any 
CPU blocking serial calls.

In my case, a DSP consumes almost all of the CPU, leaving only a few 
fractions of time available, enough to configure some digital pins and 
transfer data to another core or even another microprocessor on 
another board with Non-blocking protocol.

Like a DSP running in one board and an I2C OLED display on another board.

You can send up to 16 bytes per call or you can split into any number of  
values concatenated up to 16 bytes.

## Dependencies

thanks to ArminJo
https://github.com/ArminJo/digitalWriteFast/blob/master/digitalWriteFast.h

thanks to KurtE
https://github.com/KurtE/UNOR4-stuff/tree/main/libraries/GIGA_digitalWriteFast


## Features

- **UP to 16-bits Parallel Communication**: Uses any bits data bus for transmitting data.
- **Pack and Unpack 2 numbers MACROS**: Pack/Unpack 2 numbers to transfer in one send.
- **Pack and Unpack any numbers function**: Pack/Unpack n numbers to transfer in one send.
- **Two Device Roles**: Supports ONE **Sender** (Master) and ONE **Receiver** (Slave) roles.


```ruby
               ___________________
Sender________/          |||||||| \_______________
              v                   v
                  ^                       ^
                  ________________________
Receiver_________/               |||||||| \____

```