# OTP Decryption Client

A TCP client that connects to the OTP decryption server to decrypt ciphertext using the One-Time Pad encryption method.

## Overview

The decryption client reads ciphertext and encryption key files, connects to the decryption server, and receives the decrypted plaintext.

## Usage

```bash
./dec_client <ciphertext_file> <key_file> <port_number>
```

**Parameters:**
- `ciphertext_file`: Path to file containing the ciphertext to decrypt
- `key_file`: Path to file containing the encryption key
- `port_number`: Port number of the decryption server

**Example:**
```bash
./dec_client ciphertext.txt mykey.txt 57171
```

## Features

- **File Input**: Reads ciphertext and key from specified files
- **Key Validation**: Ensures encryption key is at least as long as the ciphertext
- **Network Communication**: Connects to localhost decryption server
- **Error Handling**: Comprehensive error checking for file operations and network communication
- **Dynamic Memory Management**: Allocates memory based on file sizes

## Protocol

1. Connects to decryption server on localhost
2. Sends identification string "decrypt"
3. Sends ciphertext size followed by ciphertext
4. Sends encryption key size followed by encryption key
5. Receives plaintext size followed by plaintext
6. Outputs decrypted plaintext to stdout

## Character Set

Supports uppercase letters A-Z and space character (27 total characters).

## Output

The decrypted plaintext is printed to stdout with a newline.

## Compilation

```bash
gcc -o dec_client dec_client.c
```
