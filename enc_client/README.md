# OTP Encryption Client

A TCP client that connects to the OTP encryption server to encrypt plaintext using the One-Time Pad encryption method.

## Overview

The encryption client reads plaintext and encryption key files, connects to the encryption server, and receives the encrypted ciphertext.

## Usage

```bash
./enc_client <plaintext_file> <key_file> <port_number>
```

**Parameters:**
- `plaintext_file`: Path to file containing the plaintext to encrypt
- `key_file`: Path to file containing the encryption key
- `port_number`: Port number of the encryption server

**Example:**
```bash
./enc_client plaintext.txt mykey.txt 57170
```

## Features

- **File Input**: Reads plaintext and key from specified files
- **Character Validation**: Ensures input files contain only valid characters (A-Z and space)
- **Key Validation**: Ensures encryption key is at least as long as the plaintext
- **Network Communication**: Connects to localhost encryption server
- **Error Handling**: Comprehensive error checking for file operations and network communication
- **Dynamic Memory Management**: Allocates memory based on file sizes

## Protocol

1. Connects to encryption server on localhost
2. Sends identification string "encrypt"
3. Sends plaintext size followed by plaintext
4. Sends encryption key size followed by encryption key
5. Receives ciphertext size followed by ciphertext
6. Outputs encrypted ciphertext to stdout

## Character Set

Supports uppercase letters A-Z and space character (27 total characters). Input files are validated to ensure they contain only these characters.

## Output

The encrypted ciphertext is printed to stdout with a newline.

## Compilation

```bash
gcc -o enc_client enc_client.c
```
