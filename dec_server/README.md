# OTP Decryption Server

A multi-threaded TCP server that decrypts ciphertext using the One-Time Pad (OTP) encryption method.

## Overview

The decryption server listens for client connections and performs OTP decryption by:
1. Accepting connections from decryption clients only
2. Receiving ciphertext and encryption key from clients
3. Decrypting the ciphertext using the provided key
4. Returning the plaintext to the client

## Usage

```bash
./dec_server <port_number>
```

**Parameters:**
- `port_number`: The port number on which the server will listen for connections

**Example:**
```bash
./dec_server 57171
```

## Features

- **Client Validation**: Only accepts connections from decryption clients (rejects encryption clients)
- **Multi-threaded**: Handles multiple clients simultaneously using fork()
- **Dynamic Memory Management**: Allocates memory based on message sizes
- **Error Handling**: Comprehensive error checking for network operations and memory allocation
- **Key Length Validation**: Ensures encryption key is at least as long as the ciphertext

## Protocol

1. Client sends identification string "decrypt"
2. Client sends ciphertext size followed by ciphertext
3. Client sends encryption key size followed by encryption key
4. Server validates key length and performs decryption
5. Server sends plaintext size followed by plaintext back to client

## Character Set

Supports uppercase letters A-Z and space character (27 total characters).

## Compilation

```bash
gcc -o dec_server dec_server.c
```
