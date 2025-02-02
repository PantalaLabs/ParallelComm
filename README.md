
# ParallelComm Library

Non-blocking library to send/receive data through digital pins between uC or 
between cores on the same uC (not tested yet).

## Motivation

CPU cycle hungry projects where it is not possible at all to make any 
CPU blocking serial calls.

In my case, a DSP consumes almost all of the CPU, leaving only a few time 
available, enough to configure some digital pins and 
transfer data to another uC connected to a Oled Display.

You can send up to 16 bits per call or split into any number of  
values concatenated up to 16 bits.

## Dependencies

thanks to KurtE
https://github.com/KurtE/UNOR4-stuff/tree/main/libraries/GIGA_digitalWriteFast

thanks to ArminJo
https://github.com/ArminJo/digitalWriteFast/blob/master/digitalWriteFast.h

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