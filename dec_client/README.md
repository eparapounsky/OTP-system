# OTP Decryption Client

A TCP client that connects to the decryption server to decrypt ciphertext using the One-Time Pad encryption method.

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
