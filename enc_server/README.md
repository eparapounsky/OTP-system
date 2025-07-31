# OTP Encryption Server

A multi-threaded TCP server that encrypts plaintext using the One-Time Pad (OTP) encryption method.

## Overview

The encryption server listens for client connections and performs OTP encryption by:
1. Accepting connections from encryption clients only
2. Receiving plaintext and encryption key from clients
3. Encrypting the plaintext using the provided key
4. Returning the ciphertext to the client

## Usage

```bash
./enc_server <port_number>
```

**Parameters:**
- `port_number`: The port number on which the server will listen for connections

**Example:**
```bash
./enc_server 57170
```

## Features

- **Client Validation**: Only accepts connections from encryption clients (rejects decryption clients)
- **Multi-threaded**: Handles multiple clients simultaneously using fork()
- **Dynamic Memory Management**: Allocates memory based on message sizes
- **Error Handling**: Comprehensive error checking for network operations and memory allocation
- **Key Length Validation**: Ensures encryption key is at least as long as the plaintext

## Protocol

1. Client sends identification string "encrypt"
2. Client sends plaintext size followed by plaintext
3. Client sends encryption key size followed by encryption key
4. Server validates key length and performs encryption
5. Server sends ciphertext size followed by ciphertext back to client

## Character Set

Supports uppercase letters A-Z and space character (27 total characters).

## Compilation

```bash
gcc -o enc_server enc_server.c
```
