# One-Time Pad (OTP) Encryption System

This system implements the One-Time Pad cipher, a theoretically unbreakable encryption method when used correctly, by securely transmitting plaintext and key data between client and server programs for encryption and decryption. 

The system supports 27 characters: uppercase letters A-Z and the space character. All input text must contain only these characters.

**NOTE: Linux/Unix Only** - This system uses POSIX system calls (`fork()`, `waitpid()`, POSIX sockets) and is designed for Linux/Unix environments.

The system consists of five main components:

- **Key Generator** (`keygen`): Generates random encryption keys of specified length.
- **Encryption Server** (`enc_server`): Multi-threaded server that encrypts plaintext using OTP.
- **Encryption Client** (`enc_client`): Connects to encryption server to encrypt plaintext files.
- **Decryption Server** (`dec_server`): Multi-threaded server that decrypts ciphertext using OTP.
- **Decryption Client** (`dec_client`): Connects to decryption server to decrypt ciphertext files.

## Usage

1. **Build project:**

```bash
chmod +x build.sh
./build.sh
```

2. **Generate a key:**

```bash
./keygen 1024 > key.txt
```

3. **Start the servers:**

```bash
./enc_server 57170 &
./dec_server 57171 &
```

4. **Encrypt a message:**

```bash
echo "THE EAGLE FLIES AT MIDNIGHT" > message.txt
./enc_client message.txt key.txt 57170 > ciphertext.txt
```

5. **Decrypt the message:**

```bash
./dec_client ciphertext.txt key.txt 57171
# Output: THE EAGLE FLIES AT MIDNIGHT
```

## Security Features

- **Client Validation**: Servers only accept connections from appropriate client types
- **Key Length Validation**: Ensures keys are at least as long as the message
- **Character Validation**: Input validation ensures only valid characters are processed
- **Memory Safety**: Proper memory allocation and cleanup
