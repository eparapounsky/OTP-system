# One-Time Pad (OTP) Encryption System

A complete implementation of the One-Time Pad encryption system with client-server architecture for secure message encryption and decryption.

## Overview

This system implements the One-Time Pad cipher, a theoretically unbreakable encryption method when used correctly. The system consists of five main components:

- **Key Generator** (`keygen`): Generates random encryption keys
- **Encryption Server** (`enc_server`): Multi-threaded server for encrypting plaintext
- **Encryption Client** (`enc_client`): Client for sending plaintext to encryption server
- **Decryption Server** (`dec_server`): Multi-threaded server for decrypting ciphertext
- **Decryption Client** (`dec_client`): Client for sending ciphertext to decryption server

## Quick Start

1. **Compile all components:**

```bash
gcc -o keygen keygen/keygen.c
gcc -o enc_server enc_server/enc_server.c
gcc -o enc_client enc_client/enc_client.c
gcc -o dec_server dec_server/dec_server.c
gcc -o dec_client dec_client/dec_client.c
```

2. **Generate a key:**

```bash
./keygen 1024 > mykey.txt
```

3. **Start the servers:**

```bash
./enc_server 57170 &
./dec_server 57171 &
```

4. **Encrypt a message:**

```bash
echo "HELLO WORLD" > plaintext.txt
./enc_client plaintext.txt mykey.txt 57170 > ciphertext.txt
```

5. **Decrypt the message:**

```bash
./dec_client ciphertext.txt mykey.txt 57171
```

## Components

### Key Generator (`keygen`)

Generates random encryption keys of specified length.

```bash
./keygen <key_length>
```

### Encryption Server (`enc_server`)

Multi-threaded server that encrypts plaintext using OTP.

```bash
./enc_server <port_number>
```

### Encryption Client (`enc_client`)

Connects to encryption server to encrypt plaintext files.

```bash
./enc_client <plaintext_file> <key_file> <port_number>
```

### Decryption Server (`dec_server`)

Multi-threaded server that decrypts ciphertext using OTP.

```bash
./dec_server <port_number>
```

### Decryption Client (`dec_client`)

Connects to decryption server to decrypt ciphertext files.

```bash
./dec_client <ciphertext_file> <key_file> <port_number>
```

## Character Set

The system supports 27 characters:

- Uppercase letters: A-Z (26 characters)
- Space character (1 character)

All input text must contain only these characters.

## Security Features

- **Client Validation**: Servers only accept connections from appropriate client types
- **Key Length Validation**: Ensures keys are at least as long as the message
- **Character Validation**: Input validation ensures only valid characters are processed
- **Memory Safety**: Proper memory allocation and cleanup
- **Network Security**: Local connections only (localhost)

## Example Usage

```bash
# 1. Generate a key
./keygen 256 > secret.key

# 2. Create plaintext
echo "SECRET MESSAGE FOR ALICE" > message.txt

# 3. Start servers
./enc_server 57170 &
./dec_server 57171 &

# 4. Encrypt
./enc_client message.txt secret.key 57170 > encrypted.txt

# 5. Decrypt
./dec_client encrypted.txt secret.key 57171
# Output: SECRET MESSAGE FOR ALICE

# 6. Clean up
rm secret.key message.txt encrypted.txt
```

## Building and Testing

Each component can be built independently:

```bash
cd keygen && gcc -o keygen keygen.c
cd enc_server && gcc -o enc_server enc_server.c
cd enc_client && gcc -o enc_client enc_client.c
cd dec_server && gcc -o dec_server dec_server.c
cd dec_client && gcc -o dec_client dec_client.c
```
