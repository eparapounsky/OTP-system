# OTP Encryption Server

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
