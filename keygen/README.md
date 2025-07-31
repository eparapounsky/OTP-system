# OTP Key Generator

A utility that generates random encryption keys for use with the One-Time Pad (OTP) encryption system.

## Overview

The key generator creates cryptographically random keys of a specified length using the allowed character set for the OTP system.

## Usage

```bash
./keygen <key_length>
```

**Parameters:**
- `key_length`: The desired length of the generated key (integer)

**Example:**
```bash
./keygen 256
```

This will generate a 256-character random key.

## Features

- **Random Key Generation**: Uses system time as seed for pseudo-random generation
- **Configurable Length**: Generate keys of any specified length
- **Valid Character Set**: Only generates characters valid for OTP encryption (A-Z and space)
- **Memory Management**: Properly allocates and frees memory for the generated key
- **Error Handling**: Validates command-line arguments and memory allocation

## Character Set

Generates keys using uppercase letters A-Z and space character (27 total characters).

## Output

The generated key is printed to stdout with a newline, making it easy to redirect to a file:

```bash
./keygen 100 > mykey.txt
```

## Key Usage

Generated keys should be:
- At least as long as the plaintext to be encrypted
- Used only once (hence "One-Time Pad")
- Kept secret and secure
- Shared securely between communicating parties

## Compilation

```bash
gcc -o keygen keygen.c
```
