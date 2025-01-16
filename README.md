
# ParallelComm Library

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